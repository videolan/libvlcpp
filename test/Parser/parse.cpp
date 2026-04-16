/*****************************************************************************
 * parse.cpp: Parser::queue regression test
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

#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <iostream>

int main(int ac, char** av)
{
    if (ac < 2)
    {
        std::cerr << "usage: " << av[0] << " <file to parse>" << std::endl;
        return 1;
    }

    const char* vlcArgs = "-vv";
    auto instance = VLC::Instance(1, &vlcArgs);

    /* default config */
    VLC::Parser parser(instance);
    VLC::Media media(av[1], VLC::Media::FromPath);
    VLC::Parser::Request req(media);
    req.setParseFlags(VLC::Parser::ParseFlags::Parse |
                      VLC::Parser::ParseFlags::FetchLocal);

    VLC::Parser::Status parserStatus = VLC::Parser::Status::Failed;
    bool parsingFinished = false;
    std::atomic<int64_t> reportedDuration{-1};
    std::mutex stateMutex;
    std::condition_variable stateCv;
    std::unique_ptr<VLC::Parser::TaskIdentifier> queuedTaskId;

    VLC::Parser::Callbacks cbs([&](VLC::Parser::Task&& task, VLC::Parser::Status status) {
        std::lock_guard<std::mutex> lk(stateMutex);
        assert(*queuedTaskId == task);
        auto media = task.getMedia();
        assert(media.isValid());
        reportedDuration.store(media.duration().count());
        parserStatus = status;
        parsingFinished = true;
        stateCv.notify_all();
    });

    /* block until the parsing is done. Hold stateMutex across queue() so the
       callback (which also locks stateMutex) cannot observe queuedTaskId
       before we assign it */
    {
        std::unique_lock<std::mutex> lk(stateMutex);
        queuedTaskId.reset(new VLC::Parser::TaskIdentifier(parser.queue(req, cbs)));
        assert(stateCv.wait_for(lk, std::chrono::seconds(5),
                                [&] { return parsingFinished; }));
    }

    assert(parserStatus == VLC::Parser::Status::Done);
    assert(reportedDuration.load() > 0);

    return 0;
}
