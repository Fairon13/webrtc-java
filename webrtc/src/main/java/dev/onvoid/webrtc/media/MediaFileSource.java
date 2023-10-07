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

package dev.onvoid.webrtc.media;

import dev.onvoid.webrtc.media.audio.AudioStreamInfo;
import dev.onvoid.webrtc.media.video.VideoStreamInfo;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;

/**
 * A media file source for Audio/VideoTracks, e.g. a video or audio file.
 *
 * @author Alex Andres
 */
public class MediaFileSource extends MediaSource {

	static {
		try {
			System.load("C:\\Users\\Alex\\Downloads\\FFmpeg-n6.0-build\\bin\\avutil-58.dll");
			System.load("C:\\Users\\Alex\\Downloads\\FFmpeg-n6.0-build\\bin\\swresample-4.dll");
			System.load("C:\\Users\\Alex\\Downloads\\FFmpeg-n6.0-build\\bin\\avcodec-60.dll");
			System.load("C:\\Users\\Alex\\Downloads\\FFmpeg-n6.0-build\\bin\\avformat-60.dll");
			System.load("C:\\Users\\Alex\\Downloads\\FFmpeg-n6.0-build\\bin\\avfilter-9.dll");
		}
		catch (Throwable e) {
			e.printStackTrace();
		}
	}


	public MediaFileSource(Path filePath) throws IOException {
		super();

		if (!Files.exists(filePath)) {
			throw new IOException(filePath.toAbsolutePath() + " does not exist");
		}

		initialize(filePath.toAbsolutePath().toString());
	}

	public native void start();

	public native void stop();

	public native void dispose();

	public native List<AudioStreamInfo> getAudioStreamInfo();
	public native List<VideoStreamInfo> getVideoStreamInfo();
	public native List<MediaStreamInfo> getMediaStreamInfo();

	private native void initialize(String filePath);

}
