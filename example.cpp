// File: example.cpp
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

#include "network.hpp" // main header
#include "utils/memory_utils.hpp" // for sdk::find_pattern

namespace hook::utils {

    __int64 GetLocalComponentViaKey(int key) {
        typedef __int64(*t__int64_0)();
        typedef __int64(__fastcall* t__int64_2)(__int64, int);
        static t__int64_0 oGetLocalPlayer = nullptr; // 48 8D 0D ? ? ? ? E9 ? ? ? ? ? ? ? ? 8B 05 ? ? ? ? C3
        static t__int64_2 oGetComponentByID = nullptr; // 4C 8B C9 33 C9 49 8B 81 ? ? ? ? 49 2B 81 ? ? ? ? ? ? ? 74 ? 4D 8B 81 ? ? ? ? 90
        if (!oGetLocalPlayer) oGetLocalPlayer = (t__int64_0)(sdk::find_pattern(nullptr, "48 8D 0D ? ? ? ? E9 ? ? ? ? ? ? ? ? 8B 05 ? ? ? ? C3"));
        if (!oGetComponentByID) oGetComponentByID = (t__int64_2)(sdk::find_pattern(nullptr, "4C 8B C9 33 C9 49 8B 81 ? ? ? ? 49 2B 81 ? ? ? ? ? ? ? 74 ? 4D 8B 81 ? ? ? ? 90"));
        if (!oGetLocalPlayer || !oGetComponentByID) return 0;

        __int64 entity = oGetLocalPlayer();
        if (!entity) return 0; // in loading screen
        __int64 result = oGetComponentByID(entity, key);
        return result;
    }

} // namespace hook::utils

// examples
void RequestSpecificWorld(int64_t world_id) {
    uint64_t v5[14];
    memset(v5, 0, 0x60u);
    network::setup(v5);
    network::add_int64(v5, world_id);
    network::call("RequestSpecificWorld", hook::utils::GetViaSessionKey(376), v5);
}

void JoinPlayer(const char* player_name) {
    uint64_t v5[14];
    memset(v5, 0, 0x60u);
    network::setup(v5);
    network::add_string(v5, player_name);
    network::call("JoinPlayer", hook::utils::GetViaSessionKey(376), v5);
}

void InvitePlayer(const char* player_name) {
    uint64_t v5[14];
    memset(v5, 0, 0x60u);
    network::setup(v5);
    network::add_string(v5, player_name);
    network::add_int(v5, 0);
    network::call("InvitePlayer", hook::utils::GetViaSessionKey(376), v5);
}

void EquipPetAppearance(const char* pet_path) {
    uint64_t v5[14];
    memset(v5, 0, 0x60u);
    network::setup(v5);
    network::add_string(v5, pet_path);
    network::call("EquipPetAppearance", hook::utils::GetViaSessionKey(219), v5);
}

void SetClass(int32_t class_id) {
    uint64_t v5[14];
    memset(v5, 0, 0x60u);
    network::setup(v5);
    network::add_int(v5, class_id);
    network::add_byte(v5, 0);
    network::call("SetClass", hook::utils::GetViaSessionKey(10), v5);
}

void example() {
    RequestSpecificWorld((int64_t)7164068446503133809LL);
    JoinPlayer("player_name");
    InvitePlayer("player_name");
    EquipPetAppearance("collections/pet/mug_rootbeer");
    SetClass(17); // solarion
}
