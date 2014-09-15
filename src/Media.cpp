/*****************************************************************************
 * libvlc_Media.cpp: Media implementation
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

namespace VLC
{

Media Media::fromPath(Instance& instance, const std::string& path)
{
    InternalPtr ptr = libvlc_media_new_path( instance, path.c_str() );
    return Media(ptr);
}

Media Media::fromLocation(Instance& instance, const std::string& location)
{
    InternalPtr ptr = libvlc_media_new_location( instance, location.c_str() );
    return Media(ptr);
}

Media Media::fromFileDescriptor(Instance& instance, int fd)
{
    InternalPtr ptr = libvlc_media_new_fd( instance, fd );
    return Media( ptr );
}

Media Media::fromList(MediaList& list)
{
    InternalPtr ptr = libvlc_media_list_media( list );
    return Media( ptr );
}

Media Media::asNode(Instance& instance, const std::string& nodeName)
{
    InternalPtr ptr = libvlc_media_new_as_node( instance, nodeName.c_str() );
    return Media( ptr );
}

Media::Media(const Media& another)
    : Internal(another)
{
    retain();
    if ( another.m_eventManager != NULL )
        m_eventManager = new EventManager( *another.m_eventManager );
}

Media& Media::operator=(const Media& another)
{
    if (this == &another)
    {
        return *this;
    }
    release();
    m_obj = another.m_obj;
    delete m_eventManager;
    if ( another.m_eventManager != NULL )
        m_eventManager = new EventManager( *another.m_eventManager );
    retain();
    return *this;
}

bool Media::operator==(const Media& another) const
{
    return m_obj == another.m_obj;
}

Media::~Media()
{
    delete m_eventManager;
    release();
}

void Media::addOption(const std::string& psz_options)
{
    libvlc_media_add_option(m_obj, psz_options.c_str());
}

void Media::addOptionFlag(const std::string& psz_options, unsigned i_flags)
{
    libvlc_media_add_option_flag(m_obj, psz_options.c_str(), i_flags);
}

std::string Media::mrl()
{
    char * c_result = libvlc_media_get_mrl(m_obj);
    std::string result = c_result;
    libvlc_free(c_result);
    return result;
}

Media Media::duplicate()
{
    InternalPtr obj = libvlc_media_duplicate(m_obj);
    return Media( obj );
}

std::string Media::meta(libvlc_meta_t e_meta)
{
    char * c_result = libvlc_media_get_meta(m_obj, e_meta);
    std::string result = c_result;
    libvlc_free(c_result);
    return result;
}

void Media::setMeta(libvlc_meta_t e_meta, const std::string& psz_value)
{
    libvlc_media_set_meta(m_obj, e_meta, psz_value.c_str());
}

int Media::saveMeta()
{
    return libvlc_media_save_meta(m_obj);
}

libvlc_state_t Media::state()
{
    return libvlc_media_get_state(m_obj);
}

bool Media::stats(libvlc_media_stats_t * p_stats)
{
    return libvlc_media_get_stats(m_obj, p_stats);
}

EventManager& Media::eventManager()
{
    if ( m_eventManager == NULL )
    {
        libvlc_event_manager_t* obj = libvlc_media_event_manager(m_obj);
        m_eventManager = new EventManager( obj );
    }
    return *m_eventManager;
}

libvlc_time_t Media::duration()
{
    return libvlc_media_get_duration(m_obj);
}

void Media::parse()
{
    libvlc_media_parse(m_obj);
}

void Media::parseAsync()
{
    libvlc_media_parse_async(m_obj);
}

bool Media::isParsed()
{
    return libvlc_media_is_parsed(m_obj);
}

void Media::setUserData(void * p_new_user_data)
{
    libvlc_media_set_user_data(m_obj, p_new_user_data);
}

void * Media::userData()
{
    return libvlc_media_get_user_data(m_obj);
}

std::vector<MediaTrack> Media::tracks()
{
    libvlc_media_track_t**  tracks;
    uint32_t                nbTracks = libvlc_media_tracks_get(m_obj, &tracks);
    std::vector<MediaTrack> res;

    if ( nbTracks == 0 )
        return res;

    for ( uint32_t i = 0; i < nbTracks; ++i )
        res.push_back( MediaTrack(tracks[i]));
    libvlc_media_tracks_release( tracks, nbTracks );
    return res;
}

Media::Media(Internal::InternalPtr ptr)
    : Internal(ptr)
    , m_eventManager( NULL )
{
}

void Media::retain()
{
    libvlc_media_retain(m_obj);
}

void Media::release()
{
    libvlc_media_release(m_obj);
}

} // namespace VLC

