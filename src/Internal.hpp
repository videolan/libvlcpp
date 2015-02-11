/*****************************************************************************
 * libvlc_Internal.hpp: Handles an internal vlc type.
 *****************************************************************************
 * Copyright (C) 2008-2014 VideoLAN
 *
 * Authors: Hugo Beauzée-Luyssen <hugo@beauzee.fr>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifndef VLCPP_H
#define VLCPP_H

#include <cassert>
#include <stdlib.h>
#include <vlc/libvlc.h>
#include <memory>
#include <stdexcept>

namespace VLC
{

template <typename T, typename Releaser = void(*)(T*)>
class Internal
{
    public:
        using InternalType  = T;
        using InternalPtr   = T*;
        using Pointer       = std::unique_ptr<T, Releaser>;

        InternalPtr get() { return m_obj.get(); }
        bool isValid() const { return (bool)m_obj; }
    protected:
        Internal( InternalPtr obj, Releaser releaser )
            : m_obj{ obj, releaser }
        {
            if ( obj == nullptr )
                throw std::runtime_error("Wrapping a NULL instance");
        }
        Internal(Releaser releaser)
            : m_obj{ nullptr, releaser }
        {
        }

    protected:
        Pointer     m_obj;
};

}

#endif // VLCPP_H
