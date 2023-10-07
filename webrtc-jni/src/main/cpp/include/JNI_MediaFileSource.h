/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class dev_onvoid_webrtc_media_MediaFileSource */

#ifndef _Included_dev_onvoid_webrtc_media_MediaFileSource
#define _Included_dev_onvoid_webrtc_media_MediaFileSource
#ifdef __cplusplus
extern "C" {
#endif
	/*
	 * Class:     dev_onvoid_webrtc_media_MediaFileSource
	 * Method:    start
	 * Signature: ()V
	 */
	JNIEXPORT void JNICALL Java_dev_onvoid_webrtc_media_MediaFileSource_start
	(JNIEnv*, jobject);

	/*
	 * Class:     dev_onvoid_webrtc_media_MediaFileSource
	 * Method:    stop
	 * Signature: ()V
	 */
	JNIEXPORT void JNICALL Java_dev_onvoid_webrtc_media_MediaFileSource_stop
	(JNIEnv*, jobject);

	/*
	 * Class:     dev_onvoid_webrtc_media_MediaFileSource
	 * Method:    dispose
	 * Signature: ()V
	 */
	JNIEXPORT void JNICALL Java_dev_onvoid_webrtc_media_MediaFileSource_dispose
	(JNIEnv*, jobject);

	/*
	 * Class:     dev_onvoid_webrtc_media_MediaFileSource
	 * Method:    getAudioStreamInfo
	 * Signature: ()Ljava/util/List;
	 */
	JNIEXPORT jobject JNICALL Java_dev_onvoid_webrtc_media_MediaFileSource_getAudioStreamInfo
	(JNIEnv*, jobject);

	/*
	 * Class:     dev_onvoid_webrtc_media_MediaFileSource
	 * Method:    getVideoStreamInfo
	 * Signature: ()Ljava/util/List;
	 */
	JNIEXPORT jobject JNICALL Java_dev_onvoid_webrtc_media_MediaFileSource_getVideoStreamInfo
	(JNIEnv*, jobject);

	/*
	 * Class:     dev_onvoid_webrtc_media_MediaFileSource
	 * Method:    getMediaStreamInfo
	 * Signature: ()Ljava/util/List;
	 */
	JNIEXPORT jobject JNICALL Java_dev_onvoid_webrtc_media_MediaFileSource_getMediaStreamInfo
	(JNIEnv*, jobject);

	/*
	 * Class:     dev_onvoid_webrtc_media_MediaFileSource
	 * Method:    initialize
	 * Signature: (Ljava/lang/String;)V
	 */
	JNIEXPORT void JNICALL Java_dev_onvoid_webrtc_media_MediaFileSource_initialize
	(JNIEnv*, jobject, jstring);

#ifdef __cplusplus
}
#endif
#endif
