/*****************************************************************************
 * libvlc_MediaDiscoverer.hpp: MediaDiscoverer API
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

#ifndef LIBVLC_CXX_MEDIADISCOVERER_H
#define LIBVLC_CXX_MEDIADISCOVERER_H

#include "common.hpp"

#include <string>

namespace VLC
{

class MediaDiscovererEventManager;
class Instance;

class MediaDiscoverer : public Internal<libvlc_media_discoverer_t>
{
public:
    // libvlc_media_discoverer_new_from_name
    /**
     * Discover media service by name.
     *
     * \param p_inst  libvlc instance
     *
     * \param psz_name  service name
     * \warning This is returned as a pointer, as this is not refcounter by VLC, and is
     *          fairly expensive to instantiate.
     */
    MediaDiscoverer(Instance& inst, const std::string& name)
        : Internal{ libvlc_media_discoverer_new_from_name(getInternalPtr<libvlc_instance_t>( inst ), name.c_str()),
                    libvlc_media_discoverer_release }
    {
    }

    /**
     * Get media service discover object its localized name.
     *
     * \return localized name
     */
    std::string localizedName()
    {
        auto str = wrapCStr( libvlc_media_discoverer_localized_name(*this) );
        if ( str == nullptr )
            return {};
        return str.get();
    }

    /**
     * Get event manager from media service discover object.
     *
     * \return event manager object.
     */
    MediaDiscovererEventManager& eventManager()
    {
        if ( m_eventManager )
        {
            libvlc_event_manager_t* obj = libvlc_media_discoverer_event_manager( *this );
            m_eventManager = std::make_shared<MediaDiscovererEventManager>( obj );
        }
        return *m_eventManager;
    }

    /**
     * Query if media service discover object is running.
     *
     * \return true if running, false if not
     */
    bool isRunning()
    {
        return libvlc_media_discoverer_is_running(*this);
    }

private:
    std::shared_ptr<MediaDiscovererEventManager> m_eventManager;
};

} // namespace VLC

#endif

