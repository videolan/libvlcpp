/*****************************************************************************
 * libvlc_structures.hpp: LibVLC++ structures
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

#ifndef LIBVLC_CXX_STRUCTURES_H
#define LIBVLC_CXX_STRUCTURES_H

#include <string>

#include "common.hpp"

namespace VLC
{

class Instance;
class Media;

class VLCPP_API ModuleDescription
{
public:
    const std::string& name() const;
    const std::string& shortname() const;
    const std::string& longname() const;
    const std::string& help() const;

private:
    explicit ModuleDescription( libvlc_module_description_t* c );
    std::string m_name;
    std::string m_shortname;
    std::string m_longname;
    std::string m_help;

    friend class Instance;
};


class VLCPP_API MediaTrack
{
public:
    enum Type
    {
        Unknown = -1,
        Audio,
        Video,
        Subtitle
    };

    uint32_t codec() const;
    uint32_t originalFourCC() const;
    int32_t id() const;
    Type type() const;
    int32_t profile() const;
    int32_t level() const;
    // Audio specific
    uint32_t channels() const;
    uint32_t rate() const;
    // Video specific
    uint32_t height() const;
    uint32_t width() const;
    uint32_t sarNum() const;
    uint32_t sarDen() const;
    uint32_t fpsNum() const;
    uint32_t fpsDen() const;
    // Subtitles specific
    const std::string& encoding() const;

private:
    explicit MediaTrack(libvlc_media_track_t* c);

    uint32_t m_codec;
    uint32_t m_originalFourcc;
    uint32_t m_id;
    Type m_type;
    int32_t m_profile;
    int32_t m_level;
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


class VLCPP_API AudioOutputDescription
{
public:
    const std::string& name() const;
    const std::string& description() const;

private:
    AudioOutputDescription( libvlc_audio_output_t* c );
    std::string m_name;
    std::string m_description;

    friend class Instance;
};

class VLCPP_API AudioOutputDeviceDescription
{
    public:
        /**< Device identifier string */
        const std::string& device() const;
        /**< User-friendly device description */
        const std::string& description() const;

    private:
        explicit AudioOutputDeviceDescription( libvlc_audio_output_device_t* d );

        std::string m_device;
        std::string m_description;

        friend class Instance;
        friend class MediaPlayer;
};

class VLCPP_API TrackDescription
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
