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

    __int64 GetViaSessionKey(int key) {
        typedef __int64(*t__int64_0)();
        typedef __int64(__fastcall* t__int64_2)(__int64, int);
        static t__int64_0 oGetSession = nullptr; // 48 8D 0D ? ? ? ? E9 ? ? ? ? ? ? ? ? 8B 05 ? ? ? ? C3
        static t__int64_2 oDecryptViaKey = nullptr; // 4C 8B C9 33 C9 49 8B 81 ? ? ? ? 49 2B 81 ? ? ? ? ? ? ? 74 ? 4D 8B 81 ? ? ? ? 90
        if (!oGetSession) oGetSession = (t__int64_0)(sdk::find_pattern(nullptr, "48 8D 0D ? ? ? ? E9 ? ? ? ? ? ? ? ? 8B 05 ? ? ? ? C3"));
        if (!oDecryptViaKey) oDecryptViaKey = (t__int64_2)(sdk::find_pattern(nullptr, "4C 8B C9 33 C9 49 8B 81 ? ? ? ? 49 2B 81 ? ? ? ? ? ? ? 74 ? 4D 8B 81 ? ? ? ? 90"));
        if (!oGetSession || !oDecryptViaKey) return 0;

        __int64 session = oGetSession();
        if (!session) return 0; // in loading screen
        __int64 result = oDecryptViaKey(session, key);
        return result;
    }

} // namespace hook::utils

// examples
void RequestSpecificWorld(int64_t world_id) {
    network::call(192, 200, "RequestSpecificWorld", hook::utils::GetViaSessionKey(376), world_id);
}

void JoinPlayer(const char* player_name) {
    network::call(192, 200, "JoinPlayer", hook::utils::GetViaSessionKey(376), player_name);
}

void InvitePlayer(const char* player_name) {
    network::call(192, 200, "InvitePlayer", hook::utils::GetViaSessionKey(376), player_name, 0);
}

void EquipPetAppearance(const char* pet_path) {
    network::call(192, -1, "EquipPetAppearance", hook::utils::GetViaSessionKey(219), pet_path);
}

void example() {
    RequestSpecificWorld((int64_t)7164068446503133809LL);
    JoinPlayer("player_name");
    InvitePlayer("player_name");
    EquipPetAppearance("collections/pet/mug_rootbeer");
}