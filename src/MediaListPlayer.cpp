/*****************************************************************************
 * libvlc_MediaListPlayer.cpp: MediaListPlayer implementation
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
#include <memory>

namespace VLC
{

bool MediaListPlayer::operator==(const MediaListPlayer& another) const
{
    return m_obj == another.m_obj;
}

MediaListPlayer::MediaListPlayer( InstancePtr instance )
    : Internal{ libvlc_media_list_player_new( instance->get() ), libvlc_media_list_player_release }
{
}

EventManagerPtr MediaListPlayer::eventManager()
{
    if ( m_eventManager )
    {
        libvlc_event_manager_t* obj = libvlc_media_list_player_event_manager(get());
        m_eventManager = std::make_shared<EventManager>( obj );
    }
    return m_eventManager;
}

void MediaListPlayer::setMediaPlayer( MediaPlayerPtr mi )
{
    libvlc_media_list_player_set_media_player( get(), mi->get() );
}

void MediaListPlayer::setMediaList( MediaListPtr mlist )
{
    libvlc_media_list_player_set_media_list( get(), mlist->get() );
}

void MediaListPlayer::play()
{
    libvlc_media_list_player_play(get());
}

void MediaListPlayer::pause()
{
    libvlc_media_list_player_pause(get());
}

bool MediaListPlayer::isPlaying()
{
    return libvlc_media_list_player_is_playing(get()) != 0;
}

libvlc_state_t MediaListPlayer::state()
{
    return libvlc_media_list_player_get_state( get() );
}

int MediaListPlayer::playItemAtIndex(int i_index)
{
    return libvlc_media_list_player_play_item_at_index(get(), i_index);
}

int MediaListPlayer::playItem(MediaPtr p_md)
{
    return libvlc_media_list_player_play_item( get(), p_md->get() );
}

void MediaListPlayer::stop()
{
    libvlc_media_list_player_stop(get());
}

int MediaListPlayer::next()
{
    return libvlc_media_list_player_next(get());
}

int MediaListPlayer::previous()
{
    return libvlc_media_list_player_previous(get());
}

void MediaListPlayer::setPlaybackMode(libvlc_playback_mode_t e_mode)
{
    libvlc_media_list_player_set_playback_mode(get(), e_mode);
}

MediaListPlayer::MediaListPlayer( InternalPtr ptr )
    : Internal{ ptr, libvlc_media_list_player_release }
{
}

} // namespace VLC

