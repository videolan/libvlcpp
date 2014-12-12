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

#ifdef _WIN32
#ifdef libvlcpp_BUILD_DLL
#ifdef libvlcpp_EXPORTS
#   define VLCPP_API __declspec(dllexport)
#else
#   define VLCPP_API __declspec(dllimport)
#endif
#else
# define VLCPP_API
#endif
#else
# define VLCPP_API
#endif

#endif
