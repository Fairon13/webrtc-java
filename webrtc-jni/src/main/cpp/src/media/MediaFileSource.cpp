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

#include "media/MediaFileSource.h"

#include <algorithm>
#include <iterator>
#include <Exception.h>
#include <windows.h> 
#include <rtc_base/logging.h>

extern "C"
{
#include "libavutil/imgutils.h"
}

#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avcodec.lib")

namespace jni
{
	MediaFileSource::MediaFileSource(std::string filePath) :
        VideoTrackSource(/*remote=*/false),
        worker_thread(nullptr),
        running(false),
        audioDecoder(nullptr),
        videoDecoder(nullptr),
        filePath(filePath),
        durationUs(0),
        bitrate(0),
        loop(false),
        state_(SourceState::kInitializing),
        formatCtx(nullptr),
        packet(nullptr)
	{
        open();
	}

    MediaFileSource::~MediaFileSource()
    {
        stop();
        dispose();
    }

    const std::uint64_t MediaFileSource::getDuration() const
    {
        return durationUs;
    }

    const std::uint64_t MediaFileSource::getBitrate() const
    {
        return bitrate;
    }

    const StreamInfos MediaFileSource::getAudioStreamInfo() const
    {
        StreamInfos audioInfo;

        // Filter audio streams.
        std::copy_if(streamInfos.begin(), streamInfos.end(), std::back_inserter(audioInfo),
            [](const std::shared_ptr<MediaStreamInfo> & i) {
                return dynamic_cast<AudioStreamInfo *>(i.get());
            });

        return audioInfo;
    }

    const StreamInfos MediaFileSource::getVideoStreamInfo() const
    {
        StreamInfos videoInfo;

        // Filter video streams.
        std::copy_if(streamInfos.begin(), streamInfos.end(), std::back_inserter(videoInfo),
            [](const std::shared_ptr<MediaStreamInfo>& i) {
                return dynamic_cast<VideoStreamInfo *>(i.get());
            });

        return videoInfo;
    }

    const StreamInfos & MediaFileSource::getStreamInfo() const
    {
        return streamInfos;
    }

    const void MediaFileSource::setLoop(const bool loop) const
    {
        this->loop = loop;
    }

    const bool MediaFileSource::getLoop() const
    {
        return this->loop;
    }

    void MediaFileSource::start()
    {
        const std::scoped_lock lock{ mutex };

        if (running) {
            return;
        }

        worker_thread = rtc::Thread::Create();
        worker_thread->SetName("MediaFileSource worker", nullptr);
        
        if (!worker_thread->Start()) {
            throw jni::Exception("Failed to start thread");
        }

        running = true;

        worker_thread->PostTask([this] {
            run();
        });
    }

    void MediaFileSource::stop()
    {
        const std::scoped_lock lock{ mutex };

        if (running) {
            running = false;

            worker_thread->Quit();
        }
    }

    void MediaFileSource::onVideoFrame(const webrtc::VideoFrame & frame)
    {
        printf("video: width = %d, height = %d, timestamp = %lld\n",
            frame.width(), frame.height(), frame.timestamp_us());
        fflush(NULL);
    }

    int MediaFileSource::addStateListener(std::weak_ptr<SourceStateListener> listener)
    {
        return state_.signal().bindMember(listener, &SourceStateListener::onSourceState);
    }

    void MediaFileSource::removeStateListener(std::weak_ptr<SourceStateListener> listener)
    {
        
    }

    rtc::VideoSourceInterface<webrtc::VideoFrame> * MediaFileSource::source()
    {
        return this;
    }

    webrtc::MediaSourceInterface::SourceState MediaFileSource::state() const
    {
        return state_.get();
    }

    bool MediaFileSource::remote() const
    {
        return false;
    }

