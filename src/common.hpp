/*****************************************************************************
 * libvlc_common.hpp: Stuff
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

#ifndef LIBVLC_CXX_COMMON_H
#define LIBVLC_CXX_COMMON_H

#include <vlc/vlc.h>
#include <memory>

namespace VLC
{
    class Media;
    using MediaPtr = std::shared_ptr<Media>;

    // Work around cross class dependencies
    // Class A needs to access B's internal pointer
    // Class B needs to access A's internal pointer
    // By using a template function to do this, we're delegating
    // the access to both classes' guts to a later point, when the compiler
    // already knows everything it needs.
    // The only drawback is that we can't return decltype(ptr->get()), since when
    // the compiler checks for the prototype, it hasn't parsed all the declarations yet.
    template <typename TYPE, typename REF>
    TYPE* getInternalPtr(const REF& ref)
    {
        return ref.get();
    }

    std::unique_ptr<char, void (*)(void*)> wrapCStr(char* str)
    {
        return std::unique_ptr<char, void(*)(void*)>( str, [](void* ptr) { libvlc_free(ptr); } );
    }
}

#endif
