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

#ifndef JNI_WEBRTC_MEDIA_STREAM_INFO_H_
#define JNI_WEBRTC_MEDIA_STREAM_INFO_H_

#include <string>
#include <jni.h>

#include "JavaClass.h"
#include "JavaRef.h"
#include "JavaString.h"
#include "JavaUtils.h"
#include "JNI_WebRTC.h"

namespace jni
{
	class MediaStreamInfo
	{
		public:
			MediaStreamInfo(const std::uint32_t & index, const std::string & name, const std::string & name_long, const std::uint64_t & bitrate) :
				index(index),
				codec_name(name),
				codec_name_long(name_long),
				bitrate(bitrate)
			{
			}

			virtual ~MediaStreamInfo() = default;

			const std::uint32_t getStreamIndex() const
			{
				return index;
			}

			const std::string getCodecName() const
			{
				return codec_name;
			}

			const std::string getCodecLongName() const
			{
				return codec_name_long;
			}

			const std::uint64_t getBitrate() const
			{
				return bitrate;
			}

		private:
			std::uint32_t index;
			std::string codec_name;
			std::string codec_name_long;
			std::uint64_t bitrate;
	};


	class AudioStreamInfo : public MediaStreamInfo
	{
		public:
			AudioStreamInfo(const std::uint32_t & index, const std::string & name, const std::string & name_long,
				const std::uint64_t & bitrate, const std::uint32_t & channels, const std::uint32_t & sample_rate) :
				MediaStreamInfo(index, name, name_long, bitrate),
				channels(channels),
				sample_rate(sample_rate)
			{
			}

			~AudioStreamInfo() = default;
			
			const std::uint32_t getChannels() const
			{
				return channels;
			}

			const std::uint32_t getSampleRate() const
			{
				return sample_rate;
			}

		private:
			std::uint32_t channels;
			std::uint32_t sample_rate;
	};


	class VideoStreamInfo : public MediaStreamInfo
	{
		public:
			VideoStreamInfo(const std::uint32_t & index, const std::string & name, const std::string & name_long,
				const std::uint64_t & bitrate, const std::uint32_t & width, const std::uint32_t & height, const std::string & format_name,
				const double & frame_rate) :
				MediaStreamInfo(index, name, name_long, bitrate),
				width(width),
				height(height),
				format_name(format_name),
				frame_rate(frame_rate)
			{
			}

			~VideoStreamInfo() = default;

			const std::uint32_t getWidth() const
			{
				return width;
			}

			const std::uint32_t getHeight() const
			{
				return height;
			}

			const std::string getFormatName() const
			{
				return format_name;
			}

			const double getFrateRate() const
			{
				return frame_rate;
			}

		private:
			std::uint32_t width;
			std::uint32_t height;
			std::string format_name;
			double frame_rate;
	};

	/*
	 * Java Native Interface.
	 */

	namespace MediaStreamInfos
	{
		class JavaMediaStreamInfoClass : public JavaClass
		{
			public:
				explicit JavaMediaStreamInfoClass(JNIEnv * env)
				{
					cls = FindClass(env, PKG_MEDIA"MediaStreamInfo");

					index = GetFieldID(env, cls, "index", "I");
					codecName = GetFieldID(env, cls, "codecName", STRING_SIG);
					codecNameLong = GetFieldID(env, cls, "codecNameLong", STRING_SIG);
					bitrate = GetFieldID(env, cls, "bitrate", "J");
				}

				jclass cls;
				jfieldID index;
				jfieldID codecName;
				jfieldID codecNameLong;
				jfieldID bitrate;
		};

		class JavaAudioStreamInfoClass : public JavaMediaStreamInfoClass
		{
			public:
				explicit JavaAudioStreamInfoClass(JNIEnv * env) :
					JavaMediaStreamInfoClass(env)
				{
					cls = FindClass(env, PKG_AUDIO"AudioStreamInfo");

					ctor = GetMethod(env, cls, "<init>", "()V");

					channels = GetFieldID(env, cls, "channels", "I");
					sampleRate = GetFieldID(env, cls, "sampleRate", "I");
				}

				jclass cls;
				jmethodID ctor;
				jfieldID channels;
				jfieldID sampleRate;
		};

		class JavaVideoStreamInfoClass : public JavaMediaStreamInfoClass
		{
			public:
				explicit JavaVideoStreamInfoClass(JNIEnv * env) :
					JavaMediaStreamInfoClass(env)
				{
					cls = FindClass(env, PKG_VIDEO"VideoStreamInfo");

					ctor = GetMethod(env, cls, "<init>", "()V");

					width = GetFieldID(env, cls, "width", "I");
					height = GetFieldID(env, cls, "height", "I");
					formatName = GetFieldID(env, cls, "formatName", STRING_SIG);
					frameRate = GetFieldID(env, cls, "frameRate", "D");
				}

				jclass cls;
				jmethodID ctor;
				jfieldID width;
				jfieldID height;
				jfieldID formatName;
				jfieldID frameRate;
		};

		static JavaLocalRef<jobject> toJava(JNIEnv * env, const std::shared_ptr<MediaStreamInfo> & streamInfo)
		{
			const auto javaBaseClass = JavaClasses::get<JavaMediaStreamInfoClass>(env);
			jobject jInfo = nullptr;

			if (dynamic_cast<AudioStreamInfo *>(streamInfo.get())) {
				const auto info = dynamic_cast<AudioStreamInfo *>(streamInfo.get());
				const auto javaClass = JavaClasses::get<JavaAudioStreamInfoClass>(env);

				jInfo = env->NewObject(javaClass->cls, javaClass->ctor);

				env->SetIntField(jInfo, javaClass->channels, info->getChannels());
				env->SetIntField(jInfo, javaClass->sampleRate, info->getSampleRate());
			}
			else if (dynamic_cast<VideoStreamInfo *>(streamInfo.get())) {
				const auto info = dynamic_cast<VideoStreamInfo *>(streamInfo.get());
				const auto javaClass = JavaClasses::get<JavaVideoStreamInfoClass>(env);

				jInfo = env->NewObject(javaClass->cls, javaClass->ctor);

				env->SetIntField(jInfo, javaClass->width, info->getWidth());
				env->SetIntField(jInfo, javaClass->height, info->getHeight());
				env->SetObjectField(jInfo, javaClass->formatName, JavaString::toJava(env, info->getFormatName()));
				env->SetDoubleField(jInfo, javaClass->frameRate, info->getFrateRate());
			}

			if (jInfo) {
				env->SetIntField(jInfo, javaBaseClass->index, streamInfo->getStreamIndex());
				env->SetObjectField(jInfo, javaBaseClass->codecName, JavaString::toJava(env, streamInfo->getCodecName()));
				env->SetObjectField(jInfo, javaBaseClass->codecNameLong, JavaString::toJava(env, streamInfo->getCodecLongName()));
				env->SetLongField(jInfo, javaBaseClass->bitrate, streamInfo->getBitrate());
			}

			return JavaLocalRef<jobject>(env, jInfo);
		}
	}
}

#endif