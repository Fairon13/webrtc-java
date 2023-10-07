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

#include "media/Codec.h"
#include <Exception.h>

namespace jni
{
	Codec::Codec() :
		codec(nullptr),
		context(nullptr),
		stream(nullptr),
		frame(nullptr)
	{
	}

	Codec::~Codec()
    {
		close();
    }

	void Codec::open()
	{
		frame = av_frame_alloc();

		if (!frame) {
			throw Exception("Could not allocate frame");
		}
	}

	void Codec::close()
	{
		if (frame) {
			av_frame_free(&frame);
		}
		if (context) {
			avCodecApi.avcodec_free_context(&context);
		}
	}

	void Codec::flush()
	{
	}
}