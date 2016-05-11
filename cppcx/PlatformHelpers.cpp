/*****************************************************************************
* PlatformHelpers.cpp
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

#include "Helpers.h"

char *FromPlatformString(Platform::String^ str)
{
    size_t len = WideCharToMultiByte(CP_UTF8, 0, str->Data(), -1, NULL, 0, NULL, NULL);
    if(len == 0)
        return NULL;
    char* psz_str = new char[len];
    WideCharToMultiByte(CP_UTF8, 0, str->Data(), -1, psz_str, len, NULL, NULL);
    return psz_str;
}

Platform::String^
ToPlatformString(const char *str) {
    if (str == NULL)
        return ref new Platform::String();
    size_t len = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    if (len == 0)
        return nullptr;
    std::unique_ptr<wchar_t[]> w_str(new wchar_t[len]);
    MultiByteToWideChar(CP_UTF8, 0, str, -1, w_str.get(), len);
    return ref new Platform::String(w_str.get());
}

Platform::String^
ToPlatformString(const std::string& str)
{
    return ToPlatformString(str.c_str() == NULL ? "" : str.c_str());
}
