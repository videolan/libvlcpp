/*****************************************************************************
 * structures.hpp: LibVLC++ structures
 *****************************************************************************
 * Copyright © 2015 libvlcpp authors & VideoLAN
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

#ifndef LIBVLC_CXX_STRUCTURES_H
#define LIBVLC_CXX_STRUCTURES_H

#include <string>

#include "common.hpp"

namespace VLC
{

class Instance;
class Media;

class ModuleDescription
{
public:
    const std::string& name() const
    {
        return m_name;
    }

    const std::string& shortname() const
    {
        return m_shortname;
    }

    const std::string& longname() const
    {
        return m_longname;
    }

    const std::string& help() const
    {
        return m_help;
    }

private:
    explicit ModuleDescription( libvlc_module_description_t* c )
    {
        if ( c->psz_name != NULL )
            m_name = c->psz_name;
        if ( c->psz_shortname != NULL )
            m_shortname = c->psz_shortname;
        if ( c->psz_longname != NULL )
            m_longname = c->psz_longname;
        if ( c->psz_help != NULL )
            m_help = c->psz_help;
    }

    std::string m_name;
    std::string m_shortname;
    std::string m_longname;
    std::string m_help;

    friend class Instance;
};


class MediaTrack
{
public:
    enum Type
    {
        Unknown = -1,
        Audio,
        Video,
        Subtitle
    };

    uint32_t codec() const
    {
        return m_codec;
    }

    uint32_t originalFourCC() const
    {
        return m_originalFourcc;
    }

    int32_t id() const
    {
        return m_id;
    }

    Type type() const
    {
        return m_type;
    }

    int32_t profile() const
    {
        return m_profile;
    }

    int32_t level() const
    {
        return m_level;
    }

    uint32_t bitrate() const
    {
        return m_bitrate;
    }

    const std::string& language() const
    {
        return m_language;
    }

    const std::string& description() const
    {
        return m_description;
    }

    // Audio specific
    uint32_t channels() const
    {
        return m_channels;
    }

    uint32_t rate() const
    {
        return m_rate;
    }

    // Video specific
    uint32_t height() const
    {
        return m_height;
    }

    uint32_t width() const
    {
        return m_width;
    }

    uint32_t sarNum() const
    {
        return m_sarNum;
    }

    uint32_t sarDen() const
    {
        return m_sarDen;
    }

    uint32_t fpsNum() const
    {
        return m_fpsNum;
    }

    uint32_t fpsDen() const
    {
        return m_fpsDen;
    }

    // Subtitles specific
    const std::string& encoding() const
    {
        return m_encoding;
    }

private:
    explicit MediaTrack(libvlc_media_track_t* c)
        : m_codec( c->i_codec )
        , m_originalFourcc( c->i_original_fourcc )
        , m_id( c->i_id )
        , m_profile( c->i_profile )
        , m_level( c->i_level )
        , m_bitrate( c->i_bitrate )
    {
        if ( c->psz_language != NULL )
            m_language = c->psz_language;
        if ( c->psz_description != NULL )
            m_description = c->psz_description;
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

    uint32_t m_codec;
    uint32_t m_originalFourcc;
    uint32_t m_id;
    Type m_type;
    int32_t m_profile;
    int32_t m_level;
    uint32_t m_bitrate;
    std::string m_language;
    std::string m_description;
    // Audio
    uint32_t m_channels;
    uint32_t m_rate;
    // Video
    uint32_t m_height;
    uint32_t m_width;
    uint32_t m_sarNum;
    uint32_t m_sarDen;
    uint32_t m_fpsNum;
    uint32_t m_fpsDen;
    // Subtitles
    std::string m_encoding;

    friend class Media;
};


class AudioOutputDescription
{
public:
    const std::string& name() const
    {
        return m_name;
    }

    const std::string& description() const
    {
        return m_description;
    }

private:
    AudioOutputDescription( libvlc_audio_output_t* c )
    {
        if ( c->psz_name != NULL )
            m_name = c->psz_name;
        if ( c->psz_description != NULL )
            m_description = c->psz_description;
    }

    std::string m_name;
    std::string m_description;

    friend class Instance;
};

class AudioOutputDeviceDescription
{
    public:
        /**< Device identifier string */
        const std::string& device() const
        {
            return m_device;
        }

        /**< User-friendly device description */
        const std::string& description() const
        {
            return m_description;
        }

    private:
        explicit AudioOutputDeviceDescription( libvlc_audio_output_device_t* d )
        {
            if ( d->psz_device != NULL )
                m_device = d->psz_device;
            if ( d->psz_description != NULL )
                m_device = d->psz_description;
        }

        std::string m_device;
        std::string m_description;

        friend class Instance;
        friend class MediaPlayer;
};

class TrackDescription
{
public:
    int id() const;
    const std::string& name() const;

private:
    explicit TrackDescription( libvlc_track_description_t* c );
    int m_id;
    std::string m_name;

    friend class MediaPlayer;
};

} // namespace VLC
#endif
