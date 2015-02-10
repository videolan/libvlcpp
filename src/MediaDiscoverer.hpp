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
#include "Internal.hpp"
#include <string>

namespace VLC
{

class EventManager;

class VLCPP_API MediaDiscoverer : public Internal<libvlc_media_discoverer_t>
{
public:
    ~MediaDiscoverer();

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
    MediaDiscoverer(Instance & inst, const std::string& name);

    /**
     * Get media service discover object its localized name.
     *
     * \return localized name
     */
    std::string localizedName();

    /**
     * Get event manager from media service discover object.
     *
     * \return event manager object.
     */
    EventManager& eventManager();

    /**
     * Query if media service discover object is running.
     *
     * \return true if running, false if not
     */
    bool isRunning();

private:
    explicit MediaDiscoverer(InternalPtr ptr);
    /**
     * Release media discover object. If the reference count reaches 0, then
     * the object will be released.
     */
    void release();

    MediaDiscoverer(const MediaDiscoverer& another);
    MediaDiscoverer& operator=(const MediaDiscoverer& another);

private:
    EventManager* m_eventManager;
};

} // namespace VLC

#endif

