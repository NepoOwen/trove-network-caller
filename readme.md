# Trove Network Caller

A small header-only library for calling Trove's internal network dispatcher from an injected DLL. It lets you send game network events (world requests, joins, invites, etc.) with strongly typed parameters, without manually constructing the game's parameter buffers.

## Features

- Single header (`network.hpp`) — just include it and call `network::call`.
- Variadic template dispatcher — pass any number of parameters of supported types.
- Automatic pattern scanning of the required game functions at first use.
- Supported parameter types:
  - `bool`
  - `int32_t`
  - `int64_t`
  - `const char*` (strings)
  - `structs::BufferStruct`

## Requirements

- Windows x64
- MSVC (Visual Studio)
- C++20 or newer (uses `std::format` and `inline` variables)
- The code must run inside the Trove process (e.g., from an injected DLL)

## Build

There is no project file — this is meant to be dropped into your existing DLL project:

1. Add this folder to your include paths.
2. `#include "network.hpp"` where needed.
3. Add `utils/memory_utils.cpp` to your project so it compiles and links.

The rest of the library is header-only.

## Usage

```cpp
#include "network.hpp"

// network::call(dispatcher_offset, extra_offset, event_name, session, args...)
network::call(192, 200, "JoinPlayer", session, "player_name");
network::call(192, -1, "EquipPetAppearance", session, "collections/pet/mug_rootbeer");
network::call(192, 200, "RequestSpecificWorld", session, (int64_t)7164068446503133809LL);
```

- `dispatcher_offset` — byte offset into the dispatcher vtable for the call function.
- `extra_offset` — byte offset into the vtable for the optional "extra" function; pass `-1` to skip it.
- `event_name` — the network event string the game expects.
- `session` — the session object obtained from the game (see `example.cpp`).
- `args...` — the event parameters, in order. Each is appended via the matching parameter type.

`network::call` initializes itself on the first call and silently returns if initialization fails or `session` is null.

## Example

`example.cpp` shows a complete session helper (`GetViaSessionKey`) plus working examples for joining a world by world id, joining/inviting a player, and equipping a pet appearance.

## Disclaimer

The signature patterns, vtable offsets, and event names in this repository are version-specific and verified against a particular Trove client build. Game updates can break them — if calls stop working, the patterns and offsets need to be re-verified and updated for the current client. This project is provided for educational purposes.

## License

MIT — see [LICENSE](LICENSE).
