/*****************************************************************************
 * libvlc_structures.cpp: LibVLC++ structures impl
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

#include <vlc.hpp>

namespace VLC
{

const std::string& ModuleDescription::name() const
{
    return m_name;
}

const std::string& ModuleDescription::shortname() const
{
    return m_shortname;
}

const std::string& ModuleDescription::longname() const
{
    return m_longname;
}

const std::string& ModuleDescription::help() const
{
    return m_help;
}

ModuleDescription::ModuleDescription(libvlc_module_description_t* c)
{
    m_name = c->psz_name ? c->psz_name : "";
    m_shortname = c->psz_shortname ? c->psz_shortname : "";
    m_longname = c->psz_longname ? c->psz_longname : "";
    m_help = c->psz_help ? c->psz_help : "";
}

uint32_t MediaTrack::codec() const
{
    return m_codec;
}

uint32_t MediaTrack::originalFourCC() const
{
    return m_originalFourcc;
}

int32_t MediaTrack::id() const
{
    return m_id;
}

MediaTrack::Type MediaTrack::type() const
{
    return m_type;
}

uint32_t MediaTrack::height() const
{
    return m_height;
}

uint32_t MediaTrack::width() const
{
    return m_width;
}

uint32_t MediaTrack::sarNum() const
{
    return m_sarNum;
}

uint32_t MediaTrack::sarDen() const
{
    return m_sarDen;
}

uint32_t MediaTrack::fpsNum() const
{
    return m_fpsNum;
}

uint32_t MediaTrack::fpsDen() const
{
    return m_fpsDen;
}

const std::string&MediaTrack::encoding() const
{
    return m_encoding;
}

int32_t MediaTrack::profile() const
{
    return m_profile;
}

int32_t MediaTrack::level() const
{
    return m_level;
}

uint32_t MediaTrack::channels() const
{
    return m_channels;
}

uint32_t MediaTrack::rate() const
{
    return m_rate;
}

MediaTrack::MediaTrack( libvlc_media_track_t* c )
    : m_codec( c->i_codec )
    , m_originalFourcc( c->i_original_fourcc )
    , m_id( c->i_id )
    , m_profile( c->i_profile )
    , m_level( c->i_level )
{
    switch ( c->i_type )
    {
        case libvlc_track_audio:
            m_type = Audio;
            m_channels = c->audio->i_channels;
            m_rate = c->audio->i_rate;
            break;
        case libvlc_track_video:
            m_type = Video;
            m_height = c->video->i_height;
            m_width = c->video->i_width;
            m_sarNum = c->video->i_sar_num;
            m_sarDen = c->video->i_sar_den;
            m_fpsNum = c->video->i_frame_rate_num;
            m_fpsDen = c->video->i_frame_rate_den;
            break;
        case libvlc_track_text:
            m_type = Subtitle;
            if ( c->subtitle->psz_encoding != NULL )
                m_encoding = c->subtitle->psz_encoding;
            break;
        case libvlc_track_unknown:
        default:
            m_type = Unknown;
            break;
    }
}

const std::string& AudioOutputDescription::name() const
{
    return m_name;
}

const std::string& AudioOutputDescription::description() const
{
    return m_description;
}

AudioOutputDescription::AudioOutputDescription( libvlc_audio_output_t* c )
{
    m_name = c->psz_name ? c->psz_name : "";
    m_description = c->psz_description ? c->psz_description : "";
}

int TrackDescription::id() const
{
    return m_id;
}

const std::string& TrackDescription::name() const
{
    return m_name;
}

TrackDescription::TrackDescription(libvlc_track_description_t* c)
{
    m_id = c->i_id;
    m_name = c->psz_name ? c->psz_name : "";
}

} // namespace VLC
