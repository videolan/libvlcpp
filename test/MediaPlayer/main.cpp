/*****************************************************************************
 * main.cpp: MediaPlayer tests
 *****************************************************************************
 * Copyright © 2015-2026 libvlcpp authors & VideoLAN
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

#include "vlcpp/vlc.hpp"

#include <atomic>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

constexpr auto playbackDuration = std::chrono::seconds(2);

/* Basic callback wiring */
void testBasicCallbacks(VLC::Instance& instance, const char* mediaPath)
{
    std::atomic<int> stateChanges{0};
    std::atomic<int> positionChanges{0};
    std::mutex stateMutex;
    std::condition_variable stateCv;
    bool reachedPlaying = false;
    bool reachedStopped = false;

    VLC::MediaPlayer::Callbacks cbs;
    cbs.onStateChanged([&](VLC::MediaPlayer::LibvlcState state) {
        ++stateChanges;
        /* signal after playback starts and stops */
        {
            std::lock_guard<std::mutex> lk(stateMutex);
            if (state == VLC::MediaPlayer::LibvlcState::Playing)
                reachedPlaying = true;
            else if (state == VLC::MediaPlayer::LibvlcState::Stopped)
                reachedStopped = true;

            stateCv.notify_all();
        }
    })
    .onPositionChanged([&](int64_t, double) {
        ++positionChanges;
    });

    VLC::MediaPlayer mp(instance, cbs);
    VLC::Media media(mediaPath, VLC::Media::FromPath);
    mp.setMedia(media);
    assert(mp.play());

    /* block until the Playing state change is delivered via the callback */
    { 
        std::unique_lock<std::mutex> lk(stateMutex);
        assert(stateCv.wait_for(lk, std::chrono::seconds(5), [&] { return reachedPlaying; }));
    }

    /* play for 2 seconds to allow position changes */
    std::this_thread::sleep_for(playbackDuration);
    mp.stopAsync();

    /* block until the Stopped state change is delivered via the callback */
    {
        std::unique_lock<std::mutex> lk(stateMutex);
        assert(stateCv.wait_for(lk, std::chrono::seconds(5), [&] { return reachedStopped; }));
    }

    assert(stateChanges.load() > 0);
    assert(positionChanges.load() > 0);
}

/* Copying a MediaPlayer via `operator=` shares the underlying
   libvlc_media_player_t*. Callbacks are registered against that single
   underlying player, so starting playback on the copy must fire the
   callbacks registered through the original's Callbacks object. */
void testCopyAssignSharesUnderlyingPlayer(VLC::Instance& instance,
                                          const char* mediaPath)
{
    std::atomic<int> positionChanges{0};
    std::atomic<bool> reachedPlaying{false};

    VLC::MediaPlayer::Callbacks cbs;
    cbs.onStateChanged([&](VLC::MediaPlayer::LibvlcState state) {
        if (state == VLC::MediaPlayer::LibvlcState::Playing)
            reachedPlaying.store(true);
    })
    .onPositionChanged([&](int64_t, double) {
        ++positionChanges;
    });

    VLC::MediaPlayer mp1(instance, cbs);
    VLC::MediaPlayer mp2 = mp1;
    assert(mp1 == mp2);

    VLC::Media media(mediaPath, VLC::Media::FromPath);
    mp2.setMedia(media);
    assert(mp2.play());
    std::this_thread::sleep_for(playbackDuration);

    /* Even though we only called play() on mp2, the original must also observe
       that playback is in progress because both wrap the same underlying
       libvlc_media_player_t. */
    assert(mp1.isPlaying());
    mp1.stopAsync();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    assert(reachedPlaying.load());
    assert(positionChanges.load() > 0);
}

/* The same MediaPlayer::Callbacks instance can be handed to two distinct
   MediaPlayer constructors. Both create independent libvlc_media_player_t
   objects but share the CallbackOwner<25> state, so callbacks must fire for
   each of the two players' activity. */
void testSharedCallbacksTwoPlayers(VLC::Instance& instance,
                                   const char* mediaPath)
{
    std::atomic<int> mediaPlaying{0};
    std::atomic<int> mediaChanges{0};
    std::mutex stateMutex;
    std::condition_variable stateCv;

    VLC::MediaPlayer::Callbacks cbs;
    cbs.onMediaChanged([&](VLC::Media&&) {
        ++mediaChanges;
    })
    .onStateChanged([&](VLC::MediaPlayer::LibvlcState state) {
        std::lock_guard<std::mutex> lk(stateMutex);
        if (state == VLC::MediaPlayer::LibvlcState::Playing)
            ++mediaPlaying;
        stateCv.notify_all();
    });

    VLC::MediaPlayer mpA(instance, cbs);
    VLC::MediaPlayer mpB(instance, cbs);
    assert(!(mpA == mpB));

    VLC::Media mediaA(mediaPath, VLC::Media::FromPath);
    VLC::Media mediaB(mediaPath, VLC::Media::FromPath);
    mpA.setMedia(mediaA);
    mpB.setMedia(mediaB);
    assert(mpA.play());
    assert(mpB.play());

    /* block until the Playing state change is delivered via the callback for both players */
    { 
        std::unique_lock<std::mutex> lk(stateMutex);
        assert(stateCv.wait_for(lk, std::chrono::seconds(5), [&] { return (mediaPlaying.load() == 2); }));
    }

    assert(mpA.isPlaying());
    assert(mpB.isPlaying());

    std::this_thread::sleep_for(playbackDuration);

    mpA.stopAsync();
    mpB.stopAsync();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    /* The shared Callbacks instance must have serviced events from both
       underlying media players. Requiring == 2 media changes */
    assert(mediaChanges.load() == 2);
}

int main(int ac, char** av)
{
    if (ac < 2)
    {
        std::cerr << "usage: " << av[0] << " <file to play>" << std::endl;
        return 1;
    }

    const char* vlcArgs = "-vv";
    auto instance = VLC::Instance(1, &vlcArgs);

    instance.logSet([](int lvl, const libvlc_log_t*, std::string message) {
        std::cout << "Hooked VLC log: " << lvl << ' ' << message << std::endl;
    });

    testBasicCallbacks(instance, av[1]);
    testCopyAssignSharesUnderlyingPlayer(instance, av[1]);
    testSharedCallbacksTwoPlayers(instance, av[1]);

    return 0;
}
