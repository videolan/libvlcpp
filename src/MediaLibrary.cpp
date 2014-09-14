/*****************************************************************************
 * libvlc_MediaLibrary.cpp: MediaLibrary implementation
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

namespace VLC
{

MediaLibrary MediaLibrary::create(Instance& instance)
{
    InternalPtr ptr = libvlc_media_library_new( instance );
    return MediaLibrary(ptr);
}

MediaLibrary::MediaLibrary(const MediaLibrary& another)
    : Internal( another )
{
    retain();
}

const MediaLibrary& MediaLibrary::operator=(const MediaLibrary& another)
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

bool MediaLibrary::operator==(const MediaLibrary& another) const
{
    return m_obj == another.m_obj;
}

MediaLibrary::~MediaLibrary()
{
    release();
}

int MediaLibrary::load()
{
    return libvlc_media_library_load(m_obj);
}

MediaLibrary::MediaLibrary(Internal::InternalPtr ptr)
    : Internal( ptr )
{
}

void MediaLibrary::release()
{
    libvlc_media_library_release(m_obj);
}

void MediaLibrary::retain()
{
    libvlc_media_library_retain(m_obj);
}

} // namespace VLC

