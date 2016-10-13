/*****************************************************************************
* MediaCX.hpp: Media API
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

#include "structuresCX.hpp"
#include <collection.h>
#include <vlcpp/vlc.hpp>

#include <ppltasks.h>
using namespace Windows::Foundation;

namespace libVLCX
{
    ref class MediaEventManager;
    ref class Instance;
    ref class MediaList;

    public enum class MediaState
    {
        NothingSpecial = 0,
        Opening,
        Buffering,
        Playing,
        Paused,
        Stopped,
        Ended,
        Error
    };

    public enum class MediaMeta
    {
        Title,
        Artist,
        Genre,
        Copyright,
        Album,
        TrackNumber,
        Description,
        Rating,
        Date,
        Setting,
        URL,
        Language,
        NowPlaying,
        Publisher,
        EncodedBy,
        ArtworkURL,
        TrackID,
        TrackTotal,
        Director,
        Season,
        Episode,
        ShowName,
        Actors,
        AlbumArtist,
        DiscNumber
    };

    public enum class ParseFlags
    {
        /**
        * Parse media if it's a local file
        */
        Local = 0x00,
        /**
        * Parse media even if it's a network file
        */
        Network = 0x01,
        /**
        * Fetch meta and covert art using local resources
        */
        FetchLocal = 0x02,
        /**
        * Fetch meta and covert art using network resources
        */
        FetchNetwork = 0x04,
        /**
        * Interact with the user (via libvlc_dialog_cbs) when preparsing this item
        * (and not its sub items). Set this flag in order to receive a callback
        * when the input is asking for credentials.
        */
        Interact = 0x08,
    };

	public enum class ParsedStatus
	{
		Skipped = VLC::Media::ParsedStatus::Skipped,
		Failed = VLC::Media::ParsedStatus::Failed,
		Done = VLC::Media::ParsedStatus::Done,
        Timeout = VLC::Media::ParsedStatus::Timeout,
	};

    public enum class FromType
    {
        FromPath,
        FromLocation,
        AsNode,
    };

    public enum class MediaType
    {
        Unknown = VLC::Media::Type::Unknown,
        File = VLC::Media::Type::File,
        Directory = VLC::Media::Type::Directory,
        Disc = VLC::Media::Type::Disc,
        Stream = VLC::Media::Type::Stream,
        Playlist = VLC::Media::Type::Playlist,
    };

    public ref class Media sealed
    {
    public:
        Media(Instance^ instance, Platform::String^ location, FromType from_type);

        /**
        * Add an option to the media.
        *
        * This option will be used to determine how the media_player will read
        * the media. This allows to use VLC's advanced reading/streaming options
        * on a per-media basis.
        *
        * \note The options are listed in 'vlc long-help' from the command line,
        * e.g. "-sout-all". Keep in mind that available options and their
        * semantics vary across LibVLC versions and builds.
        *
        * \warning Not all options affects libvlc_media_t objects: Specifically,
        * due to architectural issues most audio and video options, such as text
        * renderer options, have no effects on an individual media. These
        * options must be set through Instance::Instance() instead.
        *
        * \param psz_options  the options (as a string)
        */
        void addOption(Platform::String^ psz_options);

        /**
        * Add an option to the media with configurable flags.
        *
        * This option will be used to determine how the media_player will read
        * the media. This allows to use VLC's advanced reading/streaming options
        * on a per-media basis.
        *
        * The options are detailed in vlc long-help, for instance "--sout-all".
        * Note that all options are not usable on medias: specifically, due to
        * architectural issues, video-related options such as text renderer
        * options cannot be set on a single media. They must be set on the whole
        * libvlc instance instead.
        *
        * \param psz_options  the options (as a string)
        *
        * \param i_flags  the flags for this option
        */
        void addOptionFlag(Platform::String^ psz_options, unsigned i_flags);

        /**
        * Get the media resource locator (mrl) from a media descriptor object
        *
        * \return string with mrl of media descriptor object
        */
        Platform::String^ mrl();

        /**
        * Duplicate a media descriptor object.
        */
        Media^ duplicate();

        /**
        * Read the meta of the media.
        *
        * If the media has not yet been parsed this will return NULL.
        *
        * This methods automatically calls Media::parseAsync() , so after
        * calling it you may receive a libvlc_MediaMetaChanged event. If you
        * prefer a synchronous version ensure that you call Media::parse()
        * before get_meta().
        *
        * \see Media::parse()
        *
        * \see Media::parseAsync()
        *
        * \see libvlc_MediaMetaChanged
        *
        * \param e_meta  the meta to read
        *
        * \return the media's meta
        */
        Platform::String^ meta(MediaMeta e_meta);

        /**
        * Set the meta of the media (this function will not save the meta, call
        * libvlc_media_save_meta in order to save the meta)
        *
        * \param e_meta  the meta to write
        *
        * \param psz_value  the media's meta
        */
        void setMeta(MediaMeta e_meta, Platform::String^ psz_value);

        /**
        * Save the meta previously set
        *
        * \return true if the write operation was successful
        */
        int saveMeta();

        /**
        * Get current state of media descriptor object. Possible media states
        * are defined in libvlc_structures.c ( libvlc_NothingSpecial=0,
        * libvlc_Opening, libvlc_Buffering, libvlc_Playing, libvlc_Paused,
        * libvlc_Stopped, libvlc_Ended, libvlc_Error).
        *
        * \see libvlc_state_t
        *
        * \return state of media descriptor object
        */
        MediaState state();

        /**
        * Get the current statistics about the media
        *
        * \param p_stats  structure that contain the statistics about the media
        * (this structure must be allocated by the caller)
        *
        * \return true if the statistics are available, false otherwise
        */
        //bool stats(libvlc_media_stats_t * p_stats);

        /**
        * Get event manager from media descriptor object. NOTE: this function
        * doesn't increment reference counting.
        *
        * \return event manager object
        */
        MediaEventManager^ eventManager();

        /**
        * Get duration (in ms) of media descriptor object item.
        *
        * \return duration of media item or -1 on error
        */
        libvlc_time_t duration();

        /**
        * Parse a media.
        *
        * This fetches (local) meta data and tracks information. The method is
        * the asynchronous of Media::parse() .
        *
        * To track when this is over you can listen to libvlc_MediaParsedChanged
        * event. However if the media was already parsed you will not receive
        * this event.
        *
        * \see Media::parse()
        *
        * \see libvlc_MediaParsedChanged
        *
        * \see Media::meta()
        *
        * \see Media::tracks()
        */
        void Media::parseWithOptions(ParseFlags flags, int timeoutMs);

        IAsyncOperation<ParsedStatus>^ parseWithOptionsAsync(ParseFlags flags, int timeoutMs);

        ParsedStatus parsedStatus();

        /**
        * Get media descriptor's elementary streams description
        *
        * Note, you need to call Media::parse() or play the media at least once
        * before calling this function. Not doing this will result in an empty
        * array.
        *
        * \version LibVLC 2.1.0 and later.
        *
        * \return a vector containing all tracks
        */
        Windows::Foundation::Collections::IVector<MediaTrack^>^ tracks();

        MediaList^ subItems();

        MediaType type();

        /**
         * Add a slave to the current media.
         *
         * A slave is an external input source that may contains an additional subtitle
         * track (like a .srt) or an additional audio track (like a .ac3).
         *
         * \note This function must be called before the media is parsed (via parseWithOptions())
         *  or before the media is played (via MediaPlayer::play())
         *
         * \version LibVLC 3.0.0 and later.
         *
         * \param uri Uri of the slave (should contain a valid scheme).
         * \param type subtitle or audio
         * \param priority from 0 (low priority) to 4 (high priority)
         *
         * \return true on success, false on error.
         */
        bool addSlave(SlaveType type, unsigned priority, Platform::String^ uri);

        /**
         * Clear all slaves previously added by addSlave() or
         * internally.
         *
         * \version LibVLC 3.0.0 and later.
         */
        void slavesClear();

        /**
         * Get a media descriptor's slaves in a vector
         *
         * The list will contain slaves parsed by VLC or previously added by
         * addSlave(). The typical use case of this function is to save
         * a list of slave in a database for a later use.
         *
         * \version LibVLC 3.0.0 and later.
         *
         * \see addSlave()
         *
         * \return a vector of MediaSlave
         */
        Windows::Foundation::Collections::IVector<MediaSlave^>^ slaves();

    private:
        ~Media(){}
    
    internal:
        Media(const VLC::Media& media);
        Media(VLC::Media&& media);
        VLC::Media m_media;

    private:
        MediaEventManager^ m_eventManager;
    };

} // namespace libVLCX

