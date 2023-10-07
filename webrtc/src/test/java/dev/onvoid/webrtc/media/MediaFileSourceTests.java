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

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;

import dev.onvoid.webrtc.TestBase;
import dev.onvoid.webrtc.media.audio.AudioStreamInfo;
import dev.onvoid.webrtc.media.video.VideoStreamInfo;

import java.nio.file.Paths;
import java.util.List;

import org.junit.jupiter.api.AfterAll;
import org.junit.jupiter.api.Test;

class MediaFileSourceTests extends TestBase {

	MediaFileSource source;


	@AfterAll
	protected void disposeSource() {
		if (source != null) {
			source.dispose();
		}
	}

	@Test
	void create() throws Exception {
		source = new MediaFileSource(Paths.get("small_bunny_1080p_60fps.mp4"));

		try {
			source.start();
		}
		catch (Error e) {
			e.printStackTrace();
		}
	}

	@Test
	void getStreamInformation() throws Exception {
		source = new MediaFileSource(Paths.get("small_bunny_1080p_60fps.mp4"));

		List<MediaStreamInfo> infoList = source.getMediaStreamInfo();

		assertNotNull(infoList);
		assertEquals(2, infoList.size());

		List<AudioStreamInfo> infoAudioList = source.getAudioStreamInfo();

		// Loaded file has one audio stream.
		assertNotNull(infoAudioList);
		assertEquals(1, infoAudioList.size());

		List<VideoStreamInfo> infoVideoList = source.getVideoStreamInfo();

		// Loaded file has one video stream.
		assertNotNull(infoVideoList);
		assertEquals(1, infoVideoList.size());
	}
}
