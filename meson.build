# Copyright (C) 2014-2025 VideoLAN - VideoLabs

project(
    'libvlcpp',
    ['cpp'],
    license: 'LGPL-2.1-or-later',
    license_files: ['COPYING'],
    version: '0.1.0',
    meson_version: '>=1.1.0',
    default_options: ['cpp_std=c++11'],
)

subdir('vlcpp')

install_headers(
    libvlcpp_headers,
    subdir: 'vlcpp',
)

libvlc_dep = dependency('libvlc', version: '>=3.0')

pkg = import('pkgconfig')
pkg.generate(
    version: meson.project_version(),
    requires: ['libvlc>=3.0'],
    name: 'libvlcpp',
    description: 'libvlc C++ bindings',
)

if get_option('examples').enabled()
    subdir('examples')
endif

if get_option('tests').enabled()
    subdir('test')
endif
