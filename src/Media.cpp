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
#include <stdexcept>

namespace VLC
{

Media::Media(InstancePtr instance, const std::string &mrl, Media::FromType type)
    : Internal{ libvlc_media_release }
{
    InternalPtr ptr = nullptr;
    switch (type)
    {
    case FromLocation:
        ptr = libvlc_media_new_location( instance->get(), mrl.c_str() );
        break;
    case FromPath:
        ptr = libvlc_media_new_path( instance->get(), mrl.c_str() );
        break;
    case AsNode:
        ptr = libvlc_media_new_as_node( instance->get(), mrl.c_str() );
        break;
    default:
        break;
    }
    if ( ptr == nullptr )
        throw std::runtime_error("Failed to construct a media");
    m_obj.reset( ptr );
}


Media::Media(InstancePtr instance, int fd)
    : Internal { libvlc_media_new_fd( instance->get(), fd ), libvlc_media_release }
{
}

Media::Media(MediaListPtr list)
    : Internal{ libvlc_media_list_media( list->get() ), libvlc_media_release }
{
}

bool Media::operator==(const Media& another) const
{
    return m_obj == another.m_obj;
}

void Media::addOption(const std::string& psz_options)
{
    libvlc_media_add_option(get(), psz_options.c_str());
}

void Media::addOptionFlag(const std::string& psz_options, unsigned i_flags)
{
    libvlc_media_add_option_flag(get(), psz_options.c_str(), i_flags);
}

std::string Media::mrl()
{
    char* c_result = libvlc_media_get_mrl(get());
    if ( c_result == NULL )
        return std::string();
    std::string result = c_result;
    libvlc_free(c_result);
    return result;
}

Media Media::duplicate()
{
    InternalPtr obj = libvlc_media_duplicate(get());
    return Media( obj, true );
}

std::string Media::meta(libvlc_meta_t e_meta)
{
    char* c_result = libvlc_media_get_meta(get(), e_meta);
    if ( c_result == NULL )
        return std::string();
    std::string result = c_result;
    libvlc_free(c_result);
    return result;
}

void Media::setMeta(libvlc_meta_t e_meta, const std::string& psz_value)
{
    libvlc_media_set_meta(get(), e_meta, psz_value.c_str());
}

int Media::saveMeta()
{
    return libvlc_media_save_meta(get());
}

libvlc_state_t Media::state()
{
    return libvlc_media_get_state(get());
}

bool Media::stats(libvlc_media_stats_t * p_stats)
{
    return libvlc_media_get_stats(get(), p_stats) != 0;
}

EventManagerPtr Media::eventManager()
{
    if ( m_eventManager == NULL )
    {
        libvlc_event_manager_t* obj = libvlc_media_event_manager(get());
        m_eventManager = std::make_shared<EventManager>( obj );
    }
    return m_eventManager;
}

libvlc_time_t Media::duration()
{
    return libvlc_media_get_duration(get());
}

void Media::parse()
{
    libvlc_media_parse(get());
}

void Media::parseAsync()
{
    libvlc_media_parse_async(get());
}

bool Media::isParsed()
{
    return libvlc_media_is_parsed(get()) != 0;
}

void Media::setUserData(void * p_new_user_data)
{
    libvlc_media_set_user_data(get(), p_new_user_data);
}

void * Media::userData()
{
    return libvlc_media_get_user_data(get());
}

std::vector<MediaTrack> Media::tracks()
{
    libvlc_media_track_t**  tracks;
    uint32_t                nbTracks = libvlc_media_tracks_get(get(), &tracks);
    std::vector<MediaTrack> res;

    if ( nbTracks == 0 )
        return res;

    for ( uint32_t i = 0; i < nbTracks; ++i )
        res.push_back( MediaTrack(tracks[i]));
    libvlc_media_tracks_release( tracks, nbTracks );
    return res;
}

Media::Media(Internal::InternalPtr ptr, bool incrementRefCount)
    : Internal{ ptr, libvlc_media_release }
{
    if (incrementRefCount)
        retain();
}

void Media::retain()
{
    if ( isValid() )
        libvlc_media_retain(get());
}

} // namespace VLC

