#include "vlcpp/vlc.hpp"
#include <thread>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <cassert>

struct ImemOpaque
{
    FILE* file;
    std::string path;
};

int main(int ac, char**av)
{
    if (ac < 3)
    {
        std::cerr << "usage: " << av[0] << " <file to play> <another file to play>" << std::endl;
        return 1;
    }
    auto instance = VLC::Instance(0, nullptr);
    auto dummyOpaque = new ImemOpaque{};
    dummyOpaque->path = av[1];

    VLC::Media::Callbacks imemCbs(
        // Read:
        []( void* opaque, unsigned char* buf, size_t size ) -> ssize_t {
            auto context = reinterpret_cast<ImemOpaque*>( opaque );
            auto res = fread( buf, 1, size, context->file );
            if ( res == 0 )
                return feof( context->file ) != 0 ? 0 : -1;
            return res;
        });
        imemCbs.open( [dummyOpaque]( void*, void** opaque, uint64_t* p_size ) -> int {
            dummyOpaque->file = fopen( dummyOpaque->path.c_str(), "rb" );
            *opaque = dummyOpaque;
            fseek(dummyOpaque->file, 0, SEEK_END);
            *p_size = ftell( dummyOpaque->file );
            rewind( dummyOpaque->file );
            return 0;
        })
        .seek( []( void* opaque, uint64_t seek ) -> int {
            auto context = reinterpret_cast<ImemOpaque*>( opaque );
            if ( fseek( context->file, seek, SEEK_SET ) < 0 )
                return -1;
            return 0;
        })
        .close( []( void* opaque ) {
            auto context = reinterpret_cast<ImemOpaque*>( opaque );
            fclose( context->file );
        });
    VLC::Media imemMedia( imemCbs );

    auto opaque2 = new ImemOpaque{};
    opaque2->file = fopen( av[2], "rb" );

    // Do not use a user defined opaque
    // This is mostly meant to test that our nullptr overload are functionnal
    VLC::Media::Callbacks imemCbs2(
        [opaque2]( void* opaque, unsigned char* buf, size_t size ) -> ssize_t {
            assert( opaque == nullptr );
            auto res = fread( buf, 1, size, opaque2->file );
            if ( res == 0 )
                return feof( opaque2->file ) != 0 ? 0 : -1;
            return res;
        });
        imemCbs2.seek( [opaque2]( void*, uint64_t offset ) -> int {
            if ( fseek( opaque2->file, offset, SEEK_CUR ) < -1 )
                return -1;
            return 0;
        });

    VLC::Media imemMedia2( imemCbs2 );

    auto mp = VLC::MediaPlayer( instance, imemMedia );
    mp.play();

    auto mp2 = VLC::MediaPlayer( instance, imemMedia2 );
    mp2.play();

    std::this_thread::sleep_for( std::chrono::seconds( 10 ) );

    mp.stopAsync();
    mp2.stopAsync();

    delete dummyOpaque;
    fclose(opaque2->file);
    delete opaque2;
}
