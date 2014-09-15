/*****************************************************************************
 * libvlc_MediaList.cpp: MediaList implementation
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

#include "EventManager.hpp"

namespace VLC
{

MediaList::MediaList(const MediaList& another)
    : Internal( another )
{
    if ( another.m_eventManager )
        m_eventManager = new EventManager( *another.m_eventManager );
    retain();
}

MediaList& MediaList::operator=(const MediaList& another)
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

bool MediaList::operator==(const MediaList& another) const
{
    return m_obj == another.m_obj;
}

MediaList::~MediaList()
{
    delete m_eventManager;
    release();
}

MediaList MediaList::fromMedia(Media& md)
{
    InternalPtr ptr = libvlc_media_subitems( md );
    return MediaList( ptr );
}

MediaList MediaList::fromMediaDiscoverer( MediaDiscoverer& mdis )
{
    InternalPtr ptr = libvlc_media_discoverer_media_list( mdis );
    return MediaList( ptr );
}

MediaList MediaList::fromMediaLibrary(MediaLibrary& mlib)
{
    InternalPtr ptr = libvlc_media_library_media_list( mlib );
    return MediaList( ptr );
}

MediaList MediaList::create( Instance& instance )
{
    InternalPtr ptr = libvlc_media_list_new( instance );
    return MediaList( ptr );
}

void MediaList::setMedia( Media &md )
{
    libvlc_media_list_set_media( m_obj, md );
}

int MediaList::addMedia(Media& p_md)
{
    return libvlc_media_list_add_media( m_obj, p_md );
}

int MediaList::insertMedia( Media& md, int pos )
{
    return libvlc_media_list_insert_media( m_obj, md, pos );
}

int MediaList::removeIndex(int i_pos)
{
    return libvlc_media_list_remove_index(m_obj, i_pos);
}

int MediaList::count()
{
    return libvlc_media_list_count(m_obj);
}

Media MediaList::itemAtIndex(int i_pos)
{
    Media::InternalPtr ptr = libvlc_media_list_item_at_index(m_obj, i_pos);
    return Media( ptr );
}

int MediaList::indexOfItem( Media &md )
{
    return libvlc_media_list_index_of_item( m_obj, md );
}

bool MediaList::isReadonly()
{
    return libvlc_media_list_is_readonly(m_obj);
}

void MediaList::lock()
{
    libvlc_media_list_lock(m_obj);
}

void MediaList::unlock()
{
    libvlc_media_list_unlock(m_obj);
}

EventManager& MediaList::eventManager()
{
    if ( m_eventManager == NULL )
    {
        libvlc_event_manager_t* obj = libvlc_media_list_event_manager( *this );
        m_eventManager = new EventManager( obj );
    }
    return *m_eventManager;
}

MediaList::MediaList( Internal::InternalPtr ptr )
    : Internal( ptr )
    , m_eventManager( NULL )
{
}

void MediaList::release()
{
    libvlc_media_list_release(m_obj);
}

void MediaList::retain()
{
    libvlc_media_list_retain(m_obj);
}

} // namespace VLC

