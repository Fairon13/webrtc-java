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

#include "media/Decoder.h"
#include <Exception.h>

#include <rtc_base/logging.h>

namespace jni
{
	Decoder::Decoder(AVStream * stream) :
		Codec()
	{
		this->stream = stream;
	}

	Decoder::~Decoder()
    {
    }

	void Decoder::open()
	{
 		// Find decoder for the stream.
        codec = avCodecApi.avcodec_find_decoder(stream->codecpar->codec_id);
        if (!codec) {
            throw Exception("Failed to find decoder codec");
        }

        // Allocate a codec context for the decoder.
        context = avCodecApi.avcodec_alloc_context3(codec);
        if (!context) {
            throw Exception("Failed to allocate the decoder context");
        }
        // Copy codec parameters from input stream to output codec context.
        if (avCodecApi.avcodec_parameters_to_context(context, stream->codecpar) < 0) {
            throw Exception("Failed to copy parameters to decoder context");
        }

        // Init the decoder.
        if (avCodecApi.avcodec_open2(context, codec, NULL) < 0) {
            throw Exception("Failed to open decoder");
        }

        Codec::open();
	}

	void Decoder::close()
	{
		Codec::close();
	}

	void Decoder::flush()
    {
        decode(nullptr);

		Codec::flush();
    }

    bool Decoder::decode(AVPacket * packet)
    {
        // Submit the packet to the decoder.
        int ret = avCodecApi.avcodec_send_packet(context, packet);

        if (ret < 0) {
            RTC_LOG(LS_ERROR) << "Error submitting a packet for decoding: " << av_err2str(ret);
            return false;
        }
        
        // Get all the available frames from the decoder.
        while (ret >= 0) {
            ret = avCodecApi.avcodec_receive_frame(context, frame);

            if (ret < 0) {
                // There is no output frame available, but there were no errors during decoding.
                if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN)) {
                    return true;
                }

                RTC_LOG(LS_ERROR) << "Error during decoding: " << av_err2str(ret);
                return false;
            }

            // Handle frame data.
            ret = processFrame(frame) ? 0 : -1;

            av_frame_unref(frame);

            if (ret < 0) {
                return false;
            }
        }

        return true;
    }

    int Decoder::getStreamIndex()
    {
        return stream ? stream->index : -1;
    }
}