    void MediaFileSource::run()
    {
        try {
            initialize();
        }
        catch (...) {

            return;
        }

        state_ = SourceState::kLive;

        bool ret = true;

        //int64_t frameInterval = videoDecoder ? fpsToInterval(_video->iparams.fps) : 0;

        // Read frames from the file
        while (running && avFormatApi.av_read_frame(formatCtx, packet) >= 0) {
            // Check if the packet belongs to a stream we are interested in, otherwise skip it.
            if (videoDecoder && packet->stream_index == videoDecoder->getStreamIndex()) {
                ret = videoDecoder->decode(packet);
            }
            else if (audioDecoder && packet->stream_index == audioDecoder->getStreamIndex()) {
                ret = audioDecoder->decode(packet);
            }

            av_packet_unref(packet);

            if (!ret) {
                break;
            }
        }

        // Flush the decoders.
        if (videoDecoder) {
            videoDecoder->flush();
        }
        if (audioDecoder) {
            audioDecoder->flush();
        }

        state_ = SourceState::kEnded;

        dispose();
    }

    void MediaFileSource::open()
    {
        // Open input file, and allocate format context.
        if (avFormatApi.avformat_open_input(&formatCtx, filePath.c_str(), NULL, NULL) < 0) {
            throw jni::Exception("Could not open source file %s", filePath.c_str());
        }

        // Retrieve stream information.
        if (avFormatApi.avformat_find_stream_info(formatCtx, NULL) < 0) {
            throw jni::Exception("Could not find stream information");
        }

        // Get general information.
        durationUs = formatCtx->duration;
        bitrate = formatCtx->bit_rate;

        // Get stream information.
        for (std::uint32_t i = 0; i < formatCtx->nb_streams; i++) {
            AVCodecParameters * codecParameters = formatCtx->streams[i]->codecpar;

            // Find the registered decoder.
            const AVCodec * codec = avCodecApi.avcodec_find_decoder(codecParameters->codec_id);

            if (codec == NULL) {
                RTC_LOG(LS_WARNING) << "Unsupported codec!";
                continue;
            }

            if (codecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
                AVStream * stream = formatCtx->streams[i];
                const char * fmtName = av_get_pix_fmt_name(static_cast<AVPixelFormat>(codecParameters->format));
                const std::string formatName = fmtName ? fmtName : "unknown";
                double frameRate = av_q2d(stream->r_frame_rate);

                streamInfos.push_back(std::make_shared<VideoStreamInfo>(i, codec->name, codec->long_name,
                    codecParameters->bit_rate, codecParameters->width, codecParameters->height, formatName, frameRate));
            }
            else if (codecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {
                streamInfos.push_back(std::make_shared<AudioStreamInfo>(i, codec->name, codec->long_name,
                    codecParameters->bit_rate, codecParameters->ch_layout.nb_channels, codecParameters->sample_rate));
            }
        }
    }

    void MediaFileSource::initialize()
    {
        // Initialize video decoder.
        videoDecoder.reset(dynamic_cast<VideoDecoder *>(createDecoder(AVMEDIA_TYPE_VIDEO)));

        if (videoDecoder) {
            videoDecoder->setSink(this);
            videoDecoder->open();
        }

        // Initialize audio decoder.
        audioDecoder.reset(dynamic_cast<AudioDecoder *>(createDecoder(AVMEDIA_TYPE_AUDIO)));

        if (audioDecoder) {
            audioDecoder->open();
        }

        if (!audioDecoder && !videoDecoder) {
            dispose();
            throw jni::Exception("Could not find audio or video stream in the input");
        }

        packet = av_packet_alloc();
        if (!packet) {
            dispose();
            throw jni::Exception("Could not allocate packet");
        }
    }

    void MediaFileSource::dispose()
    {
        avFormatApi.avformat_close_input(&formatCtx);
        av_packet_free(&packet);
        
        audioDecoder.reset(nullptr);
        videoDecoder.reset(nullptr);
    }

    Decoder * MediaFileSource::createDecoder(enum AVMediaType type)
    {
        int stream_index = avFormatApi.av_find_best_stream(formatCtx, type, -1, -1, NULL, 0);

        if (stream_index < 0) {
            RTC_LOG(LS_WARNING) << "Could not find " << av_get_media_type_string(type) << " stream in input file";
            return nullptr;
        }

        AVStream * stream = formatCtx->streams[stream_index];

        switch (type) {
            case AVMEDIA_TYPE_VIDEO:
                return new VideoDecoder(stream);
            case AVMEDIA_TYPE_AUDIO:
                return new AudioDecoder(stream);
            default:
                return nullptr;
        }
    }
}