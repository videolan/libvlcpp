/*****************************************************************************
 * libvlc_MediaDiscoverer.cpp: MediaDiscoverer implementation
 *****************************************************************************
 * Copyright © 2014 the VideoLAN team
 *
 * Authors: Alexey Sokolov <alexey@alexeysokolov.co.cc>
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

#include "vlc.hpp"

#include "EventManager.hpp"
#include <stdexcept>
#include <memory>

namespace VLC
{

MediaDiscoverer::MediaDiscoverer(InstancePtr inst, const std::string& name )
    : Internal{ libvlc_media_discoverer_new_from_name(inst->get(), name.c_str()),
                libvlc_media_discoverer_release }
{
}

std::string MediaDiscoverer::localizedName()
{
    char* c_result = libvlc_media_discoverer_localized_name(get());
    if ( c_result == NULL )
        return std::string();
    std::string result = c_result;
    libvlc_free(c_result);
    return result;
}

EventManagerPtr MediaDiscoverer::eventManager()
{
    if ( m_eventManager )
    {
        libvlc_event_manager_t* obj = libvlc_media_discoverer_event_manager( get() );
        m_eventManager = std::make_shared<EventManager>( obj );
    }
    return m_eventManager;
}

bool MediaDiscoverer::isRunning()
{
    return libvlc_media_discoverer_is_running(get());
}

} // namespace VLC

