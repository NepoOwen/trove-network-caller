// File: network.hpp
// Author: NepoOwen
// Description: This header provides a simple interface for making network calls in the Trove game. It includes functions for setting up network parameters and making calls to the game's network dispatcher.
// https://github.com/NepoOwen/trove-network-caller
//
/*
MIT License

Copyright (c) 2026 NepoOwen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
#include "utils/memory_utils.hpp"
#include <cstdint>
#include <cstdio>
#include <cstring>

// structs, remove if unnecessary includes
struct Vec3 {
    float x, y, z;
};

struct BufferStruct {
    size_t length;      // strlen
    size_t capacity;    // strlen + 1
    const char* string;

    bool Valid(size_t max_len = 65536) const noexcept {
        return reinterpret_cast<unsigned long long>(string) >= 0x10000
            && length > 0
            && length <= max_len
            && capacity == length + 1;
    }
};
// end structs

namespace network {

    namespace detail {

        typedef void(__fastcall* setup_t)(uint64_t* a1);
        typedef void(__fastcall* cleanup_t)(uint64_t* a1);
        typedef void(__fastcall* add_param_byte_t)(uint64_t* a1, uint8_t a2);
        typedef void(__fastcall* add_param_int_t)(uint64_t* a1, int32_t a2);
        typedef void(__fastcall* add_param_int64_t)(uint64_t* a1, int64_t a2);
        typedef void(__fastcall* add_param_string_t)(uint64_t* a1, BufferStruct a2);
        typedef void(__fastcall* add_param_vec3_t)(uint64_t* a1, Vec3 a2);
        typedef void(__fastcall* add_param_float_t)(uint64_t* a1, float a2);
        typedef void(__fastcall* add_param_uint_t)(uint64_t* a1, uint32_t a2);
        typedef void(__fastcall* add_param_double_t)(uint64_t* a1, double a2);

        static setup_t setup = nullptr;
        static cleanup_t cleanup = nullptr;
        static add_param_byte_t add_param_byte = nullptr;
        static add_param_int_t add_param_int = nullptr;
        static add_param_int64_t add_param_int64 = nullptr;
        static add_param_string_t add_param_string = nullptr;
        static add_param_vec3_t add_param_vec3 = nullptr;
        static add_param_float_t add_param_float = nullptr;
        static add_param_uint_t add_param_uint = nullptr;
        static add_param_double_t add_param_double = nullptr;

        static bool initialize() {
            static bool initialized = false;
            static bool success = false;
            if (initialized) return success;
            initialized = true;

            setup = (setup_t)(sdk::find_pattern(nullptr, "40 53 48 83 EC ? 48 83 CA"));
            cleanup = (cleanup_t)(sdk::find_pattern(nullptr, "40 53 48 83 EC ? 48 8D 05 ? ? ? ? 48 8B D9 ? ? ? 48 8D 05 ? ? ? ? 48 89 41 ? E8"));
            add_param_byte = (add_param_byte_t)(sdk::find_pattern(nullptr, "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 0F B6 DA 48 8B F1 BA"));
            add_param_int = (add_param_int_t)(sdk::find_pattern(nullptr, "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 8B FA 48 8B F1 33 D2"));
            add_param_int64 = (add_param_int64_t)(sdk::find_pattern(nullptr, "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B FA 48 8B F1 BA ? ? ? ? 48 83 C1 ? E8 ? ? ? ? 48 8B D7"));
            add_param_string = (add_param_string_t)(sdk::find_pattern(nullptr, "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B FA 48 8B D9 BA"));
            add_param_vec3 = (add_param_vec3_t)(sdk::find_pattern(nullptr, "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B FA 48 8B F1 BA ? ? ? ? 48 83 C1 ? E8 ? ? ? ? ? ? 48 8D 4E"));
            add_param_float = (add_param_float_t)(sdk::find_pattern(nullptr, "48 89 5C 24 ? 57 48 83 EC ? 48 8B F9 0F 29 74 24 ? 48 83 C1 ? BA 05 00 00 00 0F 28 F1 E8 ? ? ? ? 0F 28 CE 48 8D 4F ? E8 ? ? ? ? 48 8B 5C 24 ? 48 8B C7 0F 28 74 24 ? 48 83 C4 ? 5F C3"));
            add_param_uint = (add_param_uint_t)(sdk::find_pattern(nullptr, "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 8B FA 48 8B F1 BA"));
            add_param_double = (add_param_double_t)(sdk::find_pattern(nullptr, "48 89 5C 24 ? 57 48 83 EC ? 48 8B F9 0F 29 74 24 ? 48 83 C1 ? BA 06 00 00 00 0F 28 F1 E8 ? ? ? ? 0F 28 CE 48 8D 4F ? E8 ? ? ? ? 48 8B 5C 24 ? 48 8B C7 0F 28 74 24 ? 48 83 C4 ? 5F C3"));

            success = (setup && cleanup &&
                add_param_byte && add_param_int &&
                add_param_int64 && add_param_string &&
                add_param_vec3 && add_param_float &&
                add_param_uint && add_param_double);
            if (!success) {
                std::printf("[NET] pattern resolution FAILED:\n");
                std::printf("  setup=%p cleanup=%p byte=%p int=%p int64=%p string=%p vec3=%p float=%p uint=%p double=%p\n",
                    (void*)setup, (void*)cleanup, (void*)add_param_byte, (void*)add_param_int,
                    (void*)add_param_int64, (void*)add_param_string, (void*)add_param_vec3,
                    (void*)add_param_float, (void*)add_param_uint, (void*)add_param_double);
            }
            return success;
        }

    } // namespace detail


    // ---- Public API: create / add_* / call ----
    inline void setup(uint64_t* v5) {
        if (!detail::initialize()) return;
        network::detail::setup(v5);
    }

    inline void add_byte(uint64_t* buffer, uint8_t value) { if (detail::add_param_byte) detail::add_param_byte(buffer, value); }
    inline void add_int(uint64_t* buffer, int32_t value) { if (detail::add_param_int) detail::add_param_int(buffer, value); }
    inline void add_uint(uint64_t* buffer, uint32_t value) { if (detail::add_param_uint) detail::add_param_uint(buffer, value); }
    inline void add_int64(uint64_t* buffer, int64_t value) { if (detail::add_param_int64) detail::add_param_int64(buffer, value); }
    inline void add_float(uint64_t* buffer, float value) { if (detail::add_param_float) detail::add_param_float(buffer, value); }
    inline void add_double(uint64_t* buffer, double value) { if (detail::add_param_double) detail::add_param_double(buffer, value); }
    inline void add_vec3(uint64_t* buffer, Vec3 value) { if (detail::add_param_vec3) detail::add_param_vec3(buffer, value); }

    inline void add_string(uint64_t* buffer, const char* value) {
        if (!detail::add_param_string) return;
        BufferStruct bs;
        bs.length = std::strlen(value);
        bs.capacity = bs.length + 1;
        bs.string = value;
        detail::add_param_string(buffer, bs);
    }

    inline void call(const char* event_name, __int64 a1, uint64_t* v5, int64_t dispatcher_offset = 192, int64_t extra_offset = -1) {
        if (!a1 || !v5) return;
        if (!detail::initialize()) return;

        typedef void(__fastcall* call_t)(__int64, void*, uint64_t*);
        call_t network_call = *(call_t*)(*(uint64_t*)a1 + dispatcher_offset);
        if (!network_call) return;

        if (extra_offset != -1) {
            typedef __int64(__fastcall* extra_t)(__int64);
            extra_t network_extra = *(extra_t*)(*(uint64_t*)a1 + 200LL);
            if (network_extra) network_extra(a1);
        }

        struct v4_struct {
            const char* name;
            uint64_t flags;
            uint64_t zero;
        } v4;
        v4.name = event_name;
        v4.flags = 3;
        v4.zero = 0;

        network_call(a1, &v4, v5);
        detail::cleanup(v5);
    }

} // namespace network

// examples usage:
/*
* // RequestSpecificWorld
uint64_t v5[14];
memset(v5, 0, 0x60u);
network::setup(v5);
network::add_int64(v5, 7164068446503133809LL);
network::call("RequestSpecificWorld", hook::utils::GetViaSessionKey(376), v5);

* // SetClass
uint64_t v5[14];
memset(v5, 0, 0x60u);
network::setup(v5);
network::add_int(v5, 10);
network::add_byte(v5, 0);
network::call("SetClass", hook::utils::GetViaSessionKey(10), v5);

* // InvitePlayer
uint64_t v5[14];
memset(v5, 0, 0x60u);
network::setup(v5);
network::add_string(v5, "rapter223");
network::add_int(v5, 0);
network::call("InvitePlayer", hook::utils::GetViaSessionKey(376), v5);

* // JoinPlayer
uint64_t v5[14];
memset(v5, 0, 0x60u);
network::setup(v5);
network::add_string(v5, "rapter223");
network::call("JoinPlayer", hook::utils::GetViaSessionKey(376), v5);
*/
