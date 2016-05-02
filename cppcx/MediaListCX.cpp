/*****************************************************************************
* MediaListCX.cpp: MediaList API
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

#include "InstanceCX.hpp"
#include "MediaListCX.hpp"
#include "MediaCX.hpp"
#include "MediaDiscovererCX.hpp"

namespace libVLCX
{
    MediaList::MediaList(VLC::MediaList& refNative) : m_list(refNative)
    {
    }

    MediaList::MediaList(VLC::MediaDiscoverer &discoverer) :  m_list(discoverer)
    {
    }

    MediaList::MediaList(Media^ md) : m_list(md->m_media)
    {
    }

    MediaList::MediaList(Instance^ instance) : m_list(instance->m_instance)
    {
    }

    void MediaList::setMedia(Media^ md)
    {
        m_list.setMedia(md->m_media);
    }

    bool MediaList::addMedia(Media^ md)
    {
        return m_list.addMedia(md->m_media);
    }

    bool MediaList::insertMedia(Media^ md, int pos)
    {
        return m_list.insertMedia(md->m_media, pos);
    }

    bool MediaList::removeIndex(int pos)
    {
        return m_list.removeIndex(pos);
    }

    int MediaList::count()
    {
        return m_list.count();
    }

    Media^ MediaList::itemAtIndex(int pos)
    {
        return ref new Media(*m_list.itemAtIndex(pos));
    }

    int MediaList::indexOfItem(Media^ md)
    {
        return m_list.indexOfItem(md->m_media);
    }

    bool MediaList::isReadonly()
    {
        return m_list.isReadonly();
    }

    void MediaList::lock()
    {
        m_list.lock();
    }

    void MediaList::unlock()
    {
        m_list.unlock();
    }

    MediaListEventManager^ MediaList::eventManager()
    {
        if (!m_event_manager)
            m_event_manager = ref new MediaListEventManager(m_list.eventManager());
        return m_event_manager;
    }
}
