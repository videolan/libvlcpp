# Copyright (C) 2014-2026 VideoLAN - VideoLabs

mediaplayer_test_sources = files('main.cpp')

mediaplayer_test_exe = executable(
    'mediaplayer-test',
    sources: mediaplayer_test_sources,
    dependencies: [libvlc_dep],
    include_directories: [vlcpp_includes],
)

test('mediaplayer-test', mediaplayer_test_exe, args: test_sample)
