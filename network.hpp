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
#include <cstring>

// types enum
enum class NetworkParamType {
    Int,
    Int64,
    String,
    Byte
};

namespace structs {

    struct BufferStruct {
        size_t length;		// strlen
        size_t capacity;	// strlen + 1
        const char* string;

        bool Valid(size_t max_len = 65536) const noexcept {
            return reinterpret_cast<unsigned long long>(string) >= 0x10000
                && length > 0
                && length <= max_len
                && capacity == length + 1;
        }
    };

} // namespace structs

namespace network {

    namespace helper {

        // Function pointer typedefs
        typedef void(__fastcall* network_setup_t)(uint64_t* a1);
        typedef void(__fastcall* network_cleanup_t)(uint64_t* a1);
        typedef void(__fastcall* network_add_param_byte_t)(uint64_t* a1, uint8_t a2);
        typedef void(__fastcall* network_add_param_int_t)(uint64_t* a1, int32_t a2);
        typedef void(__fastcall* network_add_param_int64_t)(uint64_t* a1, int64_t a2);
        typedef void(__fastcall* network_add_param_string_t)(uint64_t* a1, structs::BufferStruct a2);

        // Shared function pointers (one per program, not per TU)
        inline network_setup_t network_setup = nullptr;
        inline network_cleanup_t network_cleanup = nullptr;
        inline network_add_param_byte_t network_add_param_byte = nullptr;
        inline network_add_param_int_t network_add_param_int = nullptr;
        inline network_add_param_int64_t network_add_param_int64 = nullptr;
        inline network_add_param_string_t network_add_param_string = nullptr;

        // Initialize
        inline bool initialize() {
            static bool initialized = false;
            static bool success = false;

            if (initialized) return success;
            initialized = true;

            network_setup = (network_setup_t)(sdk::find_pattern(nullptr, "40 53 48 83 EC ? 48 83 CA"));
            network_cleanup = (network_cleanup_t)(sdk::find_pattern(nullptr, "40 53 48 83 EC ? 48 8D 05 ? ? ? ? 48 8B D9 ? ? ? 48 8D 05 ? ? ? ? 48 89 41 ? E8"));
            network_add_param_byte = (network_add_param_byte_t)(sdk::find_pattern(nullptr, "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 0F B6 DA 48 8B F1 BA"));
            network_add_param_int = (network_add_param_int_t)(sdk::find_pattern(nullptr, "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 8B FA 48 8B F1 33 D2"));
            network_add_param_int64 = (network_add_param_int64_t)(sdk::find_pattern(nullptr, "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B FA 48 8B F1 BA ? ? ? ? 48 83 C1 ? E8 ? ? ? ? 48 8B D7"));
            network_add_param_string = (network_add_param_string_t)(sdk::find_pattern(nullptr, "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B FA 48 8B D9 BA"));

            success = (network_setup && network_cleanup &&
                network_add_param_byte && network_add_param_int &&
                network_add_param_int64 && network_add_param_string);
            return success;
        }

        // Helper functions
        inline void add_byte(uint64_t* buffer, uint8_t value) {
            if (network_add_param_byte) network_add_param_byte(buffer, value);
        }

        inline void add_int(uint64_t* buffer, int32_t value) {
            if (network_add_param_int) network_add_param_int(buffer, value);
        }

        inline void add_int64(uint64_t* buffer, int64_t value) {
            if (network_add_param_int64) network_add_param_int64(buffer, value);
        }

        inline void add_string(uint64_t* buffer, const char* value) {
            if (network_add_param_string) {
                static auto make_buffer_struct = [](const char* str) -> structs::BufferStruct {
                    structs::BufferStruct buffer;
                    buffer.length = std::strlen(str);
                    buffer.capacity = std::strlen(str) + 1;
                    buffer.string = str;
                    return buffer;
                    };
                structs::BufferStruct str_buffer = make_buffer_struct(value);
                network_add_param_string(buffer, str_buffer);
            }
        }

        inline void add_string(uint64_t* buffer, const structs::BufferStruct& value) {
            if (network_add_param_string) {
                network_add_param_string(buffer, value);
            }
        }

        inline void add_network_param(uint64_t* buffer, bool value) {
            add_byte(buffer, static_cast<uint8_t>(value));
        }

        inline void add_network_param(uint64_t* buffer, int32_t value) {
            add_int(buffer, value);
        }

        inline void add_network_param(uint64_t* buffer, int64_t value) {
            add_int64(buffer, value);
        }

        inline void add_network_param(uint64_t* buffer, const char* value) {
            add_string(buffer, value);
        }

        inline void add_network_param(uint64_t* buffer, const structs::BufferStruct& value) {
            add_string(buffer, value);
        }

    } // namespace helper


    template<typename... Args>
    inline void call(int dispatcher_offset, int extra_offset, const char* event_name, __int64 a1, Args... args) {
        if (!a1) return;
        if (!helper::initialize()) return;

        // get vtable functions
        typedef void(__fastcall* network_call_t)(__int64 a1, void* a2, uint64_t* a3);
        typedef __int64(__fastcall* network_extra_t)(__int64 a1);

        network_call_t network_call = *(network_call_t*)(*(uint64_t*)a1 + dispatcher_offset);
        network_extra_t network_extra = extra_offset >= 0 ? *(network_extra_t*)(*(uint64_t*)a1 + extra_offset) : nullptr;
        if (!network_call) return;

        uint64_t v5[14];
        memset(v5, 0, sizeof(v5));
        helper::network_setup(v5);

        (helper::add_network_param(v5, args), ...);
        if (network_extra) network_extra(a1);

        struct v4_struct {
            const char* name;
            uint64_t flags;
            uint64_t zero;
        } v4;

        v4.name = event_name;
        v4.flags = 3;
        v4.zero = 0;

        // make the call
        network_call(a1, &v4, v5);
        helper::network_cleanup(v5);
    }

} // namespace network