/*****************************************************************************
* PlatformHelpers.hpp
*****************************************************************************
* Copyright © 2014 the VideoLAN team
*
* Authors: Jean-Baptiste Kempf <jb@videolan.org>
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

#pragma once

#include <Windows.h>

#include <collection.h>
#include <string>
#include <vector>

char *FromPlatformString(Platform::String^ str);

Platform::String^ ToPlatformString(const char *str);
Platform::String^ ToPlatformString(const std::string& str);

template <typename MANAGED, typename NATIVE>
Platform::Collections::Vector<MANAGED^>^ MarshallVector(const std::vector<NATIVE>& input)
{
    auto res = ref new Platform::Collections::Vector<MANAGED^>();
    for (auto& d : input)
        res->Append(ref new MANAGED(d));
    return res;
}

struct VLCString
{
    VLCString(Platform::String^ string)
        : m_pszstring(FromPlatformString(string))
        , m_string(m_pszstring)
    {
    }
    ~VLCString()
    {
        delete[] m_pszstring;
    }
    operator const char*() const { return m_pszstring; }
    operator const std::string&() const { return m_string; }

private:
    const char* m_pszstring;
    const std::string m_string;
};