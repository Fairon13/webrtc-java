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

#ifndef JNI_WEBRTC_MEDIA_AV_API_H_
#define JNI_WEBRTC_MEDIA_AV_API_H_

#include "platform/DlUtil.h"

#include <string>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/timestamp.h"
}

#ifdef av_err2str
#undef av_err2str

av_always_inline std::string av_err2string(int errnum) {
    char str[AV_ERROR_MAX_STRING_SIZE];
    return av_make_error_string(str, AV_ERROR_MAX_STRING_SIZE, errnum);
}
#define av_err2str(err) av_err2string(err).c_str()

#endif  // av_err2str


#ifdef av_ts2timestr
#undef av_ts2timestr

av_always_inline std::string av_ts_to_string(int64_t ts, AVRational * tb) {
    char str[AV_TS_MAX_STRING_SIZE];
    return av_ts_make_time_string(str, ts, tb);
}
#define av_ts2timestr(ts, tb) av_ts_to_string(ts, tb).c_str()

#endif  // av_ts2timestr


namespace jni
{
	class AvCodecApi {
    	DllHelper _dll{ "avcodec-60.dll" };

    	public:
    		decltype(avcodec_find_decoder)* avcodec_find_decoder = _dll["avcodec_find_decoder"];
            decltype(avcodec_send_packet)* avcodec_send_packet = _dll["avcodec_send_packet"];
            decltype(avcodec_alloc_context3)* avcodec_alloc_context3 = _dll["avcodec_alloc_context3"];
            decltype(avcodec_free_context)* avcodec_free_context = _dll["avcodec_free_context"];
            decltype(avcodec_parameters_to_context)* avcodec_parameters_to_context = _dll["avcodec_parameters_to_context"];
            decltype(avcodec_open2)* avcodec_open2 = _dll["avcodec_open2"];
            decltype(avcodec_receive_frame)* avcodec_receive_frame = _dll["avcodec_receive_frame"];
    };


    class AvFormatApi {
    	DllHelper _dll{ "avformat-60.dll" };

    	public:
    		decltype(avformat_open_input)* avformat_open_input = _dll["avformat_open_input"];
            decltype(avformat_close_input)* avformat_close_input = _dll["avformat_close_input"];
    		decltype(avformat_find_stream_info)* avformat_find_stream_info = _dll["avformat_find_stream_info"];
    		decltype(av_find_best_stream)* av_find_best_stream = _dll["av_find_best_stream"];
            decltype(av_read_frame)* av_read_frame = _dll["av_read_frame"];
    };
}

#endif