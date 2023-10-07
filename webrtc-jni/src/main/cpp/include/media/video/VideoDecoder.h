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

#ifndef JNI_WEBRTC_MEDIA_VIDEO_DECODER_H_
#define JNI_WEBRTC_MEDIA_VIDEO_DECODER_H_

#include "media/Decoder.h"

#include "api/video/i420_buffer.h"
#include "api/video/video_frame.h"
#include "common_video/include/video_frame_buffer_pool.h"

#include <mutex>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}

namespace jni
{
	class VideoFrameSink {
		public:
			virtual ~VideoFrameSink() = default;

			virtual void onVideoFrame(const webrtc::VideoFrame & frame) = 0;
	};


	class VideoFrameSource {
		public:
			virtual ~VideoFrameSource() = default;

			virtual void setSink(VideoFrameSink * sink) = 0;
			virtual void removeSink(VideoFrameSink * sink) = 0;
	};


	class VideoDecoder : public Decoder, public VideoFrameSource
	{
		public:
			VideoDecoder(AVStream * stream);
			virtual ~VideoDecoder();

			virtual void open();
			virtual void close();
			virtual void flush();

			virtual void setSink(VideoFrameSink * sink);
			virtual void removeSink(VideoFrameSink * sink);

		protected:
			virtual bool processFrame(AVFrame * frame);

		private:
			// Called by FFmpeg when it needs a frame buffer to store decoded frames in.
			// The |VideoFrame| returned by FFmpeg at |Decode| originate from here. Their
			// buffers are reference counted and freed by FFmpeg using |AVFreeBuffer2|.
			static int AVGetBuffer2(AVCodecContext * context, AVFrame * av_frame, int flags);

			// Called by FFmpeg when it is done with a video frame, see |AVGetBuffer2|.
			static void AVFreeBuffer2(void * opaque, uint8_t * data);

		private:
			std::mutex mutex;
			VideoFrameSink * sink;

			webrtc::VideoFrameBufferPool buffer_pool;
	};
}

#endif