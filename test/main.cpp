#include "vlc.hpp"

#include <iostream>
#include <thread>

int main(int ac, char** av)
{
    if (ac < 2)
    {
        std::cerr << "usage: " << av[0] << " <file to play>" << std::endl;
        return 1;
    }
    auto instance = std::make_shared<VLC::Instance>(0, nullptr);
    auto media = std::make_shared<VLC::Media>(instance, av[1], VLC::Media::FromPath);
    auto mp = std::make_shared<VLC::MediaPlayer>(media);
    mp->play();

    std::this_thread::sleep_for( std::chrono::seconds( 20 ) );
    mp->stop();

    // This should not compile:
    //VLC::Media m = *(media.get());
}
