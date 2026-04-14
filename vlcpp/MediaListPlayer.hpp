/*****************************************************************************
 * MediaListPlayer.hpp: MediaListPlayer API
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

#ifndef LIBVLC_CXX_MEDIALISTPLAYER_H
#define LIBVLC_CXX_MEDIALISTPLAYER_H

#include <string>

#include "common.hpp"
#include "MediaPlayer.hpp"

namespace VLC
{

class MediaList;

class MediaListPlayer : public Internal<libvlc_media_list_player_t>
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
    MediaListPlayer(const Instance& instance)
        : Internal{ libvlc_media_list_player_new( getInternalPtr<libvlc_instance_t>( instance ), nullptr, nullptr ),
                    libvlc_media_list_player_release }
    {
    }

    /**
     * Create new media_list_player with callbacks.
     *
     * \param inst libvlc instance
     * \param cbs pre-built \ref MediaPlayer::Callbacks object
     *
     * \warning The application must ensure that the Callbacks object supplied
     * remains valid and unmodified until the media list player is destroyed.
     */
    MediaListPlayer( const Instance& inst, const MediaPlayer::Callbacks& cbs )
    {
        auto ptr = libvlc_media_list_player_new( getInternalPtr<libvlc_instance_t>( inst ),
                                                 &cbs.m_cbs, cbs.m_callbacks.get() );
        if ( ptr == nullptr )
            throw std::runtime_error( "Failed to create media list player" );
        m_obj.reset( ptr, libvlc_media_list_player_release );
    }

    /**
     * Create an empty VLC MediaListPlayer instance.
     *
     * Calling any method on such an instance is undefined.
    */
    MediaListPlayer() = default;

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
     */
    bool playItemAtIndex(int i_index)
    {
        return libvlc_media_list_player_play_item_at_index(*this, i_index) == 0;
    }

    /**
     * Play the given media item
     *
     * \param p_md  the media instance
     */
    bool playItem(const Media& md)
    {
        return libvlc_media_list_player_play_item( *this,
                        getInternalPtr<libvlc_media_t>( md ) ) == 0;
    }

    /**
     * Stop playing media list
     */
    void stopAsync()
    {
        libvlc_media_list_player_stop_async(*this);
    }

    /**
     * Play next item from media list
     */
    bool next()
    {
        return libvlc_media_list_player_next(*this) == 0;
    }

    /**
     * Play previous item from media list
     */
    bool previous()
    {
        return libvlc_media_list_player_previous(*this) == 0;
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
};

} // namespace VLC

#endif

