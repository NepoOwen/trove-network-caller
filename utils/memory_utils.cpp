// File: memory_utils.cpp
// Author: NepoOwen
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

#include "memory_utils.hpp"

#include <vector>
#include <stdexcept>
#include <format>
#include <cstring>
#include <cstdlib>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // !WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace sdk {

    static std::vector<int> ida_pattern_to_bytes(const char* pattern) {
        std::vector<int> bytes = {};

        char* start = const_cast<char*>(pattern);
        const char* end = const_cast<char*>(pattern) + std::strlen(pattern);

        for (char* current = start; current < end; ++current) {
            if (*current == '?') {
                ++current;

                if (*current == '?') {
                    ++current;
                }

                bytes.push_back(-1);
            } else {
                bytes.push_back(std::strtoul(current, &current, 16));
            }
        }

        return bytes;
    }

    static std::pair<std::uint8_t*, DWORD> get_module_image(const char* module_name) {
        const HMODULE module_handle = GetModuleHandleA(module_name ? module_name : TROVE_EXE_A);
        if (module_handle == nullptr) {
            throw std::runtime_error(
                std::format("failed to get handle for module \"{}\"", module_name));
        }

        auto* image_data = reinterpret_cast<std::uint8_t*>(module_handle);
        const auto* dos_header = reinterpret_cast<const IMAGE_DOS_HEADER*>(image_data);
        const auto* nt_headers = reinterpret_cast<const IMAGE_NT_HEADERS*>(
            image_data + dos_header->e_lfanew);

        return { image_data, nt_headers->OptionalHeader.SizeOfImage };
    }

    std::uint8_t* find_pattern(const char* module_name, const char* pattern) {
        const auto [image_data, image_size] = get_module_image(module_name);

        const std::vector<int> bytes = ida_pattern_to_bytes(pattern);
        const std::size_t pattern_size = bytes.size();
        const int* pattern_data = bytes.data();

        if (pattern_size == 0 || image_size < pattern_size) {
            throw std::runtime_error(
                std::format("invalid pattern \"{}\"", pattern));
        }

        const std::size_t scan_limit = static_cast<std::size_t>(image_size) - pattern_size + 1;
        for (std::size_t i = 0; i < scan_limit; ++i) {
            bool found = true;

            for (std::size_t j = 0ul; j < pattern_size; ++j) {
                if (image_data[i + j] != pattern_data[j] && pattern_data[j] != -1) {
                    found = false;
                    break;
                }
            }

            if (found == true) {
                return &image_data[i];
            }
        }

        throw std::runtime_error(
            std::format("failed to find pattern \"{}\"", pattern));
    }

    std::uint8_t* find_pattern(const char* module_name, const int* pattern_data, std::size_t pattern_size) {
        const auto [image_data, image_size] = get_module_image(module_name);

        if (!pattern_data || pattern_size == 0 || image_size < pattern_size) {
            throw std::runtime_error(
                std::format("invalid pattern data"));
        }

        const std::size_t scan_limit = static_cast<std::size_t>(image_size) - pattern_size + 1;

        for (std::size_t i = 0; i < scan_limit; ++i) {
            bool found = true;

            for (std::size_t j = 0ul; j < pattern_size; ++j) {
                if (image_data[i + j] != pattern_data[j] && pattern_data[j] != -1) {
                    found = false;
                    break;
                }
            }

            if (found == true) {
                return &image_data[i];
            }
        }

        throw std::runtime_error(
            std::format("failed to find pattern"));
    }

    std::uint8_t* find_pattern_mm(const char* module_name, const int* pattern_data, std::size_t pattern_size) noexcept {
        if (!pattern_data || pattern_size == 0) return nullptr;
        HMODULE mod = GetModuleHandleA(module_name ? module_name : TROVE_EXE_A);
        if (!mod) return nullptr;
        auto* image = reinterpret_cast<std::uint8_t*>(mod);
        const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(image);
        const auto* nt  = reinterpret_cast<const IMAGE_NT_HEADERS*>(image + dos->e_lfanew);
        DWORD image_size = nt->OptionalHeader.SizeOfImage;
        if (static_cast<std::size_t>(image_size) < pattern_size) return nullptr;
        const std::size_t scan_limit = static_cast<std::size_t>(image_size) - pattern_size + 1;
        for (std::size_t i = 0; i < scan_limit; ++i) {
            bool found = true;
            for (std::size_t j = 0; j < pattern_size; ++j) {
                if (image[i + j] != static_cast<std::uint8_t>(pattern_data[j]) && pattern_data[j] != -1) {
                    found = false;
                    break;
                }
            }
            if (found) return &image[i];
        }
        return nullptr;
    }

    std::vector<std::uint8_t*> find_pattern_all(const char* module_name, const char* pattern) {
        const auto [image_data, image_size] = get_module_image(module_name);

        const std::vector<int> bytes = ida_pattern_to_bytes(pattern);
        const std::size_t pattern_size = bytes.size();
        const int* pattern_data = bytes.data();

        std::vector<std::uint8_t*> results;

        if (pattern_size == 0 || image_size < pattern_size) {
            throw std::runtime_error(
                std::format("invalid pattern \"{}\"", pattern));
        }

        const std::size_t scan_limit = static_cast<std::size_t>(image_size) - pattern_size + 1;
        for (std::size_t i = 0; i < scan_limit; ++i) {
            bool found = true;

            for (std::size_t j = 0ul; j < pattern_size; ++j) {
                if (image_data[i + j] != pattern_data[j] && pattern_data[j] != -1) {
                    found = false;
                    break;
                }
            }

            if (found) {
                results.push_back(&image_data[i]);
            }
        }

		if (results.empty()) {
			throw std::runtime_error(
				std::format("failed to find pattern \"{}\"", pattern));
		}

        return results;
    }

    std::uint8_t* resolve_absolute_rip_address(std::uint8_t* instruction,
                                               std::size_t offset_to_displacement,
                                               std::size_t instruction_size) {
        auto displacement = *reinterpret_cast<const std::int32_t*>(
            reinterpret_cast<std::uintptr_t>(instruction) + offset_to_displacement);
        return instruction + instruction_size + displacement;
    }

    std::uint8_t* read_pattern_pointer(const char* module_name,
        const char* pattern,
        std::size_t offset_to_displacement,
        std::size_t instruction_size) {
        auto* instruction = sdk::find_pattern(module_name, pattern);
        auto* pointer = sdk::resolve_absolute_rip_address(instruction, offset_to_displacement, instruction_size);
        return pointer;
    }

    bool IsValidPointer(const void* ptr, std::size_t min_size) {
        if (!ptr) return false;
        MEMORY_BASIC_INFORMATION mbi{};
        if (VirtualQuery(ptr, &mbi, sizeof(mbi)) == 0) return false;
        if (mbi.State != MEM_COMMIT) return false;
        if (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)) return false;
        const auto region_end = reinterpret_cast<const uint8_t*>(mbi.BaseAddress) + mbi.RegionSize;
        const auto ptr_end    = reinterpret_cast<const uint8_t*>(ptr) + min_size;
        return ptr_end <= region_end;
    }

    bool IsValidCString(const char* ptr, std::size_t maxLength) {
        if (!ptr || maxLength == 0) return false;
        MEMORY_BASIC_INFORMATION mbi{};
        if (!VirtualQuery(ptr, &mbi, sizeof(mbi))) return false;
        if (mbi.State != MEM_COMMIT) return false;
        if (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)) return false;

        const void* end = memchr(ptr, '\0', maxLength);
        if (!end) return false;

        for (const char* p = ptr; p < static_cast<const char*>(end); ++p) {
            const unsigned char c = static_cast<unsigned char>(*p);

            if (!(
                (c >= 32 && c <= 126) ||
                c == '\r' ||
                c == '\n' ||
                c == '\t'))
            {
                return false;
            }
        }

        return true;
    }

}  // namespace sdk