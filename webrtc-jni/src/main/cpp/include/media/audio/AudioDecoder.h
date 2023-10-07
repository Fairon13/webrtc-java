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

#ifndef JNI_WEBRTC_MEDIA_AUDIO_DECODER_H_
#define JNI_WEBRTC_MEDIA_AUDIO_DECODER_H_

#include "media/Decoder.h"

#include <mutex>

namespace jni
{
	class AudioFrameSink {
		public:
			virtual ~AudioFrameSink() = default;

			virtual void onAudioFrame() = 0;
	};


	class AudioFrameSource {
		public:
			virtual ~AudioFrameSource() = default;

			virtual void setSink(AudioFrameSink * sink) = 0;
			virtual void removeSink(AudioFrameSink * sink) = 0;
	};


	class AudioDecoder : public Decoder, public AudioFrameSource
	{
		public:
			AudioDecoder(AVStream * stream);
			virtual ~AudioDecoder();

			virtual void open();
			virtual void close();
			virtual void flush();

			virtual void setSink(AudioFrameSink * sink);
			virtual void removeSink(AudioFrameSink * sink);

		protected:
			virtual bool processFrame(AVFrame * frame);

		private:
			std::mutex mutex;
			AudioFrameSink * sink;
	};
}

#endif