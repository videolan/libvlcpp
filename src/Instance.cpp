/*****************************************************************************
 * libvlc_Instance.cpp: Instance implementation
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

Instance::Instance(int argc, const char* const* argv)
    : Internal{ libvlc_new( argc, argv ), libvlc_release }
{
}

bool Instance::operator==( const Instance& another ) const
{
    return m_obj == another.m_obj;
}

int Instance::addIntf( const std::string& name )
{
    return libvlc_add_intf( get(), name.c_str() );
}

void Instance::setExitHandler( void(*cb)(void *), void * opaque )
{
    libvlc_set_exit_handler( get(), cb, opaque );
}

void Instance::setUserAgent( const std::string& name, const std::string& http )
{
    libvlc_set_user_agent( get(), name.c_str(), http.c_str() );
}

void Instance::setAppId( const std::string& id, const std::string& version, const std::string& icon )
{
    libvlc_set_app_id( get(), id.c_str(), version.c_str(), icon.c_str() );
}

void Instance::logUnset()
{
    libvlc_log_unset( get() );
}

void Instance::logSet( libvlc_log_cb cb, void * data )
{
    libvlc_log_set(get(), cb, data);
}

void Instance::logSetFile( FILE * stream )
{
    libvlc_log_set_file( get(), stream );
}

std::vector<ModuleDescription> Instance::audioFilterList()
{
    libvlc_module_description_t* result = libvlc_audio_filter_list_get(get());
    std::vector<ModuleDescription> res;
    if ( result == NULL )
        return res;
    libvlc_module_description_t* p = result;
    while ( p != NULL )
    {
        res.push_back( ModuleDescription( p ) );
        p = p->p_next;
    }
    libvlc_module_description_list_release( result );
    return res;
}

std::vector<ModuleDescription> Instance::videoFilterList()
{
    libvlc_module_description_t* result = libvlc_video_filter_list_get(get());
    std::vector<ModuleDescription> res;
    if ( result == NULL )
        return res;
    libvlc_module_description_t* p = result;
    while ( p != NULL )
    {
        res.push_back( ModuleDescription( p ) );
        p = p->p_next;
    }
    libvlc_module_description_list_release( result );
    return res;
}

std::vector<AudioOutputDescription> Instance::audioOutputList()
{
    libvlc_audio_output_t* result = libvlc_audio_output_list_get(get());
    std::vector<AudioOutputDescription> res;
    if ( result == NULL )
        return res;
    libvlc_audio_output_t* p = result;
    while ( p != NULL )
    {
        res.push_back( AudioOutputDescription( p ) );
        p = p->p_next;
    }
    libvlc_audio_output_list_release(result);
    return res;
}

std::vector<AudioOutputDeviceDescription> Instance::audioOutputDeviceList(const std::string& aout)
{
    libvlc_audio_output_device_t* devices = libvlc_audio_output_device_list_get( get(), aout.c_str() );
    std::vector<AudioOutputDeviceDescription> res;
    if ( devices == NULL )
        return res;
    for ( libvlc_audio_output_device_t* p = devices; p != NULL; p = p->p_next )
        res.push_back( AudioOutputDeviceDescription( p ) );
    libvlc_audio_output_device_list_release( devices );
    return res;
}

} // namespace VLC

