# Copyright (C) 2014-2025 VideoLAN - VideoLabs

regression_test_sources = files('main.cpp')

regression_test_exe = executable(
    'regression-test',
    sources: regression_test_sources,
    dependencies: [libvlc_dep],
    include_directories: [vlcpp_includes],
)

regression_test_sample = files('sample.mp4')

test('regression-test', regression_test_exe, args: regression_test_sample)
