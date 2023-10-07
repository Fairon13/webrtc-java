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

package dev.onvoid.webrtc.media.audio;

import dev.onvoid.webrtc.media.MediaStreamInfo;

/**
 * Media stream information container for audio streams.
 *
 * @author Alex Andres
 */
public class AudioStreamInfo extends MediaStreamInfo {

	private int channels;

	private int sampleRate;


	protected AudioStreamInfo() {
		// Initialized by native code.
	}

	public int getChannels() {
		return channels;
	}

	public int getSampleRate() {
		return sampleRate;
	}

	@Override
	public String toString() {
		return "AudioStreamInfo{"
				+ "streamIndex='" + getStreamIndex()
				+ ", codecName='" + getCodecName() + '\''
				+ ", codecNameLong='" + getCodecNameLong() + '\''
				+ ", bitrate=" + getBitrate()
				+ ", channels=" + channels
				+ ", sampleRate=" + sampleRate
				+ '}';
	}
}
