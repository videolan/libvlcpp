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
    auto instance = VLC::Instance(0, nullptr);

    auto audioFilters = instance.audioFilterList();
    for (const auto& f : audioFilters)
    {
        std::cout << f.name() << std::endl;
    }

    auto media = VLC::Media(instance, av[1], VLC::Media::FromPath);
    auto mp = VLC::MediaPlayer(media);
    auto eventManager = mp.eventManager();
    eventManager->onPlaying([&media]() {
        std::cout << media.mrl() << " is playing" << std::endl;
    });
    mp.play();

    std::this_thread::sleep_for( std::chrono::seconds( 10 ) );
    mp.stop();
}
