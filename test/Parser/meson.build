# Copyright (C) 2014-2026 VideoLAN - VideoLabs

parser_parse_sources = files('parse.cpp')

parser_parse_exe = executable(
    'parser-parse-test',
    sources: parser_parse_sources,
    dependencies: [libvlc_dep],
    include_directories: [vlcpp_includes],
)

test('parser-parse-test', parser_parse_exe, args: test_sample)

parser_thumbnail_sources = files('thumbnail.cpp')

parser_thumbnail_exe = executable(
    'parser-thumbnail-test',
    sources: parser_thumbnail_sources,
    dependencies: [libvlc_dep],
    include_directories: [vlcpp_includes],
)

test('parser-thumbnail-test', parser_thumbnail_exe, args: test_sample)
