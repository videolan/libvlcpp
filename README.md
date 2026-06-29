# libvlcpp

[![Join the chat at https://discord.gg/3h3K3JF](https://img.shields.io/discord/716939396464508958?label=discord)](https://discord.gg/3h3K3JF)

**libvlcpp** is a header-only, modern C++ wrapper around
[libvlc](https://www.videolan.org/vlc/libvlc.html), the cross-platform
multimedia API powering [VLC media player](https://www.videolan.org/vlc/). It
is the official VideoLAN C++ binding and runs everywhere libvlc does.

The wrapper is intentionally thin: it adds RAII lifetime management, type
safety and C++ interfaces on top of libvlc, while mapping closely to the
underlying C API.

## Features

- **Header-only**: just needs to be added to your include path.
- **RAII**: handles are reference-counted and released automatically.
- **C++11**: works with any reasonably modern compiler and standard library.
- **Cross-platform**: across all libvlc supported platforms.

## Requirements

- A C++11-capable compiler.
- libvlc development files (`libvlc-dev` or equivalent), version 3.0 or later.

## Versioning

libvlcpp was originally designed to be retro-compatible across libvlc versions.
However, libvlc 4 introduced a large number of fundamental breaking changes to
the API, so support is now split across two branches:

| Branch    | libvlc version | Status                       |
| --------- | -------------- | ---------------------------- |
| `3.0.x`   | libvlc 3       | Stable                       |
| `master`  | libvlc 4       | In development               |

Pick the branch matching the libvlc you build against.

## Getting started

Since libvlcpp is header-only, the simplest integration is to add the repository
to your include path and include the main header:

```cpp
#include <vlcpp/vlc.hpp>
#include <thread>

int main(int argc, char** argv)
{
    auto instance = VLC::Instance(0, nullptr);
    auto media    = VLC::Media(argv[1], VLC::Media::FromPath);
    auto player   = VLC::MediaPlayer(instance, media);

    player.play();
    std::this_thread::sleep_for(std::chrono::seconds(10));
    player.stopAsync();

    return 0;
}
```

Link the resulting program against libvlc (for example `pkg-config --libs libvlc`).

### Using Meson

libvlcpp ships with Meson and can be consumed as a subproject. Drop a
wrap file in `subprojects/` and pull in the dependency:

```meson
libvlcpp_dep = dependency('libvlcpp')

executable('myplayer', 'main.cpp', dependencies: libvlcpp_dep)
```

The dependency transitively provides libvlc, so no further wiring is needed.

## Examples

More complete samples live in the [`examples`](examples) folder, including
[helloworld](examples/helloworld/main.cpp), in-memory input and renderer
discovery.

## Documentation

libvlcpp closely mirrors libvlc, so its documentation is your primary reference
alongside the wrapper headers themselves. See the
[libvlc documentation](https://videolan.videolan.me/vlc/master/index.html).

## Issues and contributing

Contributions are always welcome! Tickets and merge requests are handled on our
[GitLab](https://code.videolan.org/videolan/libvlcpp):

- [Report an issue](https://code.videolan.org/videolan/libvlcpp/-/issues)
- [Open a merge request](https://code.videolan.org/videolan/libvlcpp/-/merge_requests)

## Used by

libvlcpp is used and tested extensively across VideoLAN projects, such as the
[medialibrary](https://code.videolan.org/videolan/medialibrary) and the former
[VLC for UWP](https://code.videolan.org/videolan/vlc-winrt) app, among others.

## License

libvlcpp is released under the LGPL-2.1-or-later license. See [COPYING](COPYING)
for details.
