# Trove Network Caller

A small header-only library for calling Trove's internal network dispatcher from an injected DLL. It lets you send game network events (world requests, joins, invites, etc.) with strongly typed parameters, without manually constructing the game's parameter buffers.

## Features

- Single header (`network.hpp`) — just include it and use `network::setup` / `network::add_*` / `network::call`.
- Automatic pattern scanning of the required game functions at first use.
- Supported parameter types:
  - `uint8_t` (byte)
  - `int32_t` (int)
  - `uint32_t` (uint)
  - `int64_t` (int64)
  - `float`
  - `double`
  - `Vec3`
  - `const char*` (strings)
  - `structs::BufferStruct`

## Requirements

- Windows x64
- MSVC (Visual Studio)
- The code must run inside the Trove process (e.g., from an injected DLL)

## Build

There is no project file — this is meant to be dropped into your existing DLL project:

1. Add this folder to your include paths.
2. `#include "network.hpp"` where needed.
3. Make sure the underlying pattern-scanning (`sdk::find_pattern`) is available and linked.

The rest of the library is header-only.

## Usage

```cpp
#include "network.hpp"

void EquipPetAppearance(const char* pet_path) {
    uint64_t v5[14];
    memset(v5, 0, 0x60u);
    network::setup(v5);
    network::add_string(v5, pet_path);
    network::call("EquipPetAppearance", hook::utils::GetViaSessionKey(219), v5);
}
```

- `network::setup(buffer)` — zero-initializes the buffer and calls the game's network setup (pattern-scanned).
- `network::add_* (buffer, value)` — appends a typed parameter (`add_byte`, `add_int`, `add_uint`, `add_int64`, `add_float`, `add_double`, `add_vec3`, `add_string`).
- `network::call(event_name, session, buffer)` — resolves the dispatcher from the session vtable (offset `192`), optionally runs the "extra" function (offset `200`) when `extra_offset != -1`, then invokes the network call with `{ name, flags=3, zero=0 }` and finally cleans up the buffer.

`network::setup`/`network::call` initialize themselves (pattern scanning) on first use and silently return if initialization fails or `session` is null.

## Example

The reference send path for `RequestSpecificWorld` with a single `int64` parameter:

```cpp
__int64 a1 = hook::utils::GetViaSessionKey(376);
if (!a1) return;

uint64_t v5[14];
memset(v5, 0, 0x60u);
network::setup(v5);
network::add_int64(v5, 7164068446503133809LL);
network::call("RequestSpecificWorld", a1, v5);
```

This mirrors the game's own native send path: resolve `network_setup`/`network_cleanup`/`network_add_param_*` via patterns, build the buffer, resolve the dispatcher from vtable offsets `192` (call) and `200` (extra), invoke `network_extra(a1)` then `network_call(a1, &v4, v5)` with `{ name, flags=3, zero=0 }`, then `network_cleanup(v5)`.

## Disclaimer

The signature patterns, vtable offsets, and event names in this repository are version-specific and verified against a particular Trove client build. Game updates can break them — if calls stop working, the patterns and offsets need to be re-verified and updated for the current client. This project is provided for educational purposes.

## License

MIT — see [LICENSE](LICENSE).
