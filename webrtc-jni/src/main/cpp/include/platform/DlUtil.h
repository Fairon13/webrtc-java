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

#ifndef JNI_DL_UTIL_H_
#define JNI_DL_UTIL_H_

#if _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include <string>

class ProcPtr {
    public:
        explicit ProcPtr(FARPROC ptr) : _ptr(ptr) {}

        template <typename T, typename = std::enable_if_t<std::is_function_v<T>>>
        operator T* () const {
            return reinterpret_cast<T*>(_ptr);
        }

    private:
        FARPROC _ptr;
};

class DllHelper {
    public:
        explicit DllHelper(const char * filename) :
            _module(LoadLibraryA(filename))
        {
        }

        ~DllHelper() {
            FreeLibrary(_module);
        }

        ProcPtr operator[](LPCSTR proc_name) const
        {
            return ProcPtr(GetProcAddress(_module, proc_name));
        }

    private:
        HMODULE _module;
};

 #endif