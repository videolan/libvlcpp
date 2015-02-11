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

#include <stdexcept>

namespace VLC
{

MediaLibrary::MediaLibrary(InstancePtr instance)
    : Internal{ libvlc_media_library_new( instance->get() ), libvlc_media_library_release }
{
}

bool MediaLibrary::operator==(const MediaLibrary& another) const
{
    return m_obj == another.m_obj;
}

int MediaLibrary::load()
{
    return libvlc_media_library_load(get());
}

} // namespace VLC

