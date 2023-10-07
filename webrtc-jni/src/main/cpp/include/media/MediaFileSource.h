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

#ifndef JNI_WEBRTC_MEDIA_FILE_SOURCE_H_
#define JNI_WEBRTC_MEDIA_FILE_SOURCE_H_

#include <atomic>
#include <string>
#include <vector>
#include <memory>
#include <mutex>

#include "media/AvApi.h"
#include "MediaStreamInfo.h"
#include "audio/AudioDecoder.h"
#include "video/VideoDecoder.h"
#include "utils/Property.h"

#include "api/media_stream_interface.h"
#include "api/notifier.h"
#include "pc/video_track_source.h"
#include "rtc_base/task_queue.h"
#include "rtc_base/thread.h"

namespace jni
{
	class SourceStateListener {
		public:
			virtual ~SourceStateListener() = default;

			virtual void onSourceState(const webrtc::MediaSourceInterface::SourceState state) const = 0;
	};


	using StreamInfos = std::vector<std::shared_ptr<MediaStreamInfo>>;


	class MediaFileSource : public webrtc::Notifier<webrtc::AudioSourceInterface>, webrtc::VideoTrackSource, VideoFrameSink
	{
		public:
			MediaFileSource(std::string filePath);
			virtual ~MediaFileSource();

			const std::uint64_t getDuration() const;
			const std::uint64_t getBitrate() const;

			const StreamInfos getAudioStreamInfo() const;
			const StreamInfos getVideoStreamInfo() const;
			const StreamInfos & getStreamInfo() const;

			const void setLoop(const bool loop) const;
			const bool getLoop() const;

			void start();
			void stop();

			void onVideoFrame(const webrtc::VideoFrame & frame);

			int addStateListener(std::weak_ptr<SourceStateListener> listener);
			void removeStateListener(std::weak_ptr<SourceStateListener> listener);

			// MediaSourceInterface implementation.
			SourceState state() const override;
			bool remote() const override;

			// AudioSourceInterface implementation.
			void AddSink(webrtc::AudioTrackSinkInterface * sink) override {}
			void RemoveSink(webrtc::AudioTrackSinkInterface * sink) override {}

		protected:
			// VideoTrackSource implementation.
			virtual rtc::VideoSourceInterface<webrtc::VideoFrame> * source();

		private:
			void open();
			void run();
			void initialize();
			void dispose();
			Decoder * createDecoder(enum AVMediaType type);

		private:
			std::unique_ptr<rtc::Thread> worker_thread;
			std::mutex mutex;
			std::atomic_bool running;

			std::unique_ptr<AudioDecoder> audioDecoder;
			std::unique_ptr<VideoDecoder> videoDecoder;
			
			AvFormatApi avFormatApi;
			AvCodecApi avCodecApi;

			std::string filePath;
			std::uint64_t durationUs;
			std::uint64_t bitrate;

			mutable bool loop;

			Property<SourceState> state_;

			StreamInfos streamInfos;

			AVFormatContext * formatCtx;
			AVPacket * packet;
	};
}

#endif