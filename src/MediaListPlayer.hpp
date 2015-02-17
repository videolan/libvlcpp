/*****************************************************************************
 * libvlc_MediaListPlayer.hpp: MediaListPlayer API
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

#ifndef LIBVLC_CXX_MEDIALISTPLAYER_H
#define LIBVLC_CXX_MEDIALISTPLAYER_H

#include <string>

#include "common.hpp"

namespace VLC
{

class EventManager;
class MediaPlayer;
class MediaList;

class VLCPP_API MediaListPlayer : public Internal<libvlc_media_list_player_t>
{
public:
    /**
     * Check if 2 MediaListPlayer objects contain the same libvlc_media_list_player_t.
     * \param another another MediaListPlayer
     * \return true if they contain the same libvlc_media_list_player_t
     */
    bool operator==(const MediaListPlayer& another) const
    {
        return m_obj == another.m_obj;
    }

    /**
     * Create new media_list_player.
     *
     * \param p_instance  libvlc instance
     */
    MediaListPlayer(Instance& instance)
        : Internal{ libvlc_media_list_player_new( getInternalPtr<libvlc_instance_t>( instance ) ),
                    libvlc_media_list_player_release }
    {
    }


    /**
     * Return the event manager of this media_list_player.
     *
     * \return the event manager
     */
    EventManagerPtr eventManager()
    {
        if ( m_eventManager )
        {
            libvlc_event_manager_t* obj = libvlc_media_list_player_event_manager(*this);
            m_eventManager = std::make_shared<EventManager>( obj );
        }
        return m_eventManager;
    }


    /**
     * Replace media player in media_list_player with this instance.
     *
     * \param p_mi  media player instance
     */
    void setMediaPlayer(const MediaPlayer& mi)
    {
        libvlc_media_list_player_set_media_player( *this,
                        getInternalPtr<libvlc_media_player_t>( mi ) );
    }

    /**
     * Set the media list associated with the player
     *
     * \param p_mlist  list of media
     */
    void setMediaList(const MediaList& mlist)
    {
        libvlc_media_list_player_set_media_list( *this,
                            getInternalPtr<libvlc_media_list_t>( mlist ) );
    }

    /**
     * Play media list
     */
    void play()
    {
        libvlc_media_list_player_play(*this);
    }

    /**
     * Toggle pause (or resume) media list
     */
    void pause()
    {
        libvlc_media_list_player_pause(*this);
    }

    /**
     * Is media list playing?
     *
     * \return true for playing and false for not playing
     */
    bool isPlaying()
    {
        return libvlc_media_list_player_is_playing(*this) != 0;
    }

    /**
     * Get current libvlc_state of media list player
     *
     * \return libvlc_state_t for media list player
     */
    libvlc_state_t state()
    {
        return libvlc_media_list_player_get_state( *this );
    }

    /**
     * Play media list item at position index
     *
     * \param i_index  index in media list to play
     *
     * \return 0 upon success -1 if the item wasn't found
     */
    int playItemAtIndex(int i_index)
    {
        return libvlc_media_list_player_play_item_at_index(*this, i_index);
    }

    /**
     * Play the given media item
     *
     * \param p_md  the media instance
     *
     * \return 0 upon success, -1 if the media is not part of the media list
     */
    int playItem(const Media& md)
    {
        return libvlc_media_list_player_play_item( *this,
                        getInternalPtr<libvlc_media_t>( md ) );
    }

    /**
     * Stop playing media list
     */
    void stop()
    {
        libvlc_media_list_player_stop(*this);
    }

    /**
     * Play next item from media list
     *
     * \return 0 upon success -1 if there is no next item
     */
    int next()
    {
        return libvlc_media_list_player_next(*this);
    }

    /**
     * Play previous item from media list
     *
     * \return 0 upon success -1 if there is no previous item
     */
    int previous()
    {
        return libvlc_media_list_player_previous(*this);
    }

    /**
     * Sets the playback mode for the playlist
     *
     * \param e_mode  playback mode specification
     */
    void setPlaybackMode(libvlc_playback_mode_t e_mode)
    {
        libvlc_media_list_player_set_playback_mode(*this, e_mode);
    }

private:
    EventManagerPtr m_eventManager;

};

} // namespace VLC

#endif

