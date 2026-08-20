// File: memory_utils.hpp
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

#pragma once

#define TROVE_EXE_A "Trove_x64.exe"
#define TROVE_EXE_W L"Trove_x64.exe"

#include <cstdint>  // for std::uint8_t
#include <cstddef>  // for std::size_t
#include <vector>

namespace sdk {
    
    template <std::size_t Size>
    class padding {
    private:
        std::uint8_t m_pad[Size];
    };

    template <class Function = void*, std::size_t Index>
    Function virtual_function_get(void* vmt) {
        return (*static_cast<Function**>(vmt))[Index];
    }

    std::uint8_t* find_pattern(const char* module_name, const char* pattern);

    std::uint8_t* find_pattern(const char* module_name, const int* pattern_data, std::size_t pattern_size);

    // Non-throwing variant: returns nullptr on any failure instead of throwing.
    std::uint8_t* find_pattern_mm(const char* module_name, const int* pattern_data, std::size_t pattern_size) noexcept;

    std::vector<std::uint8_t*> find_pattern_all(const char* module_name, const char* pattern);

    std::uint8_t* resolve_absolute_rip_address(std::uint8_t* instruction,
                                               std::size_t offset_to_displacemnt,
                                               std::size_t instruction_size);

    std::uint8_t* read_pattern_pointer(const char* module_name,
                                       const char* pattern,
                                       std::size_t offset_to_displacement,
                                       std::size_t instruction_size);

    bool IsValidPointer(const void* ptr, std::size_t min_size = 1);

    bool IsValidCString(const char* ptr, std::size_t max_length = 256);

}  // namespace sdk
