/*****************************************************************************
* MediaPlayerCX.hpp: MediaPlayer API
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

#include <collection.h>
#include "StructuresCX.hpp"
#include "MediaCX.hpp"
#include <vlcpp/vlc.hpp>

namespace libVLCX
{
    ref class MediaPlayerEventManager;
    ref class AudioOutputDeviceDescription;
    ref class TrackDescription;
    ref class Instance;
    ref class Media;
    ref class Equalizer;

    public enum class TitlePosition
    {
        Disable = -1,
        Center,
        Left,
        Right,
        Top,
        TopLeft,
        TopRight,
        Bottom,
        BottomLeft,
        BottomRight
    };

    public ref class MediaPlayer sealed
    {
    public:
        MediaPlayer(Instance^ instance);

        // libvlc_media_player_new_from_media
        /**
        * Create a Media Player object from a Media
        *
        * \param p_md  the media. Afterwards the p_md can be safely destroyed.
        */
        MediaPlayer(Media^ md);

        /**
        * Set the media that will be used by the media_player. If any, previous
        * md will be released.
        *
        * \param p_md  the Media. Afterwards the p_md can be safely destroyed.
        */
        void setMedia(Media^ md);

        /**
        * Get the media used by the media_player.
        *
        * \return the media associated with p_mi, or NULL if no media is
        * associated
        */
        Media^ media();

        /**
        * Get the Event Manager from which the media player send event.
        *
        * \return the event manager associated with p_mi
        */
        MediaPlayerEventManager^ eventManager();

        /**
        * is_playing
        *
        * \return 1 if the media player is playing, 0 otherwise
        */
        bool isPlaying();

        /**
        * Play
        *
        * \return 0 if playback started (and was already started), or -1 on
        * error.
        */
        int play();

        /**
        * Pause or resume (no effect if there is no media)
        *
        * \param do_pause  play/resume if zero, pause if non-zero
        *
        * \version LibVLC 1.1.1 or later
        */
        void setPause(int do_pause);

        /**
        * Toggle pause (no effect if there is no media)
        */
        void pause();

        /**
        * Stop (no effect if there is no media)
        */
        void stop();

        /**
        * Set a Win32/Win64 API window handle (HWND) where the media player
        * should render its video output. If LibVLC was built without
        * Win32/Win64 API output support, then this has no effects.
        *
        * \param drawable  windows handle of the drawable
        */
        void setHwnd(uint64 drawable);

        /**
        * Get the Windows API window handle (HWND) previously set with
        * MediaPlayer::setHwnd() . The handle will be returned even if LibVLC is
        * not currently outputting any video to it.
        *
        * \return a window handle or NULL if there are none.
        */
        uint64 hwnd();

        /**
        * Get the current movie length (in ms).
        *
        * \return the movie length (in ms), or -1 if there is no media.
        */
        libvlc_time_t length();

        /**
        * Get the current movie time (in ms).
        *
        * \return the movie time (in ms), or -1 if there is no media.
        */
        libvlc_time_t time();

        /**
        * Set the movie time (in ms). This has no effect if no media is being
        * played. Not all formats and protocols support this.
        *
        * \param i_time  the movie time (in ms).
        */
        void setTime(libvlc_time_t i_time);

        /**
        * Get movie position as percentage between 0.0 and 1.0.
        *
        * \return movie position, or -1. in case of error
        */
        float position();

        /**
        * Set movie position as percentage between 0.0 and 1.0. This has no
        * effect if playback is not enabled. This might not work depending on
        * the underlying input format and protocol.
        *
        * \param f_pos  the position
        */
        void setPosition(float f_pos);

        /**
        * Set movie chapter (if applicable).
        *
        * \param i_chapter  chapter number to play
        */
        void setChapter(int i_chapter);

        /**
        * Get movie chapter.
        *
        * \return chapter number currently playing, or -1 if there is no media.
        */
        int chapter();

        /**
        * Get movie chapter count
        *
        * \return number of chapters in movie, or -1.
        */
        int chapterCount();

        /**
        * Is the player able to play
        *
        * \return boolean
        */
        bool willPlay();

        /**
        * Get title chapter count
        *
        * \param i_title  title
        *
        * \return number of chapters in title, or -1
        */
        int chapterCountForTitle(int i_title);

        /**
        * Set movie title
        *
        * \param i_title  title number to play
        */
        void setTitle(int i_title);

        /**
        * Get movie title
        *
        * \return title number currently playing, or -1
        */
        int title();

        /**
        * Get movie title count
        *
        * \return title number count, or -1
        */
        int titleCount();

        /**
        * Set previous chapter (if applicable)
        */
        void previousChapter();

        /**
        * Set next chapter (if applicable)
        */
        void nextChapter();

        /**
        * Get the requested movie play rate.
        *
        * \warning Depending on the underlying media, the requested rate may be
        * different from the real playback rate.
        *
        * \return movie play rate
        */
        float rate();

        /**
        * Set movie play rate
        *
        * \param rate  movie play rate to set
        *
        * \return -1 if an error was detected, 0 otherwise (but even then, it
        * might not actually work depending on the underlying media protocol)
        */
        int setRate(float rate);

        /**
        * Get current movie state
        *
        * \return the current state of the media player (playing, paused, ...)
        *
        * \see libvlc_state_t
        */
        MediaState state();

        /**
        * end bug How many video outputs does this media player have?
        *
        * \return the number of video outputs
        */
        unsigned hasVout();

        /**
        * Is this media player seekable?
        *
        * \return true if the media player can seek
        */
        bool isSeekable();

        /**
        * Can this media player be paused?
        *
        * \return true if the media player can pause
        */
        bool canPause();

        /**
        * Check if the current program is scrambled
        *
        * \return true if the current program is scrambled
        *
        * \version LibVLC 2.2.0 or later
        */
        bool programScrambled();

        /**
        * Display the next frame (if supported)
        */
        void nextFrame();

        /**
        * Navigate through DVD Menu
        *
        * \param navigate  the Navigation mode
        *
        * \version libVLC 2.0.0 or later
        */
        void navigate(unsigned int nav);

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
        void setVideoTitleDisplay(TitlePosition position, unsigned int timeout);

        /**
        * Toggle fullscreen status on non-embedded video outputs.
        *
        * \warning The same limitations applies to this function as to
        * MediaPlayer::setFullscreen() .
        */
        void toggleFullscreen();

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
        void setFullscreen(int b_fullscreen);

        /**
        * Get current fullscreen status.
        *
        * \return the fullscreen status (boolean)
        */
        bool fullscreen();

        /**
        * Toggle teletext transparent status on video output.
        */
        void toggleTeletext();

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
        * \param equalizer  opaque equalizer handle, or NULL to disable the
        * equalizer for this media player
        *
        * \return zero on success, -1 on error
        *
        * \version LibVLC 2.2.0 or later
        */
        int setEqualizer(Equalizer^ equalizer);

        ///
        /// \brief unsetEqualizer disable equalizer for this media player
        ///
        /// \return true on success, false otherwise.
        ///
        bool unsetEqualizer();

        /**
        * Set callbacks and private data for decoded audio. Use
        * MediaPlayer::setFormat() or MediaPlayer::setFormatCallbacks() to configure the
        * decoded audio format.
        *
        * \param play  callback to play audio samples (must not be NULL)
        *
        * \param pause  callback to pause playback (or NULL to ignore)
        *
        * \param resume  callback to resume playback (or NULL to ignore)
        *
        * \param flush  callback to flush audio buffers (or NULL to ignore)
        *
        * \param drain  callback to drain audio buffers (or NULL to ignore)
        *
        * \param opaque  private pointer for the audio callbacks (as first
        * parameter)
        *
        * \version LibVLC 2.0.0 or later
        */
        //void setAudioCallbacks(libvlc_audio_play_cb play, libvlc_audio_pause_cb pause, libvlc_audio_resume_cb resume, libvlc_audio_flush_cb flush, libvlc_audio_drain_cb drain, void * opaque);

        /**
        * Set callbacks and private data for decoded audio. Use
        * MediaPlayer::setFormat() or MediaPlayer::setFormatCallbacks() to configure the
        * decoded audio format.
        *
        * \param set_volume  callback to apply audio volume, or NULL to apply
        * volume in software
        *
        * \version LibVLC 2.0.0 or later
        */
        //void setVolumeCallback(libvlc_audio_set_volume_cb set_volume);

        /**
        * Set decoded audio format. This only works in combination with
        * MediaPlayer::setCallbacks() .
        *
        * \param setup  callback to select the audio format (cannot be NULL)
        *
        * \param cleanup  callback to release any allocated resources (or NULL)
        *
        * \version LibVLC 2.0.0 or later
        */
        //void setAudioFormatCallbacks(libvlc_audio_setup_cb setup, libvlc_audio_cleanup_cb cleanup);

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
        //void setAudioFormat(Platform::String^ format, unsigned rate, unsigned channels);

        /**
        * Selects an audio output module.
        *
        * \note Any change will take be effect only after playback is stopped
        * and restarted. Audio output cannot be changed while playing.
        *
        * \param psz_name  name of audio output, use psz_name of
        *
        * \see AudioOutputDescription
        *
        * \return 0 if function succeded, -1 on error
        */
        int setAudioOutput(Platform::String^ psz_name);

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
        * \return A NULL-terminated linked list of potential audio output
        * devices. It must be freed it with
        * libvlc_audio_output_device_list_release()
        *
        * \version LibVLC 2.2.0 or later.
        */
        Windows::Foundation::Collections::IVector<AudioOutputDeviceDescription^>^ outputDeviceEnum();

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
        void outputDeviceSet(Platform::String^ device_id);

        /**
        * Toggle mute status.
        *
        * \warning Toggling mute atomically is not always possible: On some
        * platforms, other processes can mute the VLC audio playback stream
        * asynchronously. Thus, there is a small race condition where toggling
        * will not work. See also the limitations of MediaPlayer::setMute() .
        */
        void toggleMute();

        /**
        * Get current mute status.
        *
        * \return the mute status (boolean) if defined, -1 if
        * undefined/unapplicable
        */
        int mute();

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
        void setMute(int status);

        /**
        * Get current software audio volume.
        *
        * \return the software volume in percents (0 = mute, 100 = nominal /
        * 0dB)
        */
        int volume();

        /**
        * Set current software audio volume.
        *
        * \param i_volume  the volume in percents (0 = mute, 100 = 0dB)
        *
        * \return 0 if the volume was set, -1 if it was out of range
        */
        int setVolume(int i_volume);

        /**
        * Get number of available audio tracks.
        *
        * \return the number of available audio tracks (int), or -1 if
        * unavailable
        */
        int audioTrackCount();

        /**
        * Get the description of available audio tracks.
        *
        * \return list with description of available audio tracks, or NULL
        */
        Windows::Foundation::Collections::IVector<TrackDescription^>^ audioTrackDescription();

        /**
        * Get current audio track.
        *
        * \return the audio track ID or -1 if no active input.
        */
        int audioTrack();

        /**
        * Set current audio track.
        *
        * \param i_track  the track ID (i_id field from track description)
        *
        * \return 0 on success, -1 on error
        */
        int setAudioTrack(int i_track);

        /**
        * Get current audio channel.
        *
        * \return the audio channel
        *
        * \see libvlc_audio_output_channel_t
        */
        int channel();

        /**
        * Set current audio channel.
        *
        * \param channel  the audio channel,
        *
        * \see libvlc_audio_output_channel_t
        *
        * \return 0 on success, -1 on error
        */
        int setChannel(int channel);

        /**
        * Get current audio delay.
        *
        * \return the audio delay (microseconds)
        *
        * \version LibVLC 1.1.1 or later
        */
        int64_t audioDelay();

        /**
        * Set current audio delay. The audio delay will be reset to zero each
        * time the media changes.
        *
        * \param i_delay  the audio delay (microseconds)
        *
        * \return 0 on success, -1 on error
        *
        * \version LibVLC 1.1.1 or later
        */
        int setAudioDelay(int64_t i_delay);

        /**
        * Set callbacks and private data to render decoded video to a custom
        * area in memory. Use MediaPlayer::setFormat() or MediaPlayer::setFormatCallbacks()
        * to configure the decoded format.
        *
        * \param lock  callback to lock video memory (must not be NULL)
        *
        * \param unlock  callback to unlock video memory (or NULL if not needed)
        *
        * \param display  callback to display video (or NULL if not needed)
        *
        * \param opaque  private pointer for the three callbacks (as first
        * parameter)
        *
        * \version LibVLC 1.1.1 or later
        */
        //void setVideoCallbacks(libvlc_video_lock_cb lock, libvlc_video_unlock_cb unlock, libvlc_video_display_cb display, void * opaque);

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
        //void setVideoFormat(Platform::String^ chroma, unsigned width, unsigned height, unsigned pitch);

        /**
        * Set decoded video chroma and dimensions. This only works in
        * combination with MediaPlayer::setCallbacks() .
        *
        * \param setup  callback to select the video format (cannot be NULL)
        *
        * \param cleanup  callback to release any allocated resources (or NULL)
        *
        * \version LibVLC 2.0.0 or later
        */
        //void setVideoFormatCallbacks(libvlc_video_format_cb setup, libvlc_video_cleanup_cb cleanup);

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
        void setKeyInput(unsigned on);

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
        void setMouseInput(unsigned on);

        /**
        * Get the pixel dimensions of a video.
        *
        * \param num  number of the video (starting from, and most commonly 0)
        *
        * \param px  pointer to get the pixel width [OUT]
        *
        * \param py  pointer to get the pixel height [OUT]
        *
        * \return 0 on success, -1 if the specified video does not exist
        */
        int size(unsigned num, unsigned * px, unsigned * py);

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
        *
        * \return 0 on success, -1 if the specified video does not exist
        */
        int cursor(unsigned num, int * px, int * py);

        /**
        * Get the current video scaling factor. See also MediaPlayer::setScale() .
        *
        * \return the currently configured zoom factor, or 0. if the video is
        * set to fit to the output window/drawable automatically.
        */
        float scale();

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
        void setScale(float f_factor);

        /**
        * Get current video aspect ratio.
        *
        * \return the video aspect ratio or NULL if unspecified (the result must
        * be released with free() or libvlc_free() ).
        */
        Platform::String^ aspectRatio();

        /**
        * Set new video aspect ratio.
        *
        * \param psz_aspect  new video aspect-ratio or NULL to reset to default
        *
        * \note Invalid aspect ratios are ignored.
        */
        void setAspectRatio(Platform::String^ psz_aspect);

        /**
        * Get current video subtitle.
        *
        * \return the video subtitle selected, or -1 if none
        */
        int spu();

        /**
        * Get the number of available video subtitles.
        *
        * \return the number of available video subtitles
        */
        int spuCount();

        /**
        * Get the description of available video subtitles.
        *
        * \return list containing description of available video subtitles
        */
        Windows::Foundation::Collections::IVector<TrackDescription^>^ spuDescription();

        /**
        * Set new video subtitle.
        *
        * \param i_spu  video subtitle track to select (i_id from track
        * description)
        *
        * \return 0 on success, -1 if out of range
        */
        int setSpu(int i_spu);

        bool addSlave(SlaveType type, Platform::String^ uri, bool select);

        /**
        * Get the current subtitle delay. Positive values means subtitles are
        * being displayed later, negative values earlier.
        *
        * \return time (in microseconds) the display of subtitles is being
        * delayed
        *
        * \version LibVLC 2.0.0 or later
        */
        int64_t spuDelay();

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
        int setSpuDelay(int64_t i_delay);

        /**
        * Get the description of available titles.
        *
        * \return list containing description of available titles
        */
        Windows::Foundation::Collections::IVector<TitleDescription^>^ titleDescription();

        /**
        * Get the description of available chapters for specific title.
        *
        * \param i_title  selected title
        *
        * \return list containing description of available chapter for title
        * i_title
        */
        Windows::Foundation::Collections::IVector<ChapterDescription^>^ chapterDescription(int i_title);

        /**
        * Get current crop filter geometry.
        *
        * \return the crop filter geometry or NULL if unset
        */
        Platform::String^ cropGeometry();

        /**
        * Set new crop filter geometry.
        *
        * \param psz_geometry  new crop filter geometry (NULL to unset)
        */
        void setCropGeometry(Platform::String^ psz_geometry);

        /**
        * Get current teletext page requested.
        *
        * \return the current teletext page requested.
        */
        int teletext();

        /**
        * Set new teletext page to retrieve.
        *
        * \param i_page  teletex page number requested
        */
        void setTeletext(int i_page);

        /**
        * Get number of available video tracks.
        *
        * \return the number of available video tracks (int)
        */
        int videoTrackCount();

        /**
        * Get the description of available video tracks.
        *
        * \return list with description of available video tracks, or NULL on
        * error
        */
        Windows::Foundation::Collections::IVector<TrackDescription^>^ videoTrackDescription();

        /**
        * Get current video track.
        *
        * \return the video track ID (int) or -1 if no active input
        */
        int videoTrack();

        /**
        * Set video track.
        *
        * \param i_track  the track ID (i_id field from track description)
        *
        * \return 0 on success, -1 if out of range
        */
        int setVideoTrack(int i_track);

        /**
        * Take a snapshot of the current video window.
        *
        * If i_width AND i_height is 0, original size is used. If i_width XOR
        * i_height is 0, original aspect-ratio is preserved.
        *
        * \param num  number of video output (typically 0 for the first/only
        * one)
        *
        * \param psz_filepath  the path where to save the screenshot to
        *
        * \param i_width  the snapshot's width
        *
        * \param i_height  the snapshot's height
        *
        * \return 0 on success, -1 if the video was not found
        */
        int takeSnapshot(unsigned num, Platform::String^ psz_filepath, unsigned int i_width, unsigned int i_height);

        /**
        * Enable or disable deinterlace filter
        *
        * \param psz_mode  type of deinterlace filter, NULL to disable
        */
        void setDeinterlace(Platform::String^ psz_mode);

        /**
        * Get an integer marquee option value
        *
        * \param option  marq option to get
        *
        * \see libvlc_video_marquee_int_option_t
        */
        int marqueeInt(unsigned option);

        /**
        * Get a string marquee option value
        *
        * \param option  marq option to get
        *
        * \see libvlc_video_marquee_string_option_t
        */
        Platform::String^ marqueeString(unsigned option);

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
        void setMarqueeInt(unsigned option, int i_val);

        /**
        * Set a marquee string option
        *
        * \param option  marq option to set
        *
        * \see libvlc_video_marquee_string_option_t
        *
        * \param psz_text  marq option value
        */
        void setMarqueeString(unsigned option, Platform::String^ psz_text);

        /**
        * Get integer logo option.
        *
        * \param option  logo option to get, values of
        * libvlc_video_logo_option_t
        */
        int logoInt(unsigned option);

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
        void setLogoInt(unsigned option, int value);

        /**
        * Set logo option as string. Options that take a different type value
        * are ignored.
        *
        * \param option  logo option to set, values of
        * libvlc_video_logo_option_t
        *
        * \param psz_value  logo option value
        */
        void setLogoString(unsigned option, Platform::String^ psz_value);

        /**
        * Get integer adjust option.
        *
        * \param option  adjust option to get, values of
        * libvlc_video_adjust_option_t
        *
        * \version LibVLC 1.1.1 and later.
        */
        int adjustInt(unsigned option);

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
        void setAdjustInt(unsigned option, int value);

        /**
        * Get float adjust option.
        *
        * \param option  adjust option to get, values of
        * libvlc_video_adjust_option_t
        *
        * \version LibVLC 1.1.1 and later.
        */
        float adjustFloat(unsigned option);

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
        void setAdjustFloat(unsigned option, float value);

    private:
        ~MediaPlayer(){}

    private:
        VLC::MediaPlayer m_mp;
        MediaPlayerEventManager^ m_eventManager;

    };

} // namespace VLC

