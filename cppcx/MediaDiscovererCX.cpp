/*****************************************************************************
* MediaDiscovererCX.cpp: MediaDiscoverer API
*****************************************************************************
* Copyright © 2015 libvlcpp authors & VideoLAN
*
* Authors: Alexey Sokolov <alexey+vlc@asokolov.org>
*          Hugo Beauzée-Luyssen <hugo@beauzee.fr>
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

#include "InstanceCX.hpp"
#include "MediaDiscovererCX.hpp"

namespace libVLCX
{
    MediaDiscoverer::MediaDiscoverer(Instance^ inst, Platform::String^ name) :
        m_discoverer(inst->m_instance, FromPlatformString(name))
    {
    }

    bool MediaDiscoverer::start()
    {
        return m_discoverer.start();
    }

    void MediaDiscoverer::stop()
    {
        m_discoverer.stop();
    }

    bool  MediaDiscoverer::isRunning()
    {
        return m_discoverer.isRunning();
    }

    MediaList^ MediaDiscoverer::mediaList()
    {
        if (m_media_list == nullptr)
            m_media_list = ref new MediaList(m_discoverer);
        return m_media_list;
    }
}
