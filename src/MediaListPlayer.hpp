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
#include "Internal.hpp"

namespace VLC
{

class EventManager;
class MediaPlayer;
class MediaList;

class VLCPP_API MediaListPlayer : public Internal<libvlc_media_list_player_t>
{
public:
    /**
     * Copy libvlc_media_list_player_t from another to new MediaListPlayer object.
     * \param another existing MediaListPlayer
     */
    MediaListPlayer(const MediaListPlayer& another);

    /**
     * Copy libvlc_media_list_player_t from another MediaListPlayer
     * to this MediaListPlayer
     * \param another existing MediaListPlayer
     */
    const MediaListPlayer& operator=(const MediaListPlayer& another);

    /**
     * Check if 2 MediaListPlayer objects contain the same libvlc_media_list_player_t.
     * \param another another MediaListPlayer
     * \return true if they contain the same libvlc_media_list_player_t
     */
    bool operator==(const MediaListPlayer& another) const;

    ~MediaListPlayer();

    // libvlc_media_list_player_new
    /**
     * Create new media_list_player.
     *
     * \param p_instance  libvlc instance
     */
    static MediaListPlayer create(Instance & instance);

    /**
     * Return the event manager of this media_list_player.
     *
     * \return the event manager
     */
    EventManager& eventManager();

    /**
     * Replace media player in media_list_player with this instance.
     *
     * \param p_mi  media player instance
     */
    void setMediaPlayer(MediaPlayer & p_mi);

    /**
     * Set the media list associated with the player
     *
     * \param p_mlist  list of media
     */
    void setMediaList(MediaList & mlist);

    /**
     * Play media list
     */
    void play();

    /**
     * Toggle pause (or resume) media list
     */
    void pause();

    /**
     * Is media list playing?
     *
     * \return true for playing and false for not playing
     */
    bool isPlaying();

    /**
     * Get current libvlc_state of media list player
     *
     * \return libvlc_state_t for media list player
     */
    libvlc_state_t state();

    /**
     * Play media list item at position index
     *
     * \param i_index  index in media list to play
     *
     * \return 0 upon success -1 if the item wasn't found
     */
    int playItemAtIndex(int i_index);

    /**
     * Play the given media item
     *
     * \param p_md  the media instance
     *
     * \return 0 upon success, -1 if the media is not part of the media list
     */
    int playItem(Media & p_md);

    /**
     * Stop playing media list
     */
    void stop();

    /**
     * Play next item from media list
     *
     * \return 0 upon success -1 if there is no next item
     */
    int next();

    /**
     * Play previous item from media list
     *
     * \return 0 upon success -1 if there is no previous item
     */
    int previous();

    /**
     * Sets the playback mode for the playlist
     *
     * \param e_mode  playback mode specification
     */
    void setPlaybackMode(libvlc_playback_mode_t e_mode);

private:
    explicit MediaListPlayer(InternalPtr ptr);
    /**
     * Release a media_list_player after use Decrement the reference count of
     * a media player object. If the reference count is 0, then
     * MediaListPlayer::release() will release the media player object. If
     * the media player object has been released, then it should not be used
     * again.
     */
    void release();

    /**
     * Retain a reference to a media player list object. Use
     * MediaListPlayer::release() to decrement reference count.
     */
    void retain();

private:
    EventManager* m_eventManager;

};

} // namespace VLC

#endif

