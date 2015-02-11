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

#include <stdexcept>
#include <memory>

namespace VLC
{

bool MediaList::operator==(const MediaList& another) const
{
    return m_obj == another.m_obj;
}

MediaList::MediaList(MediaPtr md)
    : Internal{ libvlc_media_subitems( md->get() ), libvlc_media_list_release }
{
}

MediaList::MediaList(MediaDiscovererPtr mdis )
    : Internal{ libvlc_media_discoverer_media_list( mdis->get() ), libvlc_media_list_release }
{
}

MediaList::MediaList(MediaLibraryPtr mlib)
    : Internal{ libvlc_media_library_media_list( mlib->get() ), libvlc_media_list_release }
{
}

MediaList::MediaList( InstancePtr instance )
    : Internal{ libvlc_media_list_new( instance->get() ), libvlc_media_list_release }
{
}

void MediaList::setMedia( MediaPtr md )
{
    libvlc_media_list_set_media( get(), md->get() );
}

int MediaList::addMedia(MediaPtr p_md)
{
    return libvlc_media_list_add_media( get(), p_md->get() );
}

int MediaList::insertMedia( MediaPtr md, int pos )
{
    return libvlc_media_list_insert_media( get(), md->get(), pos );
}

int MediaList::removeIndex(int i_pos)
{
    return libvlc_media_list_remove_index(get(), i_pos);
}

int MediaList::count()
{
    return libvlc_media_list_count(get());
}

Media MediaList::itemAtIndex(int i_pos)
{
    Media::InternalPtr ptr = libvlc_media_list_item_at_index(get(), i_pos);
    return Media( ptr, false );
}

int MediaList::indexOfItem( MediaPtr md )
{
    return libvlc_media_list_index_of_item( get(), md->get() );
}

bool MediaList::isReadonly()
{
    return libvlc_media_list_is_readonly(get());
}

void MediaList::lock()
{
    libvlc_media_list_lock(get());
}

void MediaList::unlock()
{
    libvlc_media_list_unlock(get());
}

EventManagerPtr MediaList::eventManager()
{
    if ( m_eventManager )
    {
        libvlc_event_manager_t* obj = libvlc_media_list_event_manager( get() );
        m_eventManager = std::make_shared<EventManager>( obj );
    }
    return m_eventManager;
}

} // namespace VLC

