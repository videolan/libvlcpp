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

#include <vlc.hpp>

namespace VLC
{

MediaListPlayer::MediaListPlayer(const MediaListPlayer& another)
    : Internal( another )
{
    retain();
}

const MediaListPlayer& MediaListPlayer::operator=(const MediaListPlayer& another)
{
    if (this == &another)
    {
        return *this;
    }
    release();
    m_obj = another.m_obj;
    retain();
    return *this;
}

bool MediaListPlayer::operator==(const MediaListPlayer& another) const
{
    return m_obj == another.m_obj;
}

MediaListPlayer::~MediaListPlayer()
{
    release();
}

MediaListPlayer* MediaListPlayer::create(Instance& instance)
{
    InternalPtr ptr = libvlc_media_list_player_new( instance );
    if ( ptr == NULL )
        return NULL;
    return new MediaListPlayer( ptr );
}

libvlc_event_manager_t * MediaListPlayer::eventManager()
{
    return libvlc_media_list_player_event_manager(m_obj);
}

void MediaListPlayer::setMediaPlayer( MediaPlayer &mi )
{
    libvlc_media_list_player_set_media_player( m_obj, mi );
}

void MediaListPlayer::setMediaList( MediaList &mlist )
{
    libvlc_media_list_player_set_media_list( m_obj, mlist );
}

void MediaListPlayer::play()
{
    libvlc_media_list_player_play(m_obj);
}

void MediaListPlayer::pause()
{
    libvlc_media_list_player_pause(m_obj);
}

bool MediaListPlayer::isPlaying()
{
    return libvlc_media_list_player_is_playing(m_obj) != 0;
}

libvlc_state_t MediaListPlayer::state()
{
    return libvlc_media_list_player_get_state( m_obj );
}

int MediaListPlayer::playItemAtIndex(int i_index)
{
    return libvlc_media_list_player_play_item_at_index(m_obj, i_index);
}

int MediaListPlayer::playItem(Media & p_md)
{
    return libvlc_media_list_player_play_item( m_obj, p_md );
}

void MediaListPlayer::stop()
{
    libvlc_media_list_player_stop(m_obj);
}

int MediaListPlayer::next()
{
    return libvlc_media_list_player_next(m_obj);
}

int MediaListPlayer::previous()
{
    return libvlc_media_list_player_previous(m_obj);
}

void MediaListPlayer::setPlaybackMode(libvlc_playback_mode_t e_mode)
{
    libvlc_media_list_player_set_playback_mode(m_obj, e_mode);
}

MediaListPlayer::MediaListPlayer(Internal::InternalPtr ptr)
    : Internal( ptr )
{
}

void MediaListPlayer::release()
{
    libvlc_media_list_player_release(m_obj);
}

void MediaListPlayer::retain()
{
    libvlc_media_list_player_retain(m_obj);
}

} // namespace VLC

