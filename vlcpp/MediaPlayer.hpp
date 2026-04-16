/*****************************************************************************
 * MediaPlayer.hpp: MediaPlayer API
 *****************************************************************************
 * Copyright © 2015 libvlcpp authors & VideoLAN
 *
 * Authors: Alexey Sokolov <alexey+vlc@asokolov.org>
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

#ifndef LIBVLC_CXX_MEDIAPLAYER_H
#define LIBVLC_CXX_MEDIAPLAYER_H

#include <array>
#include <string>
#include <vector>
#include <memory>

#include "common.hpp"
#include "Media.hpp"
#include "RendererDiscoverer.hpp"

namespace VLC
{

class AudioOutputDeviceDescription;
class Equalizer;
class Instance;
class Media;
class TrackDescription;
class TrackList;

///
/// \brief The MediaPlayer class exposes libvlc_media_player_t functionnalities
///
class MediaPlayer : private CallbackOwner<13>, public Internal<libvlc_media_player_t>
{
private:
    enum class CallbackIdx : unsigned int
    {
        AudioPlay,
        AudioPause,
        AudioResume,
        AudioFlush,
        AudioDrain,
        AudioVolume,
        AudioSetup,
        AudioCleanup,

        VideoLock,
        VideoUnlock,
        VideoDisplay,
        VideoFormat,
        VideoCleanup,
    };
public:
    enum class DeinterlaceState : signed char
    {
        Auto     = -1,
        Disabled =  0,
        Enabled  =  1
    };

    enum class Capability : unsigned int
    {
        Seek       = libvlc_capability_seek,
        Pause      = libvlc_capability_pause,
        ChangeRate = libvlc_capability_change_rate,
        Rewind     = libvlc_capability_rewind,
    };

    enum class LibvlcState : unsigned int
    {
        NothingSpecial = libvlc_NothingSpecial,
        Opening        = libvlc_Opening,
        Playing        = libvlc_Playing,
        Paused         = libvlc_Paused,
        Stopped        = libvlc_Stopped,
        Stopping       = libvlc_Stopping,
        Error          = libvlc_Error
    };

    enum class MediaStoppingReason : unsigned int
    {
        Error = libvlc_stopping_reason_error,
        Eos = libvlc_stopping_reason_eos,
        User = libvlc_stopping_reason_user,
    };

    enum class ListAction : unsigned int
    {
        Added = libvlc_list_action_added,
        Removed = libvlc_list_action_removed,
        Updated = libvlc_list_action_updated,
    };

    /**
     * Callback prototype that notify when the player changed media
     *
     * \param media new played media
     */
    using ExpectedMediaChangedCb = void(Media&&);

    /**
     * Callback prototype that notify when the player will stop the current
     * media.
     *
     * This can be called from the PLAYING state, before the
     * player requests the next media, or from the STOPPING state, ie.
     * when the player is stopping, or by an internal transition
     * (e.g., when the media reaches the end of file or errors out).
     *
     * \param media stopping media
     * \param stopping_reason reason why the media is stopping
     */
    using ExpectedMediaStoppingCb = void(Media&&, MediaStoppingReason);

    /**
     * Callback prototype that notify when the player state changed
     *
     * \param state new player state
     */
    using ExpectedStateChangedCb = void(LibvlcState);

    /**
     * Callback prototype that notify when the player buffering changed
     *
     * This event is always called with the 0 and 1 values before a playback
     * (in case of success). Values in between depends on the media type.
     *
     * \param buffering buffering in the range [0:1]
     */
    using ExpectedBufferingChangedCb = void(float);

    /**
     * Callback prototype that notify when the player capabilities changed
     *
     * \param old_caps old player capabilities
     * \param new_caps new player capabilities
     */
    using ExpectedCapabilitiesChangedCb = void(Capability, Capability);

    /**
     * Callback prototype that notify when the player position changed
     *
     * \param time a valid time or 0 (in us)
     * \param pos a valid position
     */
    using ExpectedPositionChangedCb = void(std::chrono::microseconds, double);

    /**
     * Callback prototype that notify when the player length changed
     *
     * May be called when the media is opening or during playback.
     * A started and playing media doesn't have necessarily a valid length.
     *
     * \param length a valid length or 0 (in us)
     */
    using ExpectedLengthChangedCb = void(std::chrono::microseconds);

    /**
     * Callback prototype that notify when the player added, removed or updated
     * a track
     *
     * \param action added, removed or updated
     * \param type type of the track
     * \param id valid track id, call MediaPlayer::getTrackFromId()
     * to get the track description.
     */
    using ExpectedTrackListChangedCb = void(ListAction, MediaTrack::Type, std::string&&);

    /**
     * Callback prototype that notify when a track is selected or unselected
     * by the player
     *
     * \param type type of tracks being unselected or selected
     * \param unselected_id valid track id or empty (when nothing is unselected)
     * \param selected_id valid track id or empty (when nothing is selected),
     * call MediaPlayer::getTrackFromId() to get the track description.
     */
    using ExpectedTrackSelectionChangedCb = void(MediaTrack::Type, std::string&&, std::string&&);

    /**
     * Callback prototype that notify when the player added, removed or updated
     * a program
     *
     * \param action added, removed or updated
     * \param group_id valid group id
     */
    using ExpectedProgramListChangedCb = void(ListAction, int);

    /**
     * Callback prototype that notify when a program is selected or unselected
     * by the player
     *
     * \param unselected_group_id valid group id
     * \param selected_group_id valid group id
     */
    using ExpectedProgramSelectionChangedCb = void(int, int);

    /**
     * Callback prototype that notify when the player changed titles
     *
     * Call MediaPlayer::getTitleDescriptions() to get the
     * description of new titles.
     */
    using ExpectedTitlesChangedCb = void();

    /**
     * Callback prototype that notify when the player selected a new title
     *
     * \param title description of the new selected title, valid only from this
     * callback, safe to copy
     * \param idx index of the new title
     */
    using ExpectedTitleSelectionChangedCb = void(const TitleDescription&, unsigned);

    /**
     * Callback prototype that notify when the player selected a new chapter
     *
     * \param title description of the new selected title, valid only from this
     * callback, safe to copy
     * \param title_idx index of the title
     * \param chapter description of the new selected chapter, valid only from this
     * callback, safe to copy
     * \param chapter_idx index of the new selected chapter
     */
    using ExpectedChapterSelectionChangedCb = void(const TitleDescription&, unsigned, const ChapterDescription&, unsigned);

    /**
     * Callback prototype that notify when the player recording state changed
     *
     * \param recording true if recording is enabled
     * \param file_path file path of the recording, only valid when the
     * recording ends (recording == false), else empty string
     */
    using ExpectedRecordingChangedCb = void(bool, std::string&&);

    /**
     * Callback prototype that notify when the player took a screenshot
     *
     * \param file_path file path of the screenshot
     */
    using ExpectedScreenshotTakenCb = void(std::string&&);

    /**
     * Callback prototype that notify when the player parsed the current media
     *
     * Called once per media, this won't be called if the media has already
     * been parsed by the parser.
     *
     * \param media media being played/parsed
     */
    using ExpectedMediaParsedCb = void(Media&&);

    /**
     * Callback prototype that notify when metadata were update by the player
     *
     * \param media media being played/parsed, call Media::getMeta() to
     * get new metadata
     */
    using ExpectedMediaMetaChangedCb = void(Media&&);

    /**
     * Callback prototype that notify when the player added new subitems to the
     * current media
     *
     * \param media media being played/parsed, call Media::getSubitems() to
     * get sub items
     */
    using ExpectedMediaSubitemsChangedCb = void(Media&&);

    /**
     * Callback prototype that notify when the player added new attachments to
     * the current media.
     *
     * \note It can be called several times for one playback. The array
     * contains only new elements after a second call.
     *
     * \param media media being played/parsed
     * \param list list of pictures, the list is only valid from this callback,
     * each Picture object can be copied safely with list.at(index) method
     */
    using ExpectedMediaAttachmentsAddedCb = void(Media&&, const Picture::List&);

    /**
     * Callback prototype that notify when a new player vout is added or removed
     *
     * \param vout_count number of active vouts
     */
    using ExpectedVoutChangedCb = void(unsigned);

    /**
     * Callback prototype that notify when the player is corked/uncorked
     *
     * The player can be corked when the audio output loose focus or when a
     * renderer was paused from the outside.
     *
     * \param corked true if the player is corked. In that case, the user
     * should pause the player and release all external resource needed by the
     * player
     */
    using ExpectedCorkChangedCb = void(bool);

    /**
     * Callback prototype that notify when the audio volume has changed
     *
     * \param new_volume volume in the range [0;2.f]
     */
    using ExpectedAudioVolumeChangedCb = void(float);

    /**
     * Callback prototype that notify when the audio mute state has changed
     *
     * \param muted true if muted
     */
    using ExpectedAudioMuteChangedCb = void(bool);

    /**
     * Callback prototype that notify when the audio device state has changed
     *
     * \param device the device name
     */
    using ExpectedAudioDeviceChangedCb = void(std::string&&);

    class Callbacks : protected CallbackOwner<25>
    {
    private:
        enum class Idx : unsigned int
        {
            MediaChanged, MediaStopping, StateChanged, BufferingChanged,
            CapabilitiesChanged, PositionChanged, LengthChanged,
            TrackListChanged, TrackSelectionChanged,
            ProgramListChanged, ProgramSelectionChanged,
            TitlesChanged, TitleSelectionChanged, ChapterSelectionChanged,
            RecordingChanged, ScreenshotTaken,
            MediaParsed, MediaMetaChanged, MediaSubitemsChanged, MediaAttachmentsAdded,
            VoutChanged, CorkChanged,
            AudioVolumeChanged, AudioMuteChanged, AudioDeviceChanged,
        };

        libvlc_media_player_cbs m_cbs;
        friend class MediaPlayer;
        friend class MediaListPlayer;

    public:

        /**
         * Default constructor to initialize all callbacks to nullptr.
         */
        Callbacks()
        {
            m_cbs = {};
            m_cbs.version = 0;
        }

        /**
         * Sets the on_media_changed callback.
         *
         * \param mediaChangedCb \ref ExpectedMediaChangedCb
         * \return reference to this Callbacks object for chaining
         */
        template <typename MediaChangedCb>
        Callbacks& onMediaChanged( MediaChangedCb&& mediaChangedCb )
        {
            static_assert( signature_match<MediaChangedCb, ExpectedMediaChangedCb>::value,
                           "Mismatched on_media_changed callback prototype" );
            m_cbs.on_media_changed = CallbackWrapper<(unsigned int)Idx::MediaChanged,
                                     decltype(libvlc_media_player_cbs::on_media_changed)>::wrap<Media>(
                                     *m_callbacks, std::forward<MediaChangedCb>( mediaChangedCb ) );
            return *this;
        }

        /**
         * Sets the on_media_stopping callback.
         *
         * \param mediaStoppingCb \ref ExpectedMediaStoppingCb
         * \return reference to this Callbacks object for chaining
         */
        template <typename MediaStoppingCb>
        Callbacks& onMediaStopping( MediaStoppingCb&& mediaStoppingCb )
        {
            static_assert( signature_match<MediaStoppingCb, ExpectedMediaStoppingCb>::value,
                           "Mismatched on_media_stopping callback prototype" );
            m_cbs.on_media_stopping = CallbackWrapper<(unsigned int)Idx::MediaStopping,
                                      decltype(libvlc_media_player_cbs::on_media_stopping)>::wrap<
                                      Media, MediaStoppingReason>( *m_callbacks,
                                      std::forward<MediaStoppingCb>( mediaStoppingCb ) );
            return *this;
        }

        /**
         * Sets the on_state_changed callback.
         *
         * \param stateChangedCb \ref ExpectedStateChangedCb
         * \return reference to this Callbacks object for chaining
         */
        template <typename StateChangedCb>
        Callbacks& onStateChanged( StateChangedCb&& stateChangedCb )
        {
            static_assert( signature_match<StateChangedCb, ExpectedStateChangedCb>::value,
                           "Mismatched on_state_changed callback prototype" );
            m_cbs.on_state_changed = CallbackWrapper<(unsigned int)Idx::StateChanged,
                                     decltype(libvlc_media_player_cbs::on_state_changed)>::wrap<LibvlcState>(
                                     *m_callbacks, std::forward<StateChangedCb>( stateChangedCb ) );
            return *this;
        }

        /**
         * Sets the on_buffering_changed callback.
         *
         * \param bufferingChangedCb \ref ExpectedBufferingChangedCb
         * \return reference to this Callbacks object for chaining
         */
        template <typename BufferingChangedCb>
        Callbacks& onBufferingChanged( BufferingChangedCb&& bufferingChangedCb )
        {
            static_assert( signature_match<BufferingChangedCb, ExpectedBufferingChangedCb>::value,
                           "Mismatched on_buffering_changed callback prototype" );
            m_cbs.on_buffering_changed = CallbackWrapper<(unsigned int)Idx::BufferingChanged,
                                         decltype(libvlc_media_player_cbs::on_buffering_changed)>::wrap(
                                         *m_callbacks, std::forward<BufferingChangedCb>( bufferingChangedCb ) );
            return *this;
        }

        /**
         * Sets the on_capabilities_changed callback.
         *
         * \param capabilitiesChangedCb \ref ExpectedCapabilitiesChangedCb
         * \return reference to this Callbacks object for chaining
         */
        template <typename CapabilitiesChangedCb>
        Callbacks& onCapabilitiesChanged( CapabilitiesChangedCb&& capabilitiesChangedCb )
        {
            static_assert( signature_match<CapabilitiesChangedCb, ExpectedCapabilitiesChangedCb>::value,
                           "Mismatched on_capabilities_changed callback prototype" );
            m_cbs.on_capabilities_changed = CallbackWrapper<(unsigned int)Idx::CapabilitiesChanged,
                                            decltype(libvlc_media_player_cbs::on_capabilities_changed)>::wrap<
                                            Capability, Capability>( *m_callbacks,
                                            std::forward<CapabilitiesChangedCb>( capabilitiesChangedCb ) );
            return *this;
        }

        /**
         * Sets the on_position_changed callback.
         *
         * \param positionChangedCb \ref ExpectedPositionChangedCb
         * \return reference to this Callbacks object for chaining
         */
        template <typename PositionChangedCb>
        Callbacks& onPositionChanged( PositionChangedCb&& positionChangedCb )
        {
            static_assert( signature_match<PositionChangedCb, ExpectedPositionChangedCb>::value,
                           "Mismatched on_position_changed callback prototype" );
            m_cbs.on_position_changed = CallbackWrapper<(unsigned int)Idx::PositionChanged,
                                        decltype(libvlc_media_player_cbs::on_position_changed)>::wrap<
                                        std::chrono::microseconds, double>(
                                        *m_callbacks, std::forward<PositionChangedCb>( positionChangedCb ) );
            return *this;
        }

        /**
         * Sets the on_length_changed callback.
         *
         * \param lengthChangedCb \ref ExpectedLengthChangedCb
         * \return reference to this Callbacks object for chaining
         */
        template <typename LengthChangedCb>
        Callbacks& onLengthChanged( LengthChangedCb&& lengthChangedCb )
        {
            static_assert( signature_match<LengthChangedCb, ExpectedLengthChangedCb>::value,
                           "Mismatched on_length_changed callback prototype" );
            m_cbs.on_length_changed = CallbackWrapper<(unsigned int)Idx::LengthChanged,
                                      decltype(libvlc_media_player_cbs::on_length_changed)>::wrap<
                                      std::chrono::microseconds>(
                                      *m_callbacks, std::forward<LengthChangedCb>( lengthChangedCb ) );
            return *this;
        }

        /**
         * Sets the on_track_list_changed callback.
         *
         * \param trackListChangedCb \ref ExpectedTrackListChangedCb
         * \return reference to this Callbacks object for chaining
         */
        template <typename TrackListChangedCb>
        Callbacks& onTrackListChanged( TrackListChangedCb&& trackListChangedCb )
        {
            static_assert( signature_match<TrackListChangedCb, ExpectedTrackListChangedCb>::value,
                           "Mismatched on_track_list_changed callback prototype" );
            m_cbs.on_track_list_changed = CallbackWrapper<(unsigned int)Idx::TrackListChanged,
                                          decltype(libvlc_media_player_cbs::on_track_list_changed)>::wrap<
                                          ListAction, MediaTrack::Type, std::string>(
                                          *m_callbacks, std::forward<TrackListChangedCb>( trackListChangedCb ) );
            return *this;
        }

        /**
         * Sets the on_track_selection_changed callback.
         *
         * \param trackSelectionChangedCb \ref ExpectedTrackSelectionChangedCb
         * \return reference to this Callbacks object for chaining
         */
        template <typename TrackSelectionChangedCb>
        Callbacks& onTrackSelectionChanged( TrackSelectionChangedCb&& trackSelectionChangedCb )
        {
            static_assert( signature_match<TrackSelectionChangedCb, ExpectedTrackSelectionChangedCb>::value,
                           "Mismatched on_track_selection_changed callback prototype" );
            m_cbs.on_track_selection_changed = CallbackWrapper<(unsigned int)Idx::TrackSelectionChanged,
                                               decltype(libvlc_media_player_cbs::on_track_selection_changed)>::wrap<
                                               MediaTrack::Type, std::string, std::string>( *m_callbacks,
                                               std::forward<TrackSelectionChangedCb>( trackSelectionChangedCb ) );
            return *this;
        }

        /**
         * Sets the on_program_list_changed callback.
         *
         * \param programListChangedCb \ref ExpectedProgramListChangedCb
         * \return reference to this Callbacks object for chaining
         */
        template <typename ProgramListChangedCb>
        Callbacks& onProgramListChanged( ProgramListChangedCb&& programListChangedCb )
        {
            static_assert( signature_match<ProgramListChangedCb, ExpectedProgramListChangedCb>::value,
                           "Mismatched on_program_list_changed callback prototype" );
            m_cbs.on_program_list_changed = CallbackWrapper<(unsigned int)Idx::ProgramListChanged,
                                            decltype(libvlc_media_player_cbs::on_program_list_changed)>::wrap<
                                            ListAction, int>( *m_callbacks, std::forward<ProgramListChangedCb>(
                                            programListChangedCb ) );
            return *this;
        }

        /**
         * Sets the on_program_selection_changed callback.
         *
         * \param programSelectionChangedCb \ref ExpectedProgramSelectionChangedCb
         * \return reference to this Callbacks object for chaining
         */
        template <typename ProgramSelectionChangedCb>
        Callbacks& onProgramSelectionChanged( ProgramSelectionChangedCb&& programSelectionChangedCb )
        {
            static_assert( signature_match<ProgramSelectionChangedCb, ExpectedProgramSelectionChangedCb>::value,
                           "Mismatched on_program_selection_changed callback prototype" );
            m_cbs.on_program_selection_changed = CallbackWrapper<(unsigned int)Idx::ProgramSelectionChanged,
                                                 decltype(libvlc_media_player_cbs::on_program_selection_changed)>::wrap(
                                                 *m_callbacks, std::forward<ProgramSelectionChangedCb>(
                                                 programSelectionChangedCb ) );
            return *this;
        }

        /**
         * Sets the on_titles_changed callback.
         *
         * \param titlesChangedCb \ref ExpectedTitlesChangedCb
         * \return reference to this Callbacks object for chaining
         */
        template <typename TitlesChangedCb>
        Callbacks& onTitlesChanged( TitlesChangedCb&& titlesChangedCb )
        {
            static_assert( signature_match<TitlesChangedCb, ExpectedTitlesChangedCb>::value,
                           "Mismatched on_titles_changed callback prototype" );
            m_cbs.on_titles_changed = CallbackWrapper<(unsigned int)Idx::TitlesChanged,
                                      decltype(libvlc_media_player_cbs::on_titles_changed)>::wrap(
                                      *m_callbacks, std::forward<TitlesChangedCb>( titlesChangedCb ) );
            return *this;
        }

        /**
         * Sets the on_title_selection_changed callback.
         *
         * \param titleSelectionChangedCb \ref ExpectedTitleSelectionChangedCb
         * \return reference to this Callbacks object for chaining
         */
        template <typename TitleSelectionChangedCb>
        Callbacks& onTitleSelectionChanged( TitleSelectionChangedCb&& titleSelectionChangedCb )
        {
            static_assert( signature_match<TitleSelectionChangedCb, ExpectedTitleSelectionChangedCb>::value,
                           "Mismatched on_title_selection_changed callback prototype" );
            m_cbs.on_title_selection_changed = CallbackWrapper<(unsigned int)Idx::TitleSelectionChanged,
                                               decltype(libvlc_media_player_cbs::on_title_selection_changed)>::wrap<
                                               TitleDescription, unsigned>( *m_callbacks, std::forward<TitleSelectionChangedCb>( titleSelectionChangedCb ) );
            return *this;
        }

        /**
         * Sets the on_chapter_selection_changed callback.
         *
         * \param chapterSelectionChangedCb \ref ExpectedChapterSelectionChangedCb
         * \return reference to this Callbacks object for chaining
         */
        template <typename ChapterSelectionChangedCb>
        Callbacks& onChapterSelectionChanged( ChapterSelectionChangedCb&& chapterSelectionChangedCb )
        {
            static_assert( signature_match<ChapterSelectionChangedCb, ExpectedChapterSelectionChangedCb>::value,
                           "Mismatched on_chapter_selection_changed callback prototype" );
            m_cbs.on_chapter_selection_changed = CallbackWrapper<(unsigned int)Idx::ChapterSelectionChanged,
                                                 decltype(libvlc_media_player_cbs::on_chapter_selection_changed)>::wrap<
                                                 TitleDescription, unsigned, ChapterDescription, unsigned>(
                                                 *m_callbacks, std::forward<ChapterSelectionChangedCb>(
                                                 chapterSelectionChangedCb ) );
            return *this;
        }

        /**
         * Sets the on_recording_changed callback.
         *
         * \param recordingChangedCb \ref ExpectedRecordingChangedCb
         * \return reference to this Callbacks object for chaining
         */
        template <typename RecordingChangedCb>
        Callbacks& onRecordingChanged( RecordingChangedCb&& recordingChangedCb )
        {
            static_assert( signature_match<RecordingChangedCb, ExpectedRecordingChangedCb>::value,
                           "Mismatched on_recording_changed callback prototype" );
            m_cbs.on_recording_changed = CallbackWrapper<(unsigned int)Idx::RecordingChanged,
                                         decltype(libvlc_media_player_cbs::on_recording_changed)>::wrap(
                                         *m_callbacks, std::forward<RecordingChangedCb>( recordingChangedCb ) );
            return *this;
        }

        /**
         * Sets the on_screenshot_taken callback.
         *
         * \param screenshotTakenCb \ref ExpectedScreenshotTakenCb
         * \return reference to this Callbacks object for chaining
         */
        template <typename ScreenshotTakenCb>
        Callbacks& onScreenshotTaken( ScreenshotTakenCb&& screenshotTakenCb )
        {
            static_assert( signature_match<ScreenshotTakenCb, ExpectedScreenshotTakenCb>::value,
                           "Mismatched on_screenshot_taken callback prototype" );
            m_cbs.on_screenshot_taken = CallbackWrapper<(unsigned int)Idx::ScreenshotTaken,
                                        decltype(libvlc_media_player_cbs::on_screenshot_taken)>::wrap(
                                        *m_callbacks, std::forward<ScreenshotTakenCb>( screenshotTakenCb ) );
            return *this;
        }

        /**
         * Sets the on_media_parsed callback.
         *
         * \param mediaParsedCb \ref ExpectedMediaParsedCb
         * \return reference to this Callbacks object for chaining
         */
        template <typename MediaParsedCb>
        Callbacks& onMediaParsed( MediaParsedCb&& mediaParsedCb )
        {
            static_assert( signature_match<MediaParsedCb, ExpectedMediaParsedCb>::value,
                           "Mismatched on_media_parsed callback prototype" );
            m_cbs.on_media_parsed = CallbackWrapper<(unsigned int)Idx::MediaParsed,
                                    decltype(libvlc_media_player_cbs::on_media_parsed)>::wrap<Media>(
                                    *m_callbacks, std::forward<MediaParsedCb>( mediaParsedCb ) );
            return *this;
        }

        /**
         * Sets the on_media_meta_changed callback.
         *
         * \param mediaMetaChangedCb \ref ExpectedMediaMetaChangedCb
         * \return reference to this Callbacks object for chaining
         */
        template <typename MediaMetaChangedCb>
        Callbacks& onMediaMetaChanged( MediaMetaChangedCb&& mediaMetaChangedCb )
        {
            static_assert( signature_match<MediaMetaChangedCb, ExpectedMediaMetaChangedCb>::value,
                           "Mismatched on_media_meta_changed callback prototype" );
            m_cbs.on_media_meta_changed = CallbackWrapper<(unsigned int)Idx::MediaMetaChanged,
                                          decltype(libvlc_media_player_cbs::on_media_meta_changed)>::wrap<Media>(
                                          *m_callbacks, std::forward<MediaMetaChangedCb>( mediaMetaChangedCb ) );
            return *this;
        }

        /**
         * Sets the on_media_subitems_changed callback.
         *
         * \param mediaSubitemsChangedCb \ref ExpectedMediaSubitemsChangedCb
         * \return reference to this Callbacks object for chaining
         */
        template <typename MediaSubitemsChangedCb>
        Callbacks& onMediaSubitemsChanged( MediaSubitemsChangedCb&& mediaSubitemsChangedCb )
        {
            static_assert( signature_match<MediaSubitemsChangedCb, ExpectedMediaSubitemsChangedCb>::value,
                           "Mismatched on_media_subitems_changed callback prototype" );
            m_cbs.on_media_subitems_changed = CallbackWrapper<(unsigned int)Idx::MediaSubitemsChanged,
                                              decltype(libvlc_media_player_cbs::on_media_subitems_changed)>::wrap<Media>(
                                              *m_callbacks, std::forward<MediaSubitemsChangedCb>( mediaSubitemsChangedCb ) );
            return *this;
        }

        /**
         * Sets the on_media_attachments_added callback.
         *
         * \param mediaAttachmentsAddedCb \ref ExpectedMediaAttachmentsAddedCb
         * \return reference to this Callbacks object for chaining
         */
        template <typename MediaAttachmentsAddedCb>
        Callbacks& onMediaAttachmentsAdded( MediaAttachmentsAddedCb&& mediaAttachmentsAddedCb )
        {
            static_assert( signature_match<MediaAttachmentsAddedCb, ExpectedMediaAttachmentsAddedCb>::value,
                           "Mismatched on_media_attachments_added callback prototype" );
            m_cbs.on_media_attachments_added = CallbackWrapper<(unsigned int)Idx::MediaAttachmentsAdded,
                                               decltype(libvlc_media_player_cbs::on_media_attachments_added)>::wrap<
                                               Media, Picture::List>(
                                               *m_callbacks, std::forward<MediaAttachmentsAddedCb>( mediaAttachmentsAddedCb ) );
            return *this;
        }

        /**
         * Sets the on_vout_changed callback.
         *
         * \param voutChangedCb \ref ExpectedVoutChangedCb
         * \return reference to this Callbacks object for chaining
         */
        template <typename VoutChangedCb>
        Callbacks& onVoutChanged( VoutChangedCb&& voutChangedCb )
        {
            static_assert( signature_match<VoutChangedCb, ExpectedVoutChangedCb>::value,
                           "Mismatched on_vout_changed callback prototype" );
            m_cbs.on_vout_changed = CallbackWrapper<(unsigned int)Idx::VoutChanged,
                                    decltype(libvlc_media_player_cbs::on_vout_changed)>::wrap(
                                    *m_callbacks, std::forward<VoutChangedCb>( voutChangedCb ) );
            return *this;
        }

        /**
         * Sets the on_cork_changed callback.
         *
         * \param corkChangedCb \ref ExpectedCorkChangedCb
         * \return reference to this Callbacks object for chaining
         */
        template <typename CorkChangedCb>
        Callbacks& onCorkChanged( CorkChangedCb&& corkChangedCb )
        {
            static_assert( signature_match<CorkChangedCb, ExpectedCorkChangedCb>::value,
                           "Mismatched on_cork_changed callback prototype" );
            m_cbs.on_cork_changed = CallbackWrapper<(unsigned int)Idx::CorkChanged,
                                    decltype(libvlc_media_player_cbs::on_cork_changed)>::wrap(
                                    *m_callbacks, std::forward<CorkChangedCb>( corkChangedCb ) );
            return *this;
        }

        /**
         * Sets the on_audio_volume_changed callback.
         *
         * \param audioVolumeChangedCb \ref ExpectedAudioVolumeChangedCb
         * \return reference to this Callbacks object for chaining
         */
        template <typename AudioVolumeChangedCb>
        Callbacks& onAudioVolumeChanged( AudioVolumeChangedCb&& audioVolumeChangedCb )
        {
            static_assert( signature_match<AudioVolumeChangedCb, ExpectedAudioVolumeChangedCb>::value,
                           "Mismatched on_audio_volume_changed callback prototype" );
            m_cbs.on_audio_volume_changed = CallbackWrapper<(unsigned int)Idx::AudioVolumeChanged,
                                            decltype(libvlc_media_player_cbs::on_audio_volume_changed)>::wrap(
                                            *m_callbacks, std::forward<AudioVolumeChangedCb>( audioVolumeChangedCb ) );
            return *this;
        }

        /**
         * Sets the on_audio_mute_changed callback.
         *
         * \param audioMuteChangedCb \ref ExpectedAudioMuteChangedCb
         * \return reference to this Callbacks object for chaining
         */
        template <typename AudioMuteChangedCb>
        Callbacks& onAudioMuteChanged( AudioMuteChangedCb&& audioMuteChangedCb )
        {
            static_assert( signature_match<AudioMuteChangedCb, ExpectedAudioMuteChangedCb>::value,
                           "Mismatched on_audio_mute_changed callback prototype" );
            m_cbs.on_audio_mute_changed = CallbackWrapper<(unsigned int)Idx::AudioMuteChanged,
                                          decltype(libvlc_media_player_cbs::on_audio_mute_changed)>::wrap(
                                          *m_callbacks, std::forward<AudioMuteChangedCb>( audioMuteChangedCb ) );
            return *this;
        }

        /**
         * Sets the on_audio_device_changed callback.
         *
         * \param audioDeviceChangedCb \ref ExpectedAudioDeviceChangedCb
         * \return reference to this Callbacks object for chaining
         */
        template <typename AudioDeviceChangedCb>
        Callbacks& onAudioDeviceChanged( AudioDeviceChangedCb&& audioDeviceChangedCb )
        {
            static_assert( signature_match<AudioDeviceChangedCb, ExpectedAudioDeviceChangedCb>::value,
                           "Mismatched on_audio_device_changed callback prototype" );
            m_cbs.on_audio_device_changed = CallbackWrapper<(unsigned int)Idx::AudioDeviceChanged,
                                            decltype(libvlc_media_player_cbs::on_audio_device_changed)>::wrap(
                                            *m_callbacks, std::forward<AudioDeviceChangedCb>( audioDeviceChangedCb ) );
            return *this;
        }
    };

    /**
     * Check if 2 MediaPlayer objects contain the same libvlc_media_player_t.
     * \param another another MediaPlayer
     * \return true if they contain the same libvlc_media_player_t
     */
    bool operator==(const MediaPlayer& another) const
    {
        return m_obj == another.m_obj;
    }

    /**
     * Create an empty Media Player object
     *
     * \param p_libvlc_instance  the libvlc instance in which the Media
     * Player should be created.
     */
    MediaPlayer( const Instance& instance )
        : Internal{ libvlc_media_player_new( getInternalPtr<libvlc_instance_t>( instance ),
                                             nullptr, nullptr ),
                    libvlc_media_player_release }
    {
    }

    /**
     * Create an empty Media Player object with callbacks to listen to events.
     *
     * \param inst  the libvlc instance
     * \param cbs pre-built \ref Callbacks object
     *
     * \warning The application must ensure that the Callbacks object supplied
     * remains valid and unmodified until the media player is destroyed.
     */
    MediaPlayer( const Instance& instance, const Callbacks& cbs )
    {
        auto ptr = libvlc_media_player_new( getInternalPtr<libvlc_instance_t>( instance ),
                                            &cbs.m_cbs, cbs.m_callbacks.get() );
        if ( ptr == nullptr )
            throw std::runtime_error( "Failed to create media player" );
        m_obj.reset( ptr, libvlc_media_player_release );
    }

    /**
     * Create a Media Player object from a Media
     *
     * \param inst the libvlc instance
     * \param md   the media. Afterwards the p_md can be safely destroyed.
     */
    MediaPlayer( const Instance& inst, Media& md )
        : Internal{ libvlc_media_player_new_from_media(
                        getInternalPtr<libvlc_instance_t>( inst ),
                        getInternalPtr<libvlc_media_t>( md ),
                        nullptr, nullptr ),
                    libvlc_media_player_release }
    {
    }

    /**
     * Create a Media Player object from a Media with callbacks to listen to events.
     *
     * \param inst the libvlc instance
     * \param md   the media. Afterwards the p_md can be safely destroyed.
     * \param cbs  pre-built \ref Callbacks object
     *
     * \warning The application must ensure that the Callbacks object supplied
     * remains valid and unmodified until the media player is destroyed.
     */
    MediaPlayer( const Instance& inst, Media& md, const Callbacks& cbs )
    {
        auto ptr = libvlc_media_player_new_from_media(
                        getInternalPtr<libvlc_instance_t>( inst ),
                        getInternalPtr<libvlc_media_t>( md ),
                        &cbs.m_cbs, cbs.m_callbacks.get() );
        if ( ptr == nullptr )
            throw std::runtime_error( "Failed to create media player" );
        m_obj.reset( ptr, libvlc_media_player_release );
    }

    /**
     * Create an empty VLC MediaPlayer instance.
     *
     * Calling any method on such an instance is undefined.
    */
    MediaPlayer() = default;

    /**
     * Set the media that will be used by the media_player. If any, previous
     * md will be released.
     *
     * \param p_md  the Media. Afterwards the p_md can be safely destroyed.
     */
    void setMedia(Media& md)
    {
        libvlc_media_player_set_media( *this, getInternalPtr<libvlc_media_t>( md ) );
    }

    /**
     * Get the media used by the media_player.
     *
     * \return the media associated with p_mi, or an empty Media object if no
     * media is associated
     */
    Media media()
    {
        auto media = libvlc_media_player_get_media(*this);
        if ( media == nullptr )
            return Media();
        return Media( media, false );
    }

    /**
     * \return true if the media player is playing, 0 otherwise
     */
    bool isPlaying()
    {
        return libvlc_media_player_is_playing(*this) != 0;
    }

    /**
     * @brief play Start playback
     *
     * If playback was already started, this method has no effect
     */
    bool play()
    {
        return libvlc_media_player_play(*this) == 0;
    }

    /**
     * Pause or resume (no effect if there is no media)
     *
     * \param do_pause  play/resume if true, pause if false
     *
     * \version LibVLC 1.1.1 or later
     */
    void setPause(bool pause)
    {
        libvlc_media_player_set_pause(*this, pause);
    }

    /**
     * @brief pause Toggle pause (no effect if there is no media)
     */
    void pause()
    {
        libvlc_media_player_pause(*this);
    }

    /**
     * @brief stop Stop the playback (no effect if there is no media)
     */
    void stopAsync()
    {
        libvlc_media_player_stop_async(*this);
    }

    /**
     * Set the NSView handler where the media player should render its video
     * output.
     *
     * Use the vout called "macosx".
     *
     * The drawable is an NSObject that follow the
     * VLCOpenGLVideoViewEmbedding protocol: VLCOpenGLVideoViewEmbedding <NSObject>
     *
     * Or it can be an NSView object.
     *
     * If you want to use it along with Qt4 see the QMacCocoaViewContainer.
     * Then the following code should work:  { NSView *video = [[NSView
     * alloc] init]; QMacCocoaViewContainer *container = new
     * QMacCocoaViewContainer(video, parent);
     * libvlc_media_player_set_nsobject(mp, video); [video release]; }
     *
     * You can find a live example in VLCVideoView in VLCKit.framework.
     *
     * \param drawable  the drawable that is either an NSView or an object
     * following the VLCOpenGLVideoViewEmbedding protocol.
     */
    void setNsobject(void* drawable)
    {
        libvlc_media_player_set_nsobject(*this, drawable);
    }

    /**
     * Get the NSView handler previously set with MediaPlayer::setNsobject()
     * .
     *
     * \return the NSView handler or 0 if none where set
     */
    void* nsobject()
    {
        return libvlc_media_player_get_nsobject(*this);
    }

    /**
     * Set an X Window System drawable where the media player should render
     * its video output. If LibVLC was built without X11 output support, then
     * this has no effects.
     *
     * The specified identifier must correspond to an existing Input/Output
     * class X11 window. Pixmaps are supported. The caller shall ensure that
     * the X11 server is the same as the one the VLC instance has been
     * configured with. This function must be called before video playback is
     * started; otherwise it will only take effect after playback stop and
     * restart.
     *
     * \param drawable  the ID of the X window
     */
    void setXwindow(uint32_t drawable)
    {
        libvlc_media_player_set_xwindow(*this, drawable);
    }

    /**
     * Get the X Window System window identifier previously set with
     * MediaPlayer::setXwindow() . Note that this will return the identifier
     * even if VLC is not currently using it (for instance if it is playing
     * an audio-only input).
     *
     * \return an X window ID, or 0 if none where set.
     */
    uint32_t xwindow()
    {
        return libvlc_media_player_get_xwindow(*this);
    }

    /**
     * Set a Win32/Win64 API window handle (HWND) where the media player
     * should render its video output. If LibVLC was built without
     * Win32/Win64 API output support, then this has no effects.
     *
     * \param drawable  windows handle of the drawable
     */
    void setHwnd(void * drawable)
    {
        libvlc_media_player_set_hwnd(*this, drawable);
    }

    /**
     * Get the Windows API window handle (HWND) previously set with
     * MediaPlayer::setHwnd() . The handle will be returned even if LibVLC is
     * not currently outputting any video to it.
     *
     * \return a window handle or nullptr if there are none.
     */
    void* hwnd()
    {
        return libvlc_media_player_get_hwnd(*this);
    }

    /**
     * Get the current movie length (in us).
     *
     * \return the movie length (in us), or -1 if there is no media.
     */
    std::chrono::microseconds length()
    {
        return std::chrono::microseconds{ libvlc_media_player_get_length(*this) };
    }

    /**
     * Get the current movie time (in us).
     *
     * \return the movie time (in us), or -1 if there is no media.
     */
    std::chrono::microseconds time()
    {
        return std::chrono::microseconds{ libvlc_media_player_get_time(*this) };
    }

    /**
     * Set the movie time (in us). This has no effect if no media is being
     * played. Not all formats and protocols support this.
     *
     * \version{2.x}
     * \version{3.x}
     * \param i_time  the movie time (in us).
     * \version{4.x}
     * \param b_fast  prefer fast seeking or precise seeking
     */
    void setTime(std::chrono::microseconds i_time, bool b_fast)
    {
        libvlc_media_player_set_time(*this, i_time.count(), b_fast);
    }

    /**
     * Get movie position as percentage between 0.0 and 1.0.
     *
     * \return movie position, or -1. in case of error
     */
    float position()
    {
        return libvlc_media_player_get_position(*this);
    }

    /**
     * Set movie position as percentage between 0.0 and 1.0. This has no
     * effect if playback is not enabled. This might not work depending on
     * the underlying input format and protocol.
     *
     * \version{2.x}
     * \version{3.x}
     * \param f_pos  the position
     * \version{4.x}
     * \param b_fast prefer fast seeking or precise seeking
     */
    void setPosition(float f_pos, bool b_fast)
    {
        libvlc_media_player_set_position(*this, f_pos, b_fast);
    }

    /**
     * Set movie chapter (if applicable).
     *
     * \param i_chapter  chapter number to play
     */
    void setChapter(int i_chapter)
    {
        libvlc_media_player_set_chapter(*this, i_chapter);
    }

    /**
     * Get movie chapter.
     *
     * \return chapter number currently playing, or -1 if there is no media.
     */
    int chapter()
    {
        return libvlc_media_player_get_chapter(*this);
    }

    /**
     * Get movie chapter count
     *
     * \return number of chapters in movie, or -1.
     */
    int chapterCount()
    {
        return libvlc_media_player_get_chapter_count(*this);
    }

    /**
     * Get title chapter count
     *
     * \param i_title  title
     *
     * \return number of chapters in title, or -1
     */
    int chapterCountForTitle(int i_title)
    {
        return libvlc_media_player_get_chapter_count_for_title(*this, i_title);
    }

    /**
     * Set movie title
     *
     * \param i_title  title number to play
     */
    void setTitle(int i_title)
    {
        libvlc_media_player_set_title(*this, i_title);
    }

    /**
     * Get movie title
     *
     * \return title number currently playing, or -1
     */
    int title()
    {
        return libvlc_media_player_get_title(*this);
    }

    /**
     * Get movie title count
     *
     * \return title number count, or -1
     */
    int titleCount()
    {
        return libvlc_media_player_get_title_count(*this);
    }

    /**
     * Set previous chapter (if applicable)
     */
    void previousChapter()
    {
        libvlc_media_player_previous_chapter(*this);
    }

    /**
     * Set next chapter (if applicable)
     */
    void nextChapter()
    {
        libvlc_media_player_next_chapter(*this);
    }

    /**
     * Get the requested movie play rate.
     *
     * \warning Depending on the underlying media, the requested rate may be
     * different from the real playback rate.
     *
     * \return movie play rate
     */
    float rate()
    {
        return libvlc_media_player_get_rate(*this);
    }

    /**
     * Set movie play rate
     *
     * \param rate  movie play rate to set
     *
     * \return -1 if an error was detected, 0 otherwise (but even then, it
     * might not actually work depending on the underlying media protocol)
     */
    int setRate(float rate)
    {
        return libvlc_media_player_set_rate(*this, rate);
    }

    /**
     * Get current movie state
     *
     * \return the current state of the media player (playing, paused, ...)
     *
     * \see libvlc_state_t
     */
    libvlc_state_t state()
    {
        return libvlc_media_player_get_state(*this);
    }

    /**
     * Get the amount of video outputs this media player has?
     *
     * \return the number of video outputs
     */
    uint32_t hasVout()
    {
        return libvlc_media_player_has_vout(*this);
    }

    /**
     * Is this media player seekable?
     *
     * \return true if the media player can seek
     */
    bool isSeekable()
    {
        return libvlc_media_player_is_seekable(*this) != 0;
    }

    /**
     * Can this media player be paused?
     *
     * \return true if the media player can pause
     */
    bool canPause()
    {
        return libvlc_media_player_can_pause(*this) != 0;
    }

    /**
     * Check if the current program is scrambled
     *
     * \return true if the current program is scrambled
     *
     * \version LibVLC 2.2.0 or later
     */
    bool programScrambled()
    {
        return libvlc_media_player_program_scrambled(*this) != 0;
    }

    /**
     * Display the next frame (if supported)
     */
    void nextFrame()
    {
        libvlc_media_player_next_frame(*this);
    }

    /**
     * Navigate through DVD Menu
     *
     * \param navigate  the Navigation mode
     *
     * \version libVLC 2.0.0 or later
     */
    void navigate(unsigned navigate)
    {
        libvlc_media_player_navigate(*this, navigate);
    }

    /**
     * Set if, and how, the video title will be shown when media is played.
     *
     * \param position  position at which to display the title, or
     * libvlc_position_disable to prevent the title from being displayed
     *
     * \param timeout  title display timeout in milliseconds (ignored if
     * libvlc_position_disable)
     *
     * \version libVLC 2.1.0 or later
     */
    void setVideoTitleDisplay(libvlc_position_t position, unsigned int timeout)
    {
        libvlc_media_player_set_video_title_display(*this, position, timeout);
    }

    /**
     * Toggle fullscreen status on non-embedded video outputs.
     *
     * \warning The same limitations applies to this function as to
     * MediaPlayer::setFullscreen() .
     */
    void toggleFullscreen()
    {
        libvlc_toggle_fullscreen(*this);
    }

    /**
     * Enable or disable fullscreen.
     *
     * \warning With most window managers, only a top-level windows can be in
     * full-screen mode. Hence, this function will not operate properly if
     * MediaPlayer::setXwindow() was used to embed the video in a non-top-
     * level window. In that case, the embedding window must be reparented to
     * the root window fullscreen mode is enabled. You will want to reparent
     * it back to its normal parent when disabling fullscreen.
     *
     * \param b_fullscreen  boolean for fullscreen status
     */
    void setFullscreen(bool fullscreen)
    {
        libvlc_set_fullscreen( *this, fullscreen );
    }

    /**
     * Get current fullscreen status.
     *
     * \return the fullscreen status (boolean)
     */
    bool fullscreen()
    {
        return libvlc_get_fullscreen(*this) != 0;
    }

    /**
     * Apply new equalizer settings to a media player.
     *
     * The equalizer is first created by invoking
     * libvlc_audio_equalizer_new() or
     * libvlc_audio_equalizer_new_from_preset() .
     *
     * It is possible to apply new equalizer settings to a media player
     * whether the media player is currently playing media or not.
     *
     * Invoking this method will immediately apply the new equalizer settings
     * to the audio output of the currently playing media if there is any.
     *
     * If there is no currently playing media, the new equalizer settings
     * will be applied later if and when new media is played.
     *
     * Equalizer settings will automatically be applied to subsequently
     * played media.
     *
     * To disable the equalizer for a media player invoke this method passing
     * NULL for the p_equalizer parameter.
     *
     * The media player does not keep a reference to the supplied equalizer
     * so it is safe for an application to release the equalizer reference
     * any time after this method returns.
     *
     * \param equalizer  The equalizer to be used by this media player
     *
     * \return true on success, false otherwise
     *
     * \version LibVLC 2.2.0 or later
     */
    bool setEqualizer(Equalizer& equalizer)
    {
        return libvlc_media_player_set_equalizer( *this, equalizer ) == 0;
    }

    ///
    /// \brief unsetEqualizer disable equalizer for this media player
    ///
    /// \return true on success, false otherwise.
    ///
    bool unsetEqualizer()
    {
        return libvlc_media_player_set_equalizer( *this, nullptr ) == 0;
    }

    /**
     * Set callbacks and private data for decoded audio. Use
     * MediaPlayer::setFormat() or MediaPlayer::setFormatCallbacks() to configure the
     * decoded audio format.
     *
     * \param play  callback to play audio samples (must not be nullptr)
     *              Required prototype is: void(const void *samples, unsigned count, int64_t pts)
     *
     * \param pause  callback to pause playback (or nullptr to ignore)
     *               Required prototype is void(int64_t pts);
     *
     * \param resume callback to resume playback (or nullptr to ignore)
     *               Required prototype is void(int64_t pts);
     *
     * \param flush  callback to flush audio buffers (or nullptr to ignore)
     *               Required prototype is void(int64_t pts);
     *
     * \param drain  callback to drain audio buffers (or nullptr to ignore)
     * *             Required prototype is void();
     *
     * \version LibVLC 2.0.0 or later
     */
    template <typename PlayCb, typename PauseCb, typename ResumeCb, typename FlushCb, typename DrainCb>
    void setAudioCallbacks(PlayCb&& play, PauseCb&& pause, ResumeCb&& resume, FlushCb&& flush, DrainCb&& drain)
    {
        static_assert(signature_match<PlayCb, void(const void*, unsigned int, int64_t)>::value, "Mismatched play callback prototype");
        static_assert(signature_match_or_nullptr<PauseCb, void(int64_t)>::value, "Mismatched pause callback prototype");
        static_assert(signature_match_or_nullptr<ResumeCb, void(int64_t)>::value, "Mismatched resume callback prototype");
        static_assert(signature_match_or_nullptr<FlushCb, void(int64_t pts)>::value, "Mismatched flush callback prototype");
        static_assert(signature_match_or_nullptr<DrainCb, void()>::value, "Mismatched drain callback prototype");

        libvlc_audio_set_callbacks( *this,
            CallbackWrapper<(unsigned int)CallbackIdx::AudioPlay,   libvlc_audio_play_cb>::wrap(   *m_callbacks, std::forward<PlayCb>( play ) ),
            CallbackWrapper<(unsigned int)CallbackIdx::AudioPause,  libvlc_audio_pause_cb>::wrap(  *m_callbacks, std::forward<PauseCb>( pause ) ),
            CallbackWrapper<(unsigned int)CallbackIdx::AudioResume, libvlc_audio_resume_cb>::wrap( *m_callbacks, std::forward<ResumeCb>( resume ) ),
            CallbackWrapper<(unsigned int)CallbackIdx::AudioFlush,  libvlc_audio_flush_cb>::wrap(  *m_callbacks, std::forward<FlushCb>( flush ) ),
            CallbackWrapper<(unsigned int)CallbackIdx::AudioDrain,  libvlc_audio_drain_cb>::wrap(  *m_callbacks, std::forward<DrainCb>( drain ) ),
            // We will receive the pointer as a void*, we need to offset the value *now*, otherwise we'd get
            // a shifted value, resulting in an invalid callback array.
            m_callbacks.get() );
    }

    /**
     * Set callbacks and private data for decoded audio. Use
     * MediaPlayer::setFormat() or MediaPlayer::setFormatCallbacks() to configure the
     * decoded audio format.
     *
     * \param set_volume  callback to apply audio volume, or nullptr to apply
     *                    volume in software
     *                    Expected prototype is void(float volume, bool mute)
     *
     * \version LibVLC 2.0.0 or later
     */
    template <typename VolumeCb>
    void setVolumeCallback(VolumeCb&& func)
    {
        static_assert(signature_match_or_nullptr<VolumeCb, void(float, bool)>::value, "Mismatched set volume callback");
        libvlc_audio_set_volume_callback(*this,
            CallbackWrapper<(unsigned int)CallbackIdx::AudioVolume, libvlc_audio_set_volume_cb>::wrap( this, std::forward<VolumeCb>( func ) ) );
    }

    /**
     * Set decoded audio format. This only works in combination with
     * MediaPlayer::setCallbacks() .
     *
     * \param setup  callback to select the audio format (cannot be a nullptr)
     *               Expected prototype is int(char *format, unsigned *rate, unsigned *channels)
     *               Where the return value is 0 for success, anything else to skip audio playback
     *
     * \param cleanup  callback to release any allocated resources (or nullptr)
     *                 Expected prototype is void()
     *
     * \version LibVLC 2.0.0 or later
     */
    template <typename SetupCb, typename CleanupCb>
    void setAudioFormatCallbacks(SetupCb&& setup, CleanupCb&& cleanup)
    {
        static_assert(signature_match<SetupCb, int(char*, uint32_t*, uint32_t*)>::value, "Mismatched Setup callback");
        static_assert(signature_match_or_nullptr<CleanupCb, void()>::value, "Mismatched cleanup callback");

        libvlc_audio_set_format_callbacks(*this,
            CallbackWrapper<(unsigned int)CallbackIdx::AudioSetup, libvlc_audio_setup_cb>::wrap( this, std::forward<SetupCb>( setup ) ),
            CallbackWrapper<(unsigned int)CallbackIdx::AudioCleanup, libvlc_audio_cleanup_cb>::wrap( this, std::forward<CleanupCb>( cleanup ) ) );
    }

    /**
     * Set decoded audio format. This only works in combination with
     * MediaPlayer::setCallbacks() , and is mutually exclusive with
     * MediaPlayer::setFormatCallbacks() .
     *
     * \param format  a four-characters string identifying the sample format
     * (e.g. "S16N" or "FL32")
     *
     * \param rate  sample rate (expressed in Hz)
     *
     * \param channels  channels count
     *
     * \version LibVLC 2.0.0 or later
     */
    void setAudioFormat(const std::string& format, unsigned rate, unsigned channels)
    {
        libvlc_audio_set_format(*this, format.c_str(), rate, channels);
    }

    /**
     * Selects an audio output module.
     *
     * \note Any change will take be effect only after playback is stopped
     * and restarted. Audio output cannot be changed while playing.
     *
     * \param name  name of audio output, use psz_name of
     *
     * \see AudioOutputDescription
     *
     * \return 0 if function succeded, -1 on error
     */
    int setAudioOutput(const std::string& name)
    {
        return libvlc_audio_output_set(*this, name.c_str());
    }

    /**
     * Gets a list of potential audio output devices,
     *
     * \see MediaPlayer::outputDeviceSet() .
     *
     * \note Not all audio outputs support enumerating devices. The audio
     * output may be functional even if the list is empty (NULL).
     *
     * \note The list may not be exhaustive.
     *
     * \warning Some audio output devices in the list might not actually work
     * in some circumstances. By default, it is recommended to not specify
     * any explicit audio device.
     *
     * \version LibVLC 2.2.0 or later.
     */
    std::vector<AudioOutputDeviceDescription> outputDeviceEnum()
    {
        auto devices = libvlc_audio_output_device_enum(*this);
        if ( devices == nullptr )
            return {};
        std::vector<AudioOutputDeviceDescription> res;
        std::unique_ptr<libvlc_audio_output_device_t, decltype(&libvlc_audio_output_device_list_release)>
                devicesPtr( devices, libvlc_audio_output_device_list_release);
        for ( auto* p = devices; p != nullptr; p = p->p_next )
            res.emplace_back( p );
        return res;
    }

    /**
     * Configures an explicit audio output device.
     *
     * If the module paramater is NULL, audio output will be moved to the
     * device specified by the device identifier string immediately. This is
     * the recommended usage.
     *
     * A list of adequate potential device strings can be obtained with
     * MediaPlayer::outputDeviceEnum() .
     *
     * However passing NULL is supported in LibVLC version 2.2.0 and later
     * only; in earlier versions, this function would have no effects when
     * the module parameter was NULL.
     *
     * If the module parameter is not NULL, the device parameter of the
     * corresponding audio output, if it exists, will be set to the specified
     * string. Note that some audio output modules do not have such a
     * parameter (notably MMDevice and PulseAudio).
     *
     * A list of adequate potential device strings can be obtained with
     * Instance::audioOutputDeviceList() .
     *
     * \note This function does not select the specified audio output plugin.
     * MediaPlayer::outputSet() is used for that purpose.
     *
     * \warning The syntax for the device parameter depends on the audio
     * output. Some audio output modules require further parameters (e.g. a
     * channels map in the case of ALSA).
     *
     * \param module  If NULL, current audio output module. if non-NULL, name
     * of audio output module (
     *
     * \see AudioOutputDescription )
     *
     * \param device_id  device identifier string
     *
     * \return Nothing. Errors are ignored (this is a design bug).
     */
    void outputDeviceSet(const std::string& device_id)
    {
        libvlc_audio_output_device_set(*this, device_id.c_str());
    }


    /**
     * Toggle mute status.
     *
     * \warning Toggling mute atomically is not always possible: On some
     * platforms, other processes can mute the VLC audio playback stream
     * asynchronously. Thus, there is a small race condition where toggling
     * will not work. See also the limitations of MediaPlayer::setMute() .
     */
    void toggleMute()
    {
        libvlc_audio_toggle_mute(*this);
    }

    /**
     * Get current mute status.
     */
    bool mute()
    {
        return libvlc_audio_get_mute( *this ) == 1;
    }

    /**
     * Set mute status.
     *
     * \param status  If status is true then mute, otherwise unmute
     *
     * \warning This function does not always work. If there are no active
     * audio playback stream, the mute status might not be available. If
     * digital pass-through (S/PDIF, HDMI...) is in use, muting may be
     * unapplicable. Also some audio output plugins do not support muting at
     * all.
     *
     * \note To force silent playback, disable all audio tracks. This is more
     * efficient and reliable than mute.
     */
    void setMute(bool mute)
    {
        libvlc_audio_set_mute( *this, (int)mute );
    }

    /**
     * Get current software audio volume.
     *
     * \return the software volume in percents (0 = mute, 100 = nominal /
     * 0dB)
     */
    int volume()
    {
        return libvlc_audio_get_volume(*this);
    }

    /**
     * Set current software audio volume.
     *
     * \param i_volume  the volume in percents (0 = mute, 100 = 0dB)
     */
    bool setVolume(int i_volume)
    {
        return libvlc_audio_set_volume(*this, i_volume) == 0;
    }

    /**
     * Get current audio stereo mode.
     *
     * \return the audio stereo mode
     */
    libvlc_audio_output_stereomode_t stereoMode()
    {
        return libvlc_audio_get_stereomode(*this);
    }

    /**
     * Set current audio stereo mode.
     *
     * \param mode the audio stereo mode
     */
    bool setStereoMode(libvlc_audio_output_stereomode_t mode)
    {
        return libvlc_audio_set_stereomode(*this, mode) == 0;
    }

    /**
     * Get current audio mix mode.
     *
     * \return the audio mix mode
     */
    libvlc_audio_output_mixmode_t mixMode()
    {
        return libvlc_audio_get_mixmode(*this);
    }

    /**
     * Set current audio mix mode.
     *
     * \param mode the audio mix mode
     */
    bool setMixMode(libvlc_audio_output_mixmode_t mode)
    {
        return libvlc_audio_set_mixmode(*this, mode) == 0;
    }

    /**
     * Get current audio delay.
     *
     * \return the audio delay (microseconds)
     *
     * \version LibVLC 1.1.1 or later
     */
    std::chrono::microseconds audioDelay()
    {
        return std::chrono::microseconds{ libvlc_audio_get_delay(*this) };
    }

    /**
     * Set current audio delay. The audio delay will be reset to zero each
     * time the media changes.
     *
     * \param i_delay  the audio delay (microseconds)
     *
     * \version LibVLC 1.1.1 or later
     */
    bool setAudioDelay(std::chrono::microseconds i_delay)
    {
        return libvlc_audio_set_delay(*this, i_delay.count()) == 0;
    }

    /**
     * Set callbacks and private data to render decoded video to a custom
     * area in memory. Use MediaPlayer::setFormat() or MediaPlayer::setFormatCallbacks()
     * to configure the decoded format.
     *
     * \param lock  callback to lock video memory (must not be nullptr)
     *              Expected prototype is void*(void** planes)
     *              planes is a pointer to an array of planes, that must be provided with some allocated buffers
     *              Return value is a picture identifier, to be passed to unlock & display callbacks
     *
     * \param unlock  callback to unlock video memory (or nullptr if not needed)
     *                Expected prototype is void(void* pictureId, void*const* planes);
     *                pictureId is the value returned by the lock callback.
     *                planes is the planes provided by the lock callback
     *
     * \param display callback to display video (or nullptr if not needed)
     *                Expected prototype is void(void* pictureId);
     *                pictureId is the value returned by the lock callback.
     *
     * \version LibVLC 1.1.1 or later
     */
    template <typename LockCb, typename UnlockCb, typename DisplayCb>
    void setVideoCallbacks(LockCb&& lock, UnlockCb&& unlock, DisplayCb&& display)
    {
        static_assert(signature_match<LockCb, void*(void**)>::value, "Mismatched lock callback signature");
        static_assert(signature_match_or_nullptr<UnlockCb, void(void*, void *const *)>::value, "Mismatched unlock callback signature");
        static_assert(signature_match_or_nullptr<DisplayCb, void(void*)>::value, "Mismatched lock callback signature");

        libvlc_video_set_callbacks(*this,
                CallbackWrapper<(unsigned int)CallbackIdx::VideoLock, libvlc_video_lock_cb>::wrap( *m_callbacks, std::forward<LockCb>( lock ) ),
                CallbackWrapper<(unsigned int)CallbackIdx::VideoUnlock, libvlc_video_unlock_cb>::wrap( *m_callbacks, std::forward<UnlockCb>( unlock ) ),
                CallbackWrapper<(unsigned int)CallbackIdx::VideoDisplay, libvlc_video_display_cb>::wrap( *m_callbacks, std::forward<DisplayCb>( display ) ),
                // We will receive the pointer as a void*, we need to offset the value *now*, otherwise we'd get
                // a shifted value, resulting in an empty callback array.
                m_callbacks.get() );
    }

    /**
     * Set decoded video chroma and dimensions. This only works in
     * combination with MediaPlayer::setCallbacks() , and is mutually exclusive
     * with MediaPlayer::setFormatCallbacks() .
     *
     * \param chroma  a four-characters string identifying the chroma (e.g.
     * "RV32" or "YUYV")
     *
     * \param width  pixel width
     *
     * \param height  pixel height
     *
     * \param pitch  line pitch (in bytes)
     *
     * \version LibVLC 1.1.1 or later
     */
    void setVideoFormat(const std::string& chroma, unsigned width, unsigned height, unsigned pitch)
    {
        libvlc_video_set_format(*this, chroma.c_str(), width, height, pitch);
    }

    /**
     * Set decoded video chroma and dimensions. This only works in
     * combination with MediaPlayer::setCallbacks() .
     *
     * \param setup  callback to lock video memory (must not be nullptr)
     *              Expected prototype is uint32_t(char *chroma,       // Must be filled with the required fourcc
     *                                              unsigned *width,   // Must be filled with the required width
     *                                              unsigned *height,  // Must be filled with the required height
     *                                              unsigned *pitches, // Must be filled with the required pitch for each plane
     *                                              unsigned *lines);  // Must be filled with the required number of lines for each plane
     *              The return value reprensent the amount of pictures to create in a pool. 0 to indicate an error
     *
     * \param cleanup  callback to release any allocated resources (or nullptr)
     *                 Expected prototype is void()
     *
     * \version LibVLC 2.0.0 or later
     */
    template <typename FormatCb, typename CleanupCb>
    void setVideoFormatCallbacks(FormatCb&& setup, CleanupCb&& cleanup)
    {
        static_assert(signature_match<FormatCb, uint32_t(char*, uint32_t*, uint32_t*, uint32_t*, uint32_t*)>::value,
                      "Unmatched prototype for format callback" );
        static_assert(signature_match_or_nullptr<CleanupCb, void()>::value, "Unmatched prototype for cleanup callback");

        libvlc_video_set_format_callbacks(*this,
                CallbackWrapper<(unsigned int)CallbackIdx::VideoFormat, libvlc_video_format_cb>::wrap( *m_callbacks, std::forward<FormatCb>( setup ) ),
                CallbackWrapper<(unsigned int)CallbackIdx::VideoCleanup, libvlc_video_cleanup_cb>::wrap( *m_callbacks, std::forward<CleanupCb>( cleanup ) ) );
    }

    /**
     * Enable or disable key press events handling, according to the LibVLC
     * hotkeys configuration. By default and for historical reasons, keyboard
     * events are handled by the LibVLC video widget.
     *
     * \note On X11, there can be only one subscriber for key press and mouse
     * click events per window. If your application has subscribed to those
     * events for the X window ID of the video widget, then LibVLC will not
     * be able to handle key presses and mouse clicks in any case.
     *
     * \warning This function is only implemented for X11 and Win32 at the
     * moment.
     *
     * \param on  true to handle key press events, false to ignore them.
     */
    void setKeyInput(bool enable)
    {
        libvlc_video_set_key_input(*this, (int)enable);
    }

    /**
     * Enable or disable mouse click events handling. By default, those
     * events are handled. This is needed for DVD menus to work, as well as a
     * few video filters such as "puzzle".
     *
     * \see MediaPlayer::setKeyInput() .
     *
     * \warning This function is only implemented for X11 and Win32 at the
     * moment.
     *
     * \param on  true to handle mouse click events, false to ignore them.
     */
    void setMouseInput(bool enable)
    {
        libvlc_video_set_mouse_input(*this, (int)enable);
    }

    /**
     * Get the pixel dimensions of a video.
     *
     * \param num  number of the video (starting from, and most commonly 0)
     *
     * \param px  pointer to get the pixel width [OUT]
     *
     * \param py  pointer to get the pixel height [OUT]
     */
    bool size(unsigned num, unsigned * px, unsigned * py)
    {
        return libvlc_video_get_size( *this, num, px, py ) == 0;
    }

    /**
     * Get the mouse pointer coordinates over a video. Coordinates are
     * expressed in terms of the decoded video resolution, in terms of pixels
     * on the screen/viewport (to get the latter, you can query your
     * windowing system directly).
     *
     * Either of the coordinates may be negative or larger than the
     * corresponding dimension of the video, if the cursor is outside the
     * rendering area.
     *
     * \warning The coordinates may be out-of-date if the pointer is not
     * located on the video rendering area. LibVLC does not track the pointer
     * if it is outside of the video widget.
     *
     * \note LibVLC does not support multiple pointers (it does of course
     * support multiple input devices sharing the same pointer) at the
     * moment.
     *
     * \param num  number of the video (starting from, and most commonly 0)
     *
     * \param px  pointer to get the abscissa [OUT]
     *
     * \param py  pointer to get the ordinate [OUT]
     */
    bool cursor(unsigned num, int * px, int * py)
    {
        return libvlc_video_get_cursor( *this, num, px, py ) == 0;
    }

    /**
     * Get the current video scaling factor. See also MediaPlayer::setScale() .
     *
     * \return the currently configured zoom factor, or 0. if the video is
     * set to fit to the output window/drawable automatically.
     */
    float scale()
    {
        return libvlc_video_get_scale(*this);
    }

    /**
     * Set the video scaling factor. That is the ratio of the number of
     * pixels on screen to the number of pixels in the original decoded video
     * in each dimension. Zero is a special value; it will adjust the video
     * to the output window/drawable (in windowed mode) or the entire screen.
     *
     * Note that not all video outputs support scaling.
     *
     * \param f_factor  the scaling factor, or zero
     */
    void setScale(float f_factor)
    {
        libvlc_video_set_scale(*this, f_factor);
    }

    /**
     * Get current video aspect ratio.
     *
     * \return the video aspect ratio or an empty string if unspecified.
     */
    std::string aspectRatio()
    {
        auto str = wrapCStr( libvlc_video_get_aspect_ratio(*this) );
        if ( str == nullptr )
            return {};
        return str.get();
    }

    /**
     * Set new video aspect ratio.
     *
     * \param psz_aspect  new video aspect-ratio or empty string to reset to default
     *
     * \note Invalid aspect ratios are ignored.
     */
    void setAspectRatio( const std::string& ar )
    {
        libvlc_video_set_aspect_ratio( *this, ar.size() > 0 ? ar.c_str() : nullptr );
    }

    /**
     * Get the current subtitle delay. Positive values means subtitles are
     * being displayed later, negative values earlier.
     *
     * \return time (in microseconds) the display of subtitles is being
     * delayed
     *
     * \version LibVLC 2.0.0 or later
     */
    std::chrono::microseconds spuDelay()
    {
        return std::chrono::microseconds{ libvlc_video_get_spu_delay(*this) };
    }

    /**
     * Set the subtitle delay. This affects the timing of when the subtitle
     * will be displayed. Positive values result in subtitles being displayed
     * later, while negative values will result in subtitles being displayed
     * earlier.
     *
     * The subtitle delay will be reset to zero each time the media changes.
     *
     * \param i_delay  time (in microseconds) the display of subtitles should
     * be delayed
     *
     * \return 0 on success, -1 on error
     *
     * \version LibVLC 2.0.0 or later
     */
    int setSpuDelay(std::chrono::microseconds i_delay)
    {
        return libvlc_video_set_spu_delay(*this, i_delay.count());
    }

    /**
     * Get the description of available titles.
     *
     * \return list containing description of available titles
     */
    std::vector<TitleDescription> titleDescription()
    {
        libvlc_title_description_t **titles;
        int nbTitles = libvlc_media_player_get_full_title_descriptions( *this, &titles);
        auto cleanupCb = [nbTitles]( libvlc_title_description_t** ts) {
            libvlc_title_descriptions_release( ts, nbTitles );
        };

        std::vector<TitleDescription> res;

        if ( nbTitles < 1 )
            return res;

        std::unique_ptr<libvlc_title_description_t*[], decltype(cleanupCb)> ptr( titles, cleanupCb );

        for ( int i = 0; i < nbTitles; ++i )
            res.emplace_back( ptr[i] );
        return res;
    }

    /**
     * Get the description of available chapters for specific title.
     *
     * \param i_title  selected title
     *
     * \return list containing description of available chapters for title
     * i_title
     */
    std::vector<ChapterDescription> chapterDescription(int i_title)
    {
        libvlc_chapter_description_t **chapters;
        int nbChapters = libvlc_media_player_get_full_chapter_descriptions( *this, i_title, &chapters );
        auto cleanupCb = [nbChapters](libvlc_chapter_description_t** cs) {
            libvlc_chapter_descriptions_release( cs, nbChapters );
        };

        std::vector<ChapterDescription> res;

        if ( nbChapters < 1 )
            return res;

        std::unique_ptr<libvlc_chapter_description_t*[], decltype(cleanupCb)> ptr( chapters, cleanupCb );

        for ( int i = 0; i < nbChapters; ++i )
            res.emplace_back( ptr[i] );
        return res;
    }

    /**
     * Set/unset the video crop ratio.
     *
     * This function forces a crop ratio on any and all video tracks rendered by
     * the media player. If the display aspect ratio of a video does not match the
     * crop ratio, either the top and bottom, or the left and right of the video
     * will be cut out to fit the crop ratio.
     *
     * For instance, a ratio of 1:1 will force the video to a square shape.
     *
     * To disable video crop, set a crop ratio with zero as denominator.
     *
     * A call to this function overrides any previous call to any of
     * setCropRatio(), setCropBorder() and/or setCropWindow().
     *
     * \see setAspectRatio()
     *
     * \param num crop ratio numerator (ignored if denominator is 0)
     * \param den crop ratio denominator (or 0 to unset the crop ratio)
     *
     * \version LibVLC 4.0.0 and later
     */
    void setCropRatio( uint32_t num, uint32_t den )
    {
        libvlc_video_set_crop_ratio( *this, num, den );
    }

    /**
     * Set the video crop window.
     *
     * This function selects a sub-rectangle of video to show. Any pixels outsid
     * the rectangle will not be shown.
     *
     * To unset the video crop window, use setCropRatio() or
     * setCropBorder().
     *
     * A call to this function overrides any previous call to any of
     * setCropRatio(), setCropBorder() and/or setCropWindow().
     *
     * \param x abscissa (i.e. leftmost sample column offset) of the crop window
     * \param y ordinate (i.e. topmost sample row offset) of the crop window
     * \param width sample width of the crop window (cannot be zero)
     * \param height sample height of the crop window (cannot be zero)
     *
     * \version LibVLC 4.0.0 and later
     */
    void setCropWindow( uint32_t x, uint32_t y, uint32_t width, uint32_t height )
    {
        libvlc_video_set_crop_window( *this, x, y, width, height );
    }

    /**
     * Set the video crop borders.
     *
     * This function selects the size of video edges to be cropped out.
     *
     * To unset the video crop borders, set all borders to zero.
     *
     * A call to this function overrides any previous call to any of
     * setCropRatio(), setCropBorder() and/or setCropWindow().
     *
     * \param left number of sample columns to crop on the left
     * \param right number of sample columns to crop on the right
     * \param top number of sample rows to crop on the top
     * \param bottom number of sample rows to corp on the bottom
     *
     * \version LibVLC 4.0.0 and later
     */
    void setCropBorder( uint32_t left, uint32_t right, uint32_t top, uint32_t bottom )
    {
        libvlc_video_set_crop_border( *this, left, right, top, bottom );
    }

    /**
     * Get current teletext page requested.
     *
     * \return the current teletext page requested.
     */
    int teletext()
    {
        return libvlc_video_get_teletext(*this);
    }

    /**
     * Set new teletext page to retrieve.
     *
     * \param i_page  teletex page number requested
     */
    void setTeletext(int i_page)
    {
        libvlc_video_set_teletext(*this, i_page);
    }


    /**
     * Take a snapshot of the current video window.
     *
     * If i_width AND i_height is 0, original size is used. If i_width XOR
     * i_height is 0, original aspect-ratio is preserved.
     *
     * \param num  number of video output (typically 0 for the first/only
     * one)
     *
     * \param filepath  the path where to save the screenshot to
     *
     * \param i_width  the snapshot's width
     *
     * \param i_height  the snapshot's height
     */
    bool takeSnapshot(unsigned num, const std::string& filepath, unsigned int i_width, unsigned int i_height)
    {
        return libvlc_video_take_snapshot(*this, num, filepath.c_str(), i_width, i_height) == 0;
    }

    /**
     * Enable or disable deinterlace filter
     *
     * \version{2.x}
     * \version{3.x}
     * \param psz_mode  type of deinterlace filter, empty string to disable.
     * \version{4.x}
     * \param state     The required deinterlacing state.
     * \param mode      The deinterlace mode, or empty string for the current
     *                  or default filter.
     */
    void setDeinterlace(DeinterlaceState state, const std::string& mode)
    {
        libvlc_video_set_deinterlace(*this, static_cast<int>( state ),
                                     mode.empty() ? nullptr: mode.c_str());
    }

    /**
     * Get an integer marquee option value
     *
     * \param option  marq option to get
     *
     * \see libvlc_video_marquee_int_option_t
     */
    int marqueeInt(unsigned option)
    {
        return libvlc_video_get_marquee_int(*this, option);
    }

    /**
     * Enable, disable or set an integer marquee option
     *
     * Setting libvlc_marquee_Enable has the side effect of enabling (arg !0)
     * or disabling (arg 0) the marq filter.
     *
     * \param option  marq option to set
     *
     * \see libvlc_video_marquee_int_option_t
     *
     * \param i_val  marq option value
     */
    void setMarqueeInt(unsigned option, int i_val)
    {
        libvlc_video_set_marquee_int(*this, option, i_val);
    }

    /**
     * Set a marquee string option
     *
     * \param option  marq option to set
     *
     * \see libvlc_video_marquee_string_option_t
     *
     * \param text  marq option value
     */
    void setMarqueeString(unsigned option, const std::string& text)
    {
        libvlc_video_set_marquee_string(*this, option, text.c_str());
    }

    /**
     * Get integer logo option.
     *
     * \param option  logo option to get, values of
     * libvlc_video_logo_option_t
     */
    int logoInt(unsigned option)
    {
        return libvlc_video_get_logo_int(*this, option);
    }

    /**
     * Set logo option as integer. Options that take a different type value
     * are ignored. Passing libvlc_logo_enable as option value has the side
     * effect of starting (arg !0) or stopping (arg 0) the logo filter.
     *
     * \param option  logo option to set, values of
     * libvlc_video_logo_option_t
     *
     * \param value  logo option value
     */
    void setLogoInt(unsigned option, int value)
    {
        libvlc_video_set_logo_int(*this, option, value);
    }

    /**
     * Set logo option as string. Options that take a different type value
     * are ignored.
     *
     * \param option  logo option to set, values of
     * libvlc_video_logo_option_t
     *
     * \param psz_value  logo option value
     */
    void setLogoString(unsigned option, const std::string& value)
    {
        libvlc_video_set_logo_string(*this, option, value.c_str());
    }

    /**
     * Get integer adjust option.
     *
     * \param option  adjust option to get, values of
     * libvlc_video_adjust_option_t
     *
     * \version LibVLC 1.1.1 and later.
     */
    int adjustInt(unsigned option)
    {
        return libvlc_video_get_adjust_int(*this, option);
    }

    /**
     * Set adjust option as integer. Options that take a different type value
     * are ignored. Passing libvlc_adjust_enable as option value has the side
     * effect of starting (arg !0) or stopping (arg 0) the adjust filter.
     *
     * \param option  adust option to set, values of
     * libvlc_video_adjust_option_t
     *
     * \param value  adjust option value
     *
     * \version LibVLC 1.1.1 and later.
     */
    void setAdjustInt(unsigned option, int value)
    {
        libvlc_video_set_adjust_int(*this, option, value);
    }

    /**
     * Get float adjust option.
     *
     * \param option  adjust option to get, values of
     * libvlc_video_adjust_option_t
     *
     * \version LibVLC 1.1.1 and later.
     */
    float adjustFloat(unsigned option)
    {
        return libvlc_video_get_adjust_float(*this, option);
    }

    /**
     * Set adjust option as float. Options that take a different type value
     * are ignored.
     *
     * \param option  adust option to set, values of
     * libvlc_video_adjust_option_t
     *
     * \param value  adjust option value
     *
     * \version LibVLC 1.1.1 and later.
     */
    void setAdjustFloat(unsigned option, float value)
    {
        libvlc_video_set_adjust_float(*this, option, value);
    }

    /**
     * Add a slave to the current media player.
     *
     * \note If the player is playing, the slave will be added directly. This call
     * will also update the slave list of the attached VLC::Media.
     *
     * \version LibVLC 3.0.0 and later.
     *
     * \see Media::addSlave
     *
     * \param type subtitle or audio
     * \param uri Uri of the slave (should contain a valid scheme).
     * \param select True if this slave should be selected when it's loaded
     *
     * \return true on success, false on error.
     */
    bool addSlave( MediaSlave::Type type, const std::string& uri, bool select )
    {
        return libvlc_media_player_add_slave( *this,
                        static_cast<libvlc_media_slave_type_t>( type ), uri.c_str(), select ) == 0;
    }

    bool updateViewpoint( const VideoViewpoint& viewpoint, bool b_absolute )
    {
        return libvlc_video_update_viewpoint( *this,
            static_cast<const libvlc_video_viewpoint_t*>( &viewpoint ), b_absolute ) == 0;
    }

    bool setRenderer( const RendererDiscoverer::Item& rendererItem )
    {
        return libvlc_media_player_set_renderer( *this, rendererItem ) == 0;
    }

    void unsetRenderer()
    {
        libvlc_media_player_set_renderer( *this, nullptr );
    }

    std::vector<MediaTrack> tracks( MediaTrack::Type type, bool selected )
    {
        using TrackListPtr = std::unique_ptr<libvlc_media_tracklist_t,
                                decltype(&libvlc_media_tracklist_delete)>;
        TrackListPtr trackList{ libvlc_media_player_get_tracklist( *this,
                                    static_cast<libvlc_track_type_t>( type ),
                                    selected ),
                                &libvlc_media_tracklist_delete };
        if ( trackList == nullptr )
            return {};
        auto count = libvlc_media_tracklist_count( trackList.get() );
        std::vector<MediaTrack> res{};
        for ( auto i = 0u; i < count; ++i )
            res.emplace_back( libvlc_media_tracklist_at( trackList.get(), i ) );
        return res;
    }

    void selectTracks( MediaTrack::Type type, const std::vector<MediaTrack>& tracks )
    {
        std::vector<const libvlc_media_track_t*> ctracks{};
        ctracks.reserve( tracks.size() );
        for ( const auto& mt : tracks )
            ctracks.push_back( mt.get() );
        libvlc_media_player_select_tracks( *this,
                                           static_cast<libvlc_track_type_t>( type ),
                                           ctracks.data(),
                                           ctracks.size() );
    }

    MediaTrack getTrackFromId(const std::string& id)
    {
        auto track = libvlc_media_player_get_track_from_id( *this, id.c_str() );
        if ( track == nullptr )
            throw std::runtime_error("Track not found");
        return MediaTrack{ track };
    }

    void selectTrack( const MediaTrack& track )
    {
        libvlc_media_player_select_track( *this, track );
    }

    void unselectTrackType( MediaTrack::Type type )
    {
        libvlc_media_player_unselect_track_type( *this,
                                    static_cast<libvlc_track_type_t>( type ) );
    }
};

} // namespace VLC

#endif
