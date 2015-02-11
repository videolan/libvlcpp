/*****************************************************************************
 * libvlc_MediaPlayer.cpp: MediaPlayer implementation
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

#include <stdexcept>
#include <memory>

namespace VLC
{

bool MediaPlayer::operator==(const MediaPlayer& another) const
{
    return m_obj == another.m_obj;
}

MediaPlayer::MediaPlayer( InstancePtr instance )
    : Internal{ libvlc_media_player_new( instance->get() ), libvlc_media_player_release }
{
}

MediaPlayer::MediaPlayer( MediaPtr md )
    : Internal{ libvlc_media_player_new_from_media( md->get() ), libvlc_media_player_release }
{
}

void MediaPlayer::setMedia( MediaPtr md )
{
    libvlc_media_player_set_media( get(), md->get() );
}

MediaPtr MediaPlayer::media()
{
    libvlc_media_t* media = libvlc_media_player_get_media(get());
    return std::make_shared<Media>( media, true );
}

EventManagerPtr MediaPlayer::eventManager()
{
    if ( m_eventManager == NULL )
    {
        libvlc_event_manager_t* obj = libvlc_media_player_event_manager( get() );
        m_eventManager = std::make_shared<EventManager>( obj );
    }
    return m_eventManager;
}

bool MediaPlayer::isPlaying()
{
    return libvlc_media_player_is_playing(get()) != 0;
}

int MediaPlayer::play()
{
    return libvlc_media_player_play(get());
}

void MediaPlayer::setPause(int do_pause)
{
    libvlc_media_player_set_pause(get(), do_pause);
}

void MediaPlayer::pause()
{
    libvlc_media_player_pause(get());
}

void MediaPlayer::stop()
{
    libvlc_media_player_stop(get());
}

void MediaPlayer::setNsobject(void * drawable)
{
    libvlc_media_player_set_nsobject(get(), drawable);
}

void * MediaPlayer::nsobject()
{
    return libvlc_media_player_get_nsobject(get());
}

void MediaPlayer::setAgl(uint32_t drawable)
{
    libvlc_media_player_set_agl(get(), drawable);
}

uint32_t MediaPlayer::agl()
{
    return libvlc_media_player_get_agl(get());
}

void MediaPlayer::setXwindow(uint32_t drawable)
{
    libvlc_media_player_set_xwindow(get(), drawable);
}

uint32_t MediaPlayer::xwindow()
{
    return libvlc_media_player_get_xwindow(get());
}

void MediaPlayer::setHwnd(void * drawable)
{
    libvlc_media_player_set_hwnd(get(), drawable);
}

void * MediaPlayer::hwnd()
{
    return libvlc_media_player_get_hwnd(get());
}

libvlc_time_t MediaPlayer::length()
{
    return libvlc_media_player_get_length(get());
}

libvlc_time_t MediaPlayer::time()
{
    return  libvlc_media_player_get_time(get());
}

void MediaPlayer::setTime(libvlc_time_t i_time)
{
    libvlc_media_player_set_time(get(), i_time);
}

float MediaPlayer::position()
{
    return libvlc_media_player_get_position(get());
}

void MediaPlayer::setPosition(float f_pos)
{
    libvlc_media_player_set_position(get(), f_pos);
}

void MediaPlayer::setChapter(int i_chapter)
{
    libvlc_media_player_set_chapter(get(), i_chapter);
}

int MediaPlayer::chapter()
{
    return libvlc_media_player_get_chapter(get());
}

int MediaPlayer::chapterCount()
{
    return libvlc_media_player_get_chapter_count(get());
}

bool MediaPlayer::willPlay()
{
    return libvlc_media_player_will_play(get()) != 0;
}

int MediaPlayer::chapterCountForTitle(int i_title)
{
    return libvlc_media_player_get_chapter_count_for_title(get(), i_title);
}

void MediaPlayer::setTitle(int i_title)
{
    libvlc_media_player_set_title(get(), i_title);
}

int MediaPlayer::title()
{
    return libvlc_media_player_get_title(get());
}

int MediaPlayer::titleCount()
{
    return libvlc_media_player_get_title_count(get());
}

void MediaPlayer::previousChapter()
{
    libvlc_media_player_previous_chapter(get());
}

void MediaPlayer::nextChapter()
{
    libvlc_media_player_next_chapter(get());
}

float MediaPlayer::rate()
{
    return libvlc_media_player_get_rate(get());
}

int MediaPlayer::setRate(float rate)
{
    return libvlc_media_player_set_rate(get(), rate);
}

libvlc_state_t MediaPlayer::state()
{
    return libvlc_media_player_get_state(get());
}

float MediaPlayer::fps()
{
    return libvlc_media_player_get_fps(get());
}

unsigned MediaPlayer::hasVout()
{
    return libvlc_media_player_has_vout(get());
}

bool MediaPlayer::isSeekable()
{
    return libvlc_media_player_is_seekable(get()) != 0;
}

bool MediaPlayer::canPause()
{
    return libvlc_media_player_can_pause(get()) != 0;
}

bool MediaPlayer::programScrambled()
{
    return libvlc_media_player_program_scrambled(get()) != 0;
}

void MediaPlayer::nextFrame()
{
    libvlc_media_player_next_frame(get());
}

void MediaPlayer::navigate(unsigned navigate)
{
    libvlc_media_player_navigate(get(), navigate);
}

void MediaPlayer::setVideoTitleDisplay(libvlc_position_t position, unsigned int timeout)
{
    libvlc_media_player_set_video_title_display(get(), position, timeout);
}

void MediaPlayer::toggleFullscreen()
{
    libvlc_toggle_fullscreen(get());
}

void MediaPlayer::setFullscreen(int b_fullscreen)
{
    libvlc_set_fullscreen(get(), b_fullscreen);
}

bool MediaPlayer::fullscreen()
{
    return libvlc_get_fullscreen(get()) != 0;
}

void MediaPlayer::toggleTeletext()
{
    libvlc_toggle_teletext(get());
}

int MediaPlayer::setEqualizer(libvlc_equalizer_t * p_equalizer)
{
    return libvlc_media_player_set_equalizer(get(), p_equalizer);
}

void MediaPlayer::setAudioCallbacks(libvlc_audio_play_cb play, libvlc_audio_pause_cb pause, libvlc_audio_resume_cb resume, libvlc_audio_flush_cb flush, libvlc_audio_drain_cb drain, void * opaque)
{
    libvlc_audio_set_callbacks(get(), play, pause, resume, flush, drain, opaque);
}

void MediaPlayer::setVolumeCallback(libvlc_audio_set_volume_cb set_volume)
{
    libvlc_audio_set_volume_callback(get(), set_volume);
}

void MediaPlayer::setAudioFormatCallbacks(libvlc_audio_setup_cb setup, libvlc_audio_cleanup_cb cleanup)
{
    libvlc_audio_set_format_callbacks(get(), setup, cleanup);
}

void MediaPlayer::setAudioFormat(const std::string& format, unsigned rate, unsigned channels)
{
    libvlc_audio_set_format(get(), format.c_str(), rate, channels);
}

int MediaPlayer::setAudioOutput(const std::string& psz_name)
{
    return libvlc_audio_output_set(get(), psz_name.c_str());
}

std::vector<AudioOutputDeviceDescription> MediaPlayer::outputDeviceEnum()
{
    libvlc_audio_output_device_t* devices = libvlc_audio_output_device_enum(get());
    std::vector<AudioOutputDeviceDescription> res;
    if ( devices == NULL )
        return res;
    for ( libvlc_audio_output_device_t* p = devices; p != NULL; p = p->p_next )
        res.push_back( AudioOutputDeviceDescription( p ) );
    libvlc_audio_output_device_list_release( devices );
    return res;
}

void MediaPlayer::outputDeviceSet(const std::string& module, const std::string& device_id)
{
    libvlc_audio_output_device_set(get(), module.c_str(), device_id.c_str());
}

void MediaPlayer::toggleMute()
{
    libvlc_audio_toggle_mute(get());
}

int MediaPlayer::mute()
{
    return libvlc_audio_get_mute(get());
}

void MediaPlayer::setMute(int status)
{
    libvlc_audio_set_mute(get(), status);
}

int MediaPlayer::volume()
{
    return libvlc_audio_get_volume(get());
}

int MediaPlayer::setVolume(int i_volume)
{
    return libvlc_audio_set_volume(get(), i_volume);
}

int MediaPlayer::audioTrackCount()
{
    return libvlc_audio_get_track_count(get());
}

std::vector<TrackDescription> MediaPlayer::audioTrackDescription()
{
    libvlc_track_description_t* result = libvlc_audio_get_track_description( get() );
    return getTracksDescription( result );
}

int MediaPlayer::audioTrack()
{
    return libvlc_audio_get_track(get());
}

int MediaPlayer::setAudioTrack(int i_track)
{
    return libvlc_audio_set_track(get(), i_track);
}

int MediaPlayer::channel()
{
    return libvlc_audio_get_channel(get());
}

int MediaPlayer::setChannel(int channel)
{
    return libvlc_audio_set_channel(get(), channel);
}

int64_t MediaPlayer::delay()
{
    return libvlc_audio_get_delay(get());
}

int MediaPlayer::setDelay(int64_t i_delay)
{
    return libvlc_audio_set_delay(get(), i_delay);
}

void MediaPlayer::setVideoCallbacks(libvlc_video_lock_cb lock, libvlc_video_unlock_cb unlock, libvlc_video_display_cb display, void * opaque)
{
    libvlc_video_set_callbacks(get(), lock, unlock, display, opaque);
}

void MediaPlayer::setVideoFormat(const std::string& chroma, unsigned width, unsigned height, unsigned pitch)
{
    libvlc_video_set_format(get(), chroma.c_str(), width, height, pitch);
}

void MediaPlayer::setVideoFormatCallbacks(libvlc_video_format_cb setup, libvlc_video_cleanup_cb cleanup)
{
    libvlc_video_set_format_callbacks(get(), setup, cleanup);
}

void MediaPlayer::setKeyInput(unsigned on)
{
    libvlc_video_set_key_input(get(), on);
}

void MediaPlayer::setMouseInput(unsigned on)
{
    libvlc_video_set_mouse_input(get(), on);
}

int MediaPlayer::size(unsigned num, unsigned * px, unsigned * py)
{
    return libvlc_video_get_size(get(), num, px, py);
}

int MediaPlayer::cursor(unsigned num, int * px, int * py)
{
    return libvlc_video_get_cursor(get(), num, px, py);
}

float MediaPlayer::scale()
{
    return libvlc_video_get_scale(get());
}

void MediaPlayer::setScale(float f_factor)
{
    libvlc_video_set_scale(get(), f_factor);
}

std::string MediaPlayer::aspectRatio()
{
    char* c_result = libvlc_video_get_aspect_ratio(get());
    if ( c_result == NULL )
        return std::string();
    std::string result = c_result;
    libvlc_free(c_result);
    return result;
}

void MediaPlayer::setAspectRatio(const std::string& psz_aspect)
{
    libvlc_video_set_aspect_ratio(get(), psz_aspect.c_str());
}

int MediaPlayer::spu()
{
    return libvlc_video_get_spu(get());
}

int MediaPlayer::spuCount()
{
    return libvlc_video_get_spu_count(get());
}

std::vector<TrackDescription> MediaPlayer::spuDescription()
{
    libvlc_track_description_t* result = libvlc_video_get_spu_description( get() );
    return getTracksDescription( result );
}

int MediaPlayer::setSpu(int i_spu)
{
    return libvlc_video_set_spu(get(), i_spu);
}

int MediaPlayer::setSubtitleFile(const std::string& psz_subtitle)
{
    return libvlc_video_set_subtitle_file(get(), psz_subtitle.c_str());
}

int64_t MediaPlayer::spuDelay()
{
    return libvlc_video_get_spu_delay(get());
}

int MediaPlayer::setSpuDelay(int64_t i_delay)
{
    return libvlc_video_set_spu_delay(get(), i_delay);
}

std::vector<TrackDescription> MediaPlayer::titleDescription()
{
    libvlc_track_description_t* result = libvlc_video_get_title_description( get() );
    return getTracksDescription( result );
}

std::vector<TrackDescription> MediaPlayer::chapterDescription(int i_title)
{
    libvlc_track_description_t* result = libvlc_video_get_chapter_description( get(), i_title );
    return getTracksDescription( result );
}

std::string MediaPlayer::cropGeometry()
{
    char* c_result = libvlc_video_get_crop_geometry(get());
    if ( c_result == NULL )
        return std::string();
    std::string result = c_result;
    libvlc_free(c_result);
    return result;
}

void MediaPlayer::setCropGeometry(const std::string& psz_geometry)
{
    libvlc_video_set_crop_geometry(get(), psz_geometry.c_str());
}

int MediaPlayer::teletext()
{
    return libvlc_video_get_teletext(get());
}

void MediaPlayer::setTeletext(int i_page)
{
    libvlc_video_set_teletext(get(), i_page);
}

int MediaPlayer::videoTrackCount()
{
    return libvlc_video_get_track_count(get());
}

std::vector<TrackDescription> MediaPlayer::videoTrackDescription()
{
    libvlc_track_description_t* result = libvlc_video_get_track_description( get() );
    return getTracksDescription( result );
}

int MediaPlayer::videoTrack()
{
    return libvlc_video_get_track(get());
}

int MediaPlayer::setVideoTrack(int i_track)
{
    return libvlc_video_set_track(get(), i_track);
}

int MediaPlayer::takeSnapshot(unsigned num, const std::string& psz_filepath, unsigned int i_width, unsigned int i_height)
{
    return libvlc_video_take_snapshot(get(), num, psz_filepath.c_str(), i_width, i_height);
}

void MediaPlayer::setDeinterlace(const std::string& psz_mode)
{
    libvlc_video_set_deinterlace(get(), psz_mode.c_str());
}

int MediaPlayer::marqueeInt(unsigned option)
{
    return libvlc_video_get_marquee_int(get(), option);
}

std::string MediaPlayer::marqueeString(unsigned option)
{
    char* c_result = libvlc_video_get_marquee_string(get(), option);
    if ( c_result == NULL )
        return std::string();
    std::string result = c_result;
    libvlc_free(c_result);
    return result;
}

void MediaPlayer::setMarqueeInt(unsigned option, int i_val)
{
    libvlc_video_set_marquee_int(get(), option, i_val);
}

void MediaPlayer::setMarqueeString(unsigned option, const std::string& psz_text)
{
    libvlc_video_set_marquee_string(get(), option, psz_text.c_str());
}

int MediaPlayer::logoInt(unsigned option)
{
    return libvlc_video_get_logo_int(get(), option);
}

void MediaPlayer::setLogoInt(unsigned option, int value)
{
    libvlc_video_set_logo_int(get(), option, value);
}

void MediaPlayer::setLogoString(unsigned option, const std::string& psz_value)
{
    libvlc_video_set_logo_string(get(), option, psz_value.c_str());
}

int MediaPlayer::adjustInt(unsigned option)
{
    return libvlc_video_get_adjust_int(get(), option);
}

void MediaPlayer::setAdjustInt(unsigned option, int value)
{
    libvlc_video_set_adjust_int(get(), option, value);
}

float MediaPlayer::adjustFloat(unsigned option)
{
    float c_result = libvlc_video_get_adjust_float(get(), option);
    float result = c_result;
    return result;
}

void MediaPlayer::setAdjustFloat(unsigned option, float value)
{
    libvlc_video_set_adjust_float(get(), option, value);
}

std::vector<TrackDescription> MediaPlayer::getTracksDescription(libvlc_track_description_t* tracks) const
{
    std::vector<TrackDescription> result;
    if ( tracks == NULL )
        return result;
    libvlc_track_description_t* p = tracks;
    while ( p != NULL )
    {
        result.push_back( TrackDescription( p ) );
        p = p->p_next;
    }
    libvlc_track_description_list_release(tracks);
    return result;
}

} // namespace VLC

