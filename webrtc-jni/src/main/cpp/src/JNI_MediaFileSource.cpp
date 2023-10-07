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

#include "JNI_MediaFileSource.h"
#include "media/MediaFileSource.h"
#include "media/MediaStreamInfo.h"
#include "JavaString.h"
#include "JavaArrayList.h"
#include "JavaList.h"
#include "JavaUtils.h"
#include "Exception.h"

#include "api/task_queue/default_task_queue_factory.h"
#include "rtc_base/ref_counted_object.h"

JNIEXPORT void JNICALL Java_dev_onvoid_webrtc_media_MediaFileSource_start
(JNIEnv * env, jobject caller)
{
	jni::MediaFileSource* source = GetHandle<jni::MediaFileSource>(env, caller);
	CHECK_HANDLE(source);

	try {
		source->start();
	}
	catch (...) {
		ThrowCxxJavaException(env);
	}
}

JNIEXPORT void JNICALL Java_dev_onvoid_webrtc_media_MediaFileSource_stop
(JNIEnv * env, jobject caller)
{
	jni::MediaFileSource * source = GetHandle<jni::MediaFileSource>(env, caller);
	CHECK_HANDLE(source);

	try {
		source->stop();
	}
	catch (...) {
		ThrowCxxJavaException(env);
	}
}

JNIEXPORT void JNICALL Java_dev_onvoid_webrtc_media_MediaFileSource_dispose
(JNIEnv * env, jobject caller)
{
	jni::MediaFileSource * source = GetHandle<jni::MediaFileSource>(env, caller);
	CHECK_HANDLE(source);

	delete source;
}

JNIEXPORT void JNICALL Java_dev_onvoid_webrtc_media_MediaFileSource_initialize
(JNIEnv * env, jobject caller, jstring jPath)
{
	if (jPath == NULL) {
		env->Throw(jni::JavaNullPointerException(env, "Path is null"));
		return;
	}

	std::string path = jni::JavaString::toNative(env, jni::JavaLocalRef<jstring>(env, jPath));

	try {
		std::unique_ptr<webrtc::TaskQueueFactory> taskQueueFactory = webrtc::CreateDefaultTaskQueueFactory();

		auto fileSource = rtc::make_ref_counted<jni::MediaFileSource>(path, taskQueueFactory.release());

		SetHandle(env, caller, fileSource.release());
	}
	catch (...) {
		ThrowCxxJavaException(env);
	}
}

JNIEXPORT jobject JNICALL Java_dev_onvoid_webrtc_media_MediaFileSource_getAudioStreamInfo
(JNIEnv * env, jobject caller)
{
	jni::MediaFileSource * source = GetHandle<jni::MediaFileSource>(env, caller);
	CHECK_HANDLEV(source, nullptr);

	auto infoList = jni::JavaList::toArrayList(env, source->getAudioStreamInfo(), &jni::MediaStreamInfos::toJava);

	return infoList.release();
}

JNIEXPORT jobject JNICALL Java_dev_onvoid_webrtc_media_MediaFileSource_getVideoStreamInfo
(JNIEnv * env, jobject caller)
{
	jni::MediaFileSource * source = GetHandle<jni::MediaFileSource>(env, caller);
	CHECK_HANDLEV(source, nullptr);

	auto infoList = jni::JavaList::toArrayList(env, source->getVideoStreamInfo(), &jni::MediaStreamInfos::toJava);

	return infoList.release();
}

JNIEXPORT jobject JNICALL Java_dev_onvoid_webrtc_media_MediaFileSource_getMediaStreamInfo
(JNIEnv * env, jobject caller)
{
	jni::MediaFileSource * source = GetHandle<jni::MediaFileSource>(env, caller);
	CHECK_HANDLEV(source, nullptr);

	auto infoList = jni::JavaList::toArrayList(env, source->getStreamInfo(), &jni::MediaStreamInfos::toJava);

	return infoList.release();
}