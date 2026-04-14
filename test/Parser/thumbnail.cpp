/*****************************************************************************
 * thumbnail.cpp: Parser::queueThumbnailing regression test
 *****************************************************************************
 * Copyright © 2026 libvlcpp authors & VideoLAN
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

#include <cassert>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <iostream>
#include <memory>
#include <mutex>
#include <utility>

int main(int ac, char** av)
{
    if (ac < 2)
    {
        std::cerr << "usage: " << av[0] << " <file to thumbnail>" << std::endl;
        return 1;
    }

    const char* vlcArgs = "-vv";
    auto instance = VLC::Instance(1, &vlcArgs);

    VLC::Parser parser(instance);
    VLC::Media media(av[1], VLC::Media::FromPath);
    VLC::Parser::ThumbnailerRequest thumbReq(media);
    thumbReq.setSize(320, 240, true)
            .setPictureType(VLC::Picture::Type::Png)
            .setSeekPosition(0.5, VLC::Parser::ThumbnailSeekSpeed::Fast);

    std::mutex mtx;
    std::condition_variable cv;
    VLC::Picture captured;
    bool pictureReceived = false;

    VLC::Parser::ThumbnailerCallbacks thumbCbs(
        [&](VLC::Parser::Task&& task, const VLC::Picture& picture) {
            assert(picture.isValid());
            std::lock_guard<std::mutex> lk(mtx);
            captured = picture;
            pictureReceived = true;
            cv.notify_all();
        }
    );

    parser.queueThumbnailing(thumbReq, thumbCbs);

    /* block until the thumbnail is received and verify some of its properties,
       then save it to a file and verify the file is non-empty before cleaning up */
    {
        std::unique_lock<std::mutex> lk(mtx);
        assert(cv.wait_for(lk, std::chrono::seconds(15),
                           [&] { return pictureReceived; }));

        assert(captured.type() == VLC::Picture::Type::Png);
        assert(captured.width() > 0);
        assert(captured.height() > 0);

        const char* outPath = "/tmp/libvlcpp-thumb-test.png";
        assert(captured.save(outPath));

        /* verify the file is non-empty, then clean up */
        if (FILE* f = std::fopen(outPath, "rb"))
        {
            std::fseek(f, 0, SEEK_END);
            const long sz = std::ftell(f);
            std::fclose(f);
            assert(sz > 0);
        }
        else
        {
            std::cerr << "saved thumbnail file could not be reopened" << std::endl;
            assert(false);
        }
        std::remove(outPath);
    }

    return 0;
}
