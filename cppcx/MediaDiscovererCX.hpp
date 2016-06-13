/*****************************************************************************
* MediaDiscovererCX.hpp: MediaDiscoverer API
*****************************************************************************
* Copyright © 2015 libvlcpp authors & VideoLAN
*
* Authors: Alexey Sokolov <alexey+vlc@asokolov.org>
*          Hugo Beauzée-Luyssen <hugo@beauzee.fr>
*          Bastien Penavayre <bastienPenava@gmail.com>
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

#pragma once

#include "MediaListCX.hpp"
#include "vlcpp\MediaDiscoverer.hpp"

namespace libVLCX
{
    ref class Instance;

    public ref class MediaDiscoverer sealed
    {
        VLC::MediaDiscoverer m_discoverer;
        MediaList^ m_media_list;

    public:

        /**
        * Discover media service by name.
        *
        * \param p_inst  libvlc instance
        *
        * \param psz_name  service name
        */
        MediaDiscoverer(Instance^ inst, Platform::String^ name);

        /**
        * Start media discovery.
        *
        * To stop it, call MediaDiscover::stop() or
        * destroy the object directly.
        *
        * \see MediaDiscover::stop
        *
        * \return false in case of error, true otherwise
        */
        bool start();

        /**
        * Stop media discovery.
        *
        * \see MediaDiscoverer::start()
        */
        void stop();

        /**
        * Query if media service discover object is running.
        *
        * \return true if running, false if not
        */
        bool isRunning();

        MediaList^ mediaList();
    };
}
