/*****************************************************************************
* MediaListCX.hpp: MediaList API
*****************************************************************************
* Copyright © 2015 libvlcpp authors & VideoLAN
*
* Authors: Alexey Sokolov <alexey+vlc@asokolov.org>
*          Hugo Beauzée-Luyssen <hugo@beauzee.fr>
*          Bastien Penavayre <bastienPenava@gmail.com>
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

#pragma once

#include "EventManagerCX.hpp"
#include "vlcpp\MediaList.hpp"

namespace libVLCX
{
    ref class Instance;

    public ref class MediaList sealed
    {
    private:
        MediaListEventManager^ m_event_manager;

    internal:
        VLC::MediaList m_list;

        /**
        * Wrapper default constructor
        *
        * \param nativeInstance reference to the libvlcpp of MediaList
        */
        MediaList(VLC::MediaList &nativeInstance);

        /**
        * Get media service discover media list.
        *
        * \param p_mdis  media service discover object
        */
        MediaList(VLC::MediaDiscoverer &);

    public:
        /**
        * Get subitems of media descriptor object.
        */
        MediaList(Media^ md);

        /**
        * Create an empty media list.
        *
        * \param p_instance  libvlc instance
        */
        MediaList(Instance^ instance);

        /**
        * Associate media instance with this media list instance. If another
        * media instance was present it will be released. The
        * MediaList lock should NOT be held upon entering this function.
        *
        * \param md  media instance to add
        */
        void setMedia(Media^ md);

        /**
        * Add media instance to media list The MediaList lock should be
        * held upon entering this function.
        *
        * \param md  a media instance
        */
        bool addMedia(Media^ md);

        /**
        * Insert media instance in media list on a position The
        * MediaList lock should be held upon entering this function.
        *
        * \param md  a media instance
        *
        * \param pos  position in array where to insert
        */
        bool insertMedia(Media^ md, int pos);

        /**
        * Remove media instance from media list on a position The
        * MediaList lock should be held upon entering this function.
        *
        * \param pos  position in array where to insert
        */
        bool removeIndex(int pos);

        /**
        * Get count on media list items The MediaList lock should be
        * held upon entering this function.
        *
        * \return number of items in media list
        */
        int count();

        /**
        * List media instance in media list at a position The
        * MediaList lock should be held upon entering this function.
        *
        * \param pos  position in array where to insert
        *
        * \return media instance at position pos, or nullptr if not found.
        */
        Media^ itemAtIndex(int pos);

        /**
        * Find index position of List media instance in media list. Warning: the
        * function will return the first matched position. The
        * MediaList lock should be held upon entering this function.
        *
        * \param md  media instance
        *
        * \return position of media instance or -1 if media not found
        */
        int indexOfItem(Media^ md);

        /**
        * This indicates if this media list is read-only from a user point of
        * view
        *
        * \return true if readonly, false otherwise
        */
        bool isReadonly();

        /**
        * Get lock on media list items
        */
        void lock();

        /**
        * Release lock on media list items The MediaList lock should be
        * held upon entering this function.
        */
        void unlock();

        /**
        * Get MediaListEventManager from this media list instance. The
        * p_event_manager is immutable, so you don't have to hold the lock
        *
        * \return libvlc_event_manager
        */
        MediaListEventManager^ eventManager();
    };
}
