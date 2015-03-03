/*****************************************************************************
 * main.cpp: Simple test program
 *****************************************************************************
 * Copyright © 2014 the VideoLAN team
 *
 * Authors: Hugo Beauzée-Luyssen <hugo@beauzee.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

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
    eventManager.onPlaying([&media]() {
        std::cout << media.mrl() << " is playing" << std::endl;
    });
    /*
     * Should trigger static_assert:
     *
     * eventManager->onPlaying([](std::string){});
     * eventManager->onPlaying([] { return 0;} );
    */
    mp.play();

    bool expected = true;

    auto& handler = mp.eventManager().onPositionChanged([&expected](float pos) {
        std::cout << "position changed " << pos << std::endl;
        assert(expected);
    });
    std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
    handler.unregister();
    // handler must be considered a dangling reference from now on.
    // We might want to fix this, but is it worth the cost of a shared/weak_pointer?
    expected = false;

    std::this_thread::sleep_for( std::chrono::seconds( 2 ) );

    expected = true;
    auto l = [&expected] (float){
        std::cout << "Lambda called" << std::endl;
        assert(expected);
    };
    auto& h1 = mp.eventManager().onTimeChanged(l);
    auto& h2 = mp.eventManager().onPositionChanged(l);

    std::this_thread::sleep_for( std::chrono::seconds( 2 ) );

    // Unregistering multiple events at once.
    // h1 and h2 are now invalid.
    mp.eventManager().unregister(h1, h2);
    expected = false;

    std::this_thread::sleep_for( std::chrono::milliseconds(500) );

    // Using scopped event manager to automatically unregister events
    {
        expected = true;
        // This is a copy. Assigning to a reference wouldn't clear the registered events
        auto em = mp.eventManager();
        em.onPositionChanged([&expected](float) {
            assert(expected);
        });
        std::this_thread::sleep_for( std::chrono::seconds(1) );
    }
    // From here, the lambda declared in the scope isn't registered anymore
    expected = false;

    std::this_thread::sleep_for( std::chrono::milliseconds(500) );

    // Showing that copying an object shares the associated eventmanager
    auto mp2 = mp;
    expected = true;
    auto& h3 = mp2.eventManager().onStopped([&expected]() {
        std::cout << "MediaPlayer stopped" << std::endl;
        assert(expected);
        // expect a single call since both media player share the same event manager
        expected = false;
    });
    mp.stop();
    // Unregister the RegisteredEvent from the other MP's event manager.
    // It will be unregistered from both, and when the object gets destroyed
    // by leaving the scope, it won't be unregistered from mp2's eventManager.
    // If it did, libvlc would assert as the event has been unregistered already.
    mp.eventManager().unregister(h3);
}
