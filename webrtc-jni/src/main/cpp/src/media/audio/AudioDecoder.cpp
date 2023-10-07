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

#include "media/audio/AudioDecoder.h"
#include <Exception.h>

#include <libavutil/timestamp.h>

namespace jni
{
	AudioDecoder::AudioDecoder(AVStream * stream) :
		Decoder(stream)
	{
	}

	AudioDecoder::~AudioDecoder()
    {
		close();
    }

	void AudioDecoder::open()
	{
		Decoder::open();
	}

	void AudioDecoder::close()
	{
		Decoder::close();
	}

	void AudioDecoder::flush()
    {
		Decoder::flush();
    }

	void AudioDecoder::setSink(AudioFrameSink * sink)
	{
		const std::scoped_lock lock{ mutex };

		this->sink = sink;
	}

	void AudioDecoder::removeSink(AudioFrameSink * sink)
	{
		const std::scoped_lock lock{ mutex };

		this->sink = nullptr;
	}

	bool AudioDecoder::processFrame(AVFrame * frame)
	{
		printf("audio: nb_samples = %d, channels = %d, time = %s, pts = %lld\n",
			frame->nb_samples, frame->ch_layout.nb_channels , av_ts2timestr(frame->pts, &stream->time_base), frame->pts);
		fflush(NULL);

		return true;
	}
}