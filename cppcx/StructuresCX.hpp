/*****************************************************************************
* StructuresCX.hpp: LibVLC++ structures
*****************************************************************************
* Copyright © 2014 the VideoLAN team
*
* Authors: Hugo Beauzée-Luyssen <hugo@beauzee.fr>
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

#include <vlcpp/vlc.hpp>
#include "Helpers.h"

namespace libVLCX
{
    public ref class ModuleDescription sealed
    {
    public:
        Platform::String^ name();
        Platform::String^ shortname();
        Platform::String^ longname();
        Platform::String^ help();

    internal:
        explicit ModuleDescription(const VLC::ModuleDescription& desc);

    private:
        Platform::String^ m_name;
        Platform::String^ m_shortname;
        Platform::String^ m_longname;
        Platform::String^ m_help;
    };


    public enum class TrackType
    {
        Unknown = -1,
        Audio,
        Video,
        Subtitle
    };

    public ref class MediaTrack sealed
    {
    public:
        uint32_t codec();
        uint32_t originalFourCC();
        int32_t id();
        TrackType type();
        int32_t profile();
        int32_t level();
        uint32_t bitrate();
        Platform::String^ language();
        Platform::String^ description();
        // Audio specific
        uint32_t channels();
        uint32_t rate();
        // Video specific
        uint32_t height();
        uint32_t width();
        uint32_t sarNum();
        uint32_t sarDen();
        uint32_t fpsNum();
        uint32_t fpsDen();
        // Subtitles specific
        Platform::String^ encoding();

    internal:
        explicit MediaTrack(const VLC::MediaTrack& track);

    private:
        uint32_t m_codec;
        uint32_t m_originalFourcc;
        uint32_t m_id;
        TrackType m_type;
        int32_t m_profile;
        int32_t m_level;
        uint32_t m_bitrate;
        Platform::String^ m_language;
        Platform::String^ m_description;
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
        Platform::String^ m_encoding;
    };


    public ref class AudioOutputDescription sealed
    {
    public:
        Platform::String^ name();
        Platform::String^ description();

    internal:
        AudioOutputDescription(const VLC::AudioOutputDescription& desc);

    private:
        Platform::String^ m_name;
        Platform::String^ m_description;
    };

    public ref class AudioOutputDeviceDescription sealed
    {
    public:
        /**< Device identifier string */
        Platform::String^ device();
        /**< User-friendly device description */
        Platform::String^ description();

    internal:
        explicit AudioOutputDeviceDescription(const VLC::AudioOutputDeviceDescription& desc);

    private:
        Platform::String^ m_device;
        Platform::String^ m_description;
    };

    public ref class TrackDescription sealed
    {
    public:
        int id();
        Platform::String^ name();

    internal:
        explicit TrackDescription(const VLC::TrackDescription& desc);

    private:
        int m_id;
        Platform::String^ m_name;
    };




    public ref class TitleDescription sealed
    {
        public:
            int64_t duration();
            int id();
            Platform::String^ name();
            bool isMenu();

        internal:
            explicit TitleDescription(const VLC::TitleDescription& desc);

        private:
            int m_id;
            Platform::String^ m_name;
            int64_t m_duration;
            bool m_menu;
    };

    public ref class ChapterDescription sealed
    {
        public:
            int id();
            int64_t startTime();
            int64_t duration();
            Platform::String^ name();

        internal:
            explicit ChapterDescription(const VLC::ChapterDescription& desc);

        private:
            int m_id;
            Platform::String^ m_name;
            int64_t m_duration;
            int64_t m_startTime;
    };

    public ref class MediaSlave sealed
    {
    private:
        VLC::MediaSlave m_obj;
        Platform::String ^m_uri;

    internal:
        explicit MediaSlave(const VLC::MediaSlave& slave);

    public:

        public enum class Type
        {
            Subtitle = VLC::MediaSlave::Subtitle,
            Audio = VLC::MediaSlave::Audio
        }

        Type type() const;

        unsigned priority() const;

        Platform::String^ uri() const;
    };
}
