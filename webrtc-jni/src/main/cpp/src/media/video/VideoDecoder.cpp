/*
 * Copyright 2023 Alex Andres
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "media/video/VideoDecoder.h"
#include <Exception.h>

#include "common_video/include/video_frame_buffer.h"
#include "rtc_base/logging.h"

extern "C"
{
#include "libavutil/imgutils.h"
}

namespace jni
{
	const size_t kYPlaneIndex = 0;
	const size_t kUPlaneIndex = 1;
	const size_t kVPlaneIndex = 2;


	VideoDecoder::VideoDecoder(AVStream * stream) :
		Decoder(stream),
		sink(nullptr),
		buffer_pool(true)
	{
	}

	VideoDecoder::~VideoDecoder()
    {
		close();
    }

	void VideoDecoder::open()
	{
		Decoder::open();

		// Used by FFmpeg to get buffers to store decoded frames in.
		context->get_buffer2 = AVGetBuffer2;
		context->opaque = this;
	}

	void VideoDecoder::close()
	{
		Decoder::close();
	}

	void VideoDecoder::flush()
    {
		Decoder::flush();
    }

	void VideoDecoder::setSink(VideoFrameSink * sink)
	{
		const std::scoped_lock lock{ mutex };

		this->sink = sink;
	}

	void VideoDecoder::removeSink(VideoFrameSink * sink)
	{
		const std::scoped_lock lock{ mutex };

		this->sink = nullptr;
	}

	bool VideoDecoder::processFrame(AVFrame * frame)
	{
		printf("video time: %f\n",
			context->frame_num * av_q2d(stream->time_base));
		fflush(NULL);

		if (sink) {
			int64_t frame_ts = frame->best_effort_timestamp;
			int64_t timestamp = static_cast<int64_t>(frame_ts * av_q2d(stream->time_base) * AV_TIME_BASE);

			// Obtain the |video_frame| containing the decoded image.
			webrtc::VideoFrame * input_frame = static_cast<webrtc::VideoFrame *>(av_buffer_get_opaque(frame->buf[0]));

			RTC_DCHECK(input_frame);

			rtc::scoped_refptr<webrtc::VideoFrameBuffer> frame_buffer = input_frame->video_frame_buffer();

			const webrtc::I420BufferInterface * i420_buffer = frame_buffer->GetI420();

			// When needed, FFmpeg applies cropping by moving plane pointers and adjusting
			// frame width/height. Ensure that cropped buffers lie within the allocated memory.
			RTC_DCHECK_LE(frame->width, i420_buffer->width());
			RTC_DCHECK_LE(frame->height, i420_buffer->height());
			RTC_DCHECK_GE(frame->data[kYPlaneIndex], i420_buffer->DataY());
			RTC_DCHECK_LE(frame->data[kYPlaneIndex] + frame->linesize[kYPlaneIndex] * frame->height,
				i420_buffer->DataY() + i420_buffer->StrideY() * i420_buffer->height());
			RTC_DCHECK_GE(frame->data[kUPlaneIndex], i420_buffer->DataU());
			RTC_DCHECK_LE(frame->data[kUPlaneIndex] + frame->linesize[kUPlaneIndex] * frame->height / 2,
				i420_buffer->DataU() + i420_buffer->StrideU() * i420_buffer->height() / 2);
			RTC_DCHECK_GE(frame->data[kVPlaneIndex], i420_buffer->DataV());
			RTC_DCHECK_LE(frame->data[kVPlaneIndex] + frame->linesize[kVPlaneIndex] * frame->height / 2,
				i420_buffer->DataV() + i420_buffer->StrideV() * i420_buffer->height() / 2);

			rtc::scoped_refptr<webrtc::VideoFrameBuffer> cropped_buffer = webrtc::WrapI420Buffer(
				frame->width, frame->height,
				frame->data[kYPlaneIndex], frame->linesize[kYPlaneIndex],
				frame->data[kUPlaneIndex], frame->linesize[kUPlaneIndex],
				frame->data[kVPlaneIndex], frame->linesize[kVPlaneIndex],
				// To keep reference alive.
				[frame_buffer] {});

			webrtc::VideoFrame decoded_frame = webrtc::VideoFrame::Builder()
				.set_video_frame_buffer(cropped_buffer)
				.set_timestamp_us(timestamp)
				.build();

			sink->onVideoFrame(decoded_frame);
		}
		
		return true;
	}

	int VideoDecoder::AVGetBuffer2(AVCodecContext * context, AVFrame * av_frame, int flags)
	{
		// Set in |InitDecode|.
		VideoDecoder * decoder = static_cast<VideoDecoder *>(context->opaque);

		// DCHECK values set in |InitDecode|.
		RTC_DCHECK(decoder);
		// Necessary capability to be allowed to provide our own buffers.
		RTC_DCHECK(context->codec->capabilities | AV_CODEC_CAP_DR1);

		// |av_frame->width| and |av_frame->height| are set by FFmpeg. These are the
		// actual image's dimensions and may be different from |context->width| and
		// |context->coded_width| due to reordering.
		int width = av_frame->width;
		int height = av_frame->height;

		// See |lowres|, if used the decoder scales the image by 1/2^(lowres). This
		// has implications on which resolutions are valid, but we don't use it.
		RTC_CHECK_EQ(context->lowres, 0);

		// Adjust the |width| and |height| to values acceptable by the decoder.
		// Without this, FFmpeg may overflow the buffer. If modified, |width| and/or
		// |height| are larger than the actual image and the image has to be cropped
		// (top-left corner) after decoding to avoid visible borders to the right and
		// bottom of the actual image.
		avcodec_align_dimensions(context, &width, &height);

		RTC_CHECK_GE(width, 0);
		RTC_CHECK_GE(height, 0);

		int ret = av_image_check_size(static_cast<unsigned int>(width), static_cast<unsigned int>(height), 0, nullptr);
		if (ret < 0) {
			RTC_LOG(LS_ERROR) << "Invalid picture size " << width << "x" << height;
			//decoder->ReportError();
			return ret;
		}

		// The video frame is stored in |frame_buffer|. |av_frame| is FFmpeg's version
		// of a video frame and will be set up to reference |frame_buffer|'s data.
		// FFmpeg expects the initial allocation to be zero-initialized according to
		// http://crbug.com/390941. Our pool is set up to zero-initialize new buffers.

		rtc::scoped_refptr<webrtc::I420Buffer> frame_buffer = decoder->buffer_pool.CreateI420Buffer(width, height);
		
		int y_size = width * height;
		int uv_size = frame_buffer->ChromaWidth() * frame_buffer->ChromaHeight();
		
		// DCHECK that we have a continuous buffer as is required.
		RTC_DCHECK_EQ(frame_buffer->DataU(), frame_buffer->DataY() + y_size);
		RTC_DCHECK_EQ(frame_buffer->DataV(), frame_buffer->DataU() + uv_size);
		
		int total_size = y_size + 2 * uv_size;

		av_frame->format = context->pix_fmt;
		av_frame->reordered_opaque = context->reordered_opaque;
		// Set |av_frame| members as required by FFmpeg.
		av_frame->data[kYPlaneIndex] = frame_buffer->MutableDataY();
		av_frame->linesize[kYPlaneIndex] = frame_buffer->StrideY();
		av_frame->data[kUPlaneIndex] = frame_buffer->MutableDataU();
		av_frame->linesize[kUPlaneIndex] = frame_buffer->StrideU();
		av_frame->data[kVPlaneIndex] = frame_buffer->MutableDataV();
		av_frame->linesize[kVPlaneIndex] = frame_buffer->StrideV();
		
		RTC_DCHECK_EQ(av_frame->extended_data, av_frame->data);

		// Create a VideoFrame object, to keep a reference to the buffer.
		av_frame->buf[0] = av_buffer_create(av_frame->data[kYPlaneIndex], total_size, AVFreeBuffer2,
			static_cast<void*>(
				std::make_unique<webrtc::VideoFrame>(webrtc::VideoFrame::Builder()
					.set_video_frame_buffer(frame_buffer)
					.set_rotation(webrtc::kVideoRotation_0)
					.build())
				.release()),
			0);

		RTC_CHECK(av_frame->buf[0]);

		return 0;
	}

	void VideoDecoder::AVFreeBuffer2(void * opaque, uint8_t * data) {
		// The buffer pool recycles the buffer used by |video_frame| when there are no
		// more references to it. |video_frame| is a thin buffer holder and is not
		// recycled.
		webrtc::VideoFrame * video_frame = static_cast<webrtc::VideoFrame *>(opaque);

		delete video_frame;
	}
}