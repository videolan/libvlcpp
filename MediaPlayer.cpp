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

#include "EventManager.hpp"

namespace VLC
{

MediaPlayer::MediaPlayer(const MediaPlayer& another)
    : Internal( another )
{
    if ( another.m_eventManager != NULL )
        m_eventManager = new EventManager( *another.m_eventManager );
    retain();
}

const MediaPlayer& MediaPlayer::operator=(const MediaPlayer& another)
{
    if (this == &another)
    {
        return *this;
    }
    release();
    m_obj = another.m_obj;
    delete m_eventManager;
    if ( another.m_eventManager != NULL )
        m_eventManager = new EventManager( *another.m_eventManager );
    retain();
    return *this;
}

bool MediaPlayer::operator==(const MediaPlayer& another) const
{
    return m_obj == another.m_obj;
}

MediaPlayer::~MediaPlayer()
{
    release();
}

MediaPlayer*MediaPlayer::create( Instance& instance )
{
    InternalPtr ptr = libvlc_media_player_new( instance );
    if ( ptr == NULL )
        return NULL;
    return new MediaPlayer( ptr );
}

MediaPlayer*MediaPlayer::fromMedia( Media& md )
{
    InternalPtr ptr = libvlc_media_player_new_from_media( md );
    if ( ptr == NULL )
        return NULL;
    return new MediaPlayer( ptr );
}

void MediaPlayer::setMedia( Media& md )
{
    libvlc_media_player_set_media( m_obj, md );
}

Media* MediaPlayer::media()
{
    libvlc_media_t* media = libvlc_media_player_get_media(m_obj);
    return new Media(media);
}

EventManager& MediaPlayer::eventManager()
{
    if ( m_eventManager == NULL )
    {
        libvlc_event_manager_t* obj = libvlc_media_player_event_manager( m_obj );
        m_eventManager = new EventManager( obj );
    }
    return *m_eventManager;
}

bool MediaPlayer::isPlaying()
{
    return libvlc_media_player_is_playing(m_obj) != 0;
}

int MediaPlayer::play()
{
    return libvlc_media_player_play(m_obj);
}

void MediaPlayer::setPause(int do_pause)
{
    libvlc_media_player_set_pause(m_obj, do_pause);
}

void MediaPlayer::pause()
{
    libvlc_media_player_pause(m_obj);
}

void MediaPlayer::stop()
{
    libvlc_media_player_stop(m_obj);
}

void MediaPlayer::setNsobject(void * drawable)
{
    libvlc_media_player_set_nsobject(m_obj, drawable);
}

void * MediaPlayer::nsobject()
{
    return libvlc_media_player_get_nsobject(m_obj);
}

void MediaPlayer::setAgl(uint32_t drawable)
{
    libvlc_media_player_set_agl(m_obj, drawable);
}

uint32_t MediaPlayer::agl()
{
    return libvlc_media_player_get_agl(m_obj);
}

void MediaPlayer::setXwindow(uint32_t drawable)
{
    libvlc_media_player_set_xwindow(m_obj, drawable);
}

uint32_t MediaPlayer::xwindow()
{
    return libvlc_media_player_get_xwindow(m_obj);
}

void MediaPlayer::setHwnd(void * drawable)
{
    libvlc_media_player_set_hwnd(m_obj, drawable);
}

void * MediaPlayer::hwnd()
{
    return libvlc_media_player_get_hwnd(m_obj);
}

libvlc_time_t MediaPlayer::length()
{
    return libvlc_media_player_get_length(m_obj);
}

libvlc_time_t MediaPlayer::time()
{
    return  libvlc_media_player_get_time(m_obj);
}

void MediaPlayer::setTime(libvlc_time_t i_time)
{
    libvlc_media_player_set_time(m_obj, i_time);
}

float MediaPlayer::position()
{
    return libvlc_media_player_get_position(m_obj);
}

void MediaPlayer::setPosition(float f_pos)
{
    libvlc_media_player_set_position(m_obj, f_pos);
}

void MediaPlayer::setChapter(int i_chapter)
{
    libvlc_media_player_set_chapter(m_obj, i_chapter);
}

int MediaPlayer::chapter()
{
    return libvlc_media_player_get_chapter(m_obj);
}

int MediaPlayer::chapterCount()
{
    return libvlc_media_player_get_chapter_count(m_obj);
}

bool MediaPlayer::willPlay()
{
    return libvlc_media_player_will_play(m_obj);
}

int MediaPlayer::chapterCountForTitle(int i_title)
{
    return libvlc_media_player_get_chapter_count_for_title(m_obj, i_title);
}

void MediaPlayer::setTitle(int i_title)
{
    libvlc_media_player_set_title(m_obj, i_title);
}

int MediaPlayer::title()
{
    return libvlc_media_player_get_title(m_obj);
}

int MediaPlayer::titleCount()
{
    return libvlc_media_player_get_title_count(m_obj);
}

void MediaPlayer::previousChapter()
{
    libvlc_media_player_previous_chapter(m_obj);
}

void MediaPlayer::nextChapter()
{
    libvlc_media_player_next_chapter(m_obj);
}

float MediaPlayer::rate()
{
    return libvlc_media_player_get_rate(m_obj);
}

int MediaPlayer::setRate(float rate)
{
    return libvlc_media_player_set_rate(m_obj, rate);
}

libvlc_state_t MediaPlayer::state()
{
    return libvlc_media_player_get_state(m_obj);
}

float MediaPlayer::fps()
{
    return libvlc_media_player_get_fps(m_obj);
}

unsigned MediaPlayer::hasVout()
{
    return libvlc_media_player_has_vout(m_obj);
}

bool MediaPlayer::isSeekable()
{
    return libvlc_media_player_is_seekable(m_obj);
}

bool MediaPlayer::canPause()
{
    return libvlc_media_player_can_pause(m_obj);
}

bool MediaPlayer::programScrambled()
{
    return libvlc_media_player_program_scrambled(m_obj);
}

void MediaPlayer::nextFrame()
{
    libvlc_media_player_next_frame(m_obj);
}

void MediaPlayer::navigate(unsigned navigate)
{
    libvlc_media_player_navigate(m_obj, navigate);
}

void MediaPlayer::setVideoTitleDisplay(libvlc_position_t position, unsigned int timeout)
{
    libvlc_media_player_set_video_title_display(m_obj, position, timeout);
}

void MediaPlayer::toggleFullscreen()
{
    libvlc_toggle_fullscreen(m_obj);
}

void MediaPlayer::setFullscreen(int b_fullscreen)
{
    libvlc_set_fullscreen(m_obj, b_fullscreen);
}

bool MediaPlayer::fullscreen()
{
    return libvlc_get_fullscreen(m_obj) != 0;
}

void MediaPlayer::toggleTeletext()
{
    libvlc_toggle_teletext(m_obj);
}

int MediaPlayer::setEqualizer(libvlc_equalizer_t * p_equalizer)
{
    return libvlc_media_player_set_equalizer(m_obj, p_equalizer);
}

void MediaPlayer::setCallbacks(libvlc_audio_play_cb play, libvlc_audio_pause_cb pause, libvlc_audio_resume_cb resume, libvlc_audio_flush_cb flush, libvlc_audio_drain_cb drain, void * opaque)
{
    libvlc_audio_set_callbacks(m_obj, play, pause, resume, flush, drain, opaque);
}

void MediaPlayer::setVolumeCallback(libvlc_audio_set_volume_cb set_volume)
{
    libvlc_audio_set_volume_callback(m_obj, set_volume);
}

void MediaPlayer::setFormatCallbacks(libvlc_audio_setup_cb setup, libvlc_audio_cleanup_cb cleanup)
{
    libvlc_audio_set_format_callbacks(m_obj, setup, cleanup);
}

void MediaPlayer::setFormat(const std::string& format, unsigned rate, unsigned channels)
{
    libvlc_audio_set_format(m_obj, format.c_str(), rate, channels);
}

int MediaPlayer::outputSet(const std::string& psz_name)
{
    return libvlc_audio_output_set(m_obj, psz_name.c_str());
}

std::vector<AudioOutputDeviceDescription> MediaPlayer::outputDeviceEnum()
{
    libvlc_audio_output_device_t* devices = libvlc_audio_output_device_enum(m_obj);
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
    libvlc_audio_output_device_set(m_obj, module.c_str(), device_id.c_str());
}

void MediaPlayer::toggleMute()
{
    libvlc_audio_toggle_mute(m_obj);
}

int MediaPlayer::mute()
{
    return libvlc_audio_get_mute(m_obj);
}

void MediaPlayer::setMute(int status)
{
    libvlc_audio_set_mute(m_obj, status);
}

int MediaPlayer::volume()
{
    return libvlc_audio_get_volume(m_obj);
}

int MediaPlayer::setVolume(int i_volume)
{
    return libvlc_audio_set_volume(m_obj, i_volume);
}

int MediaPlayer::audioTrackCount()
{
    return libvlc_audio_get_track_count(m_obj);
}

std::vector<TrackDescription> MediaPlayer::audioTrackDescription()
{
    libvlc_track_description_t* result = libvlc_audio_get_track_description( m_obj );
    return getTracksDescription( result );
}

int MediaPlayer::audioTrack()
{
    return libvlc_audio_get_track(m_obj);
}

int MediaPlayer::setAudioTrack(int i_track)
{
    return libvlc_audio_set_track(m_obj, i_track);
}

int MediaPlayer::channel()
{
    return libvlc_audio_get_channel(m_obj);
}

int MediaPlayer::setChannel(int channel)
{
    return libvlc_audio_set_channel(m_obj, channel);
}

int64_t MediaPlayer::delay()
{
    return libvlc_audio_get_delay(m_obj);
}

int MediaPlayer::setDelay(int64_t i_delay)
{
    return libvlc_audio_set_delay(m_obj, i_delay);
}

void MediaPlayer::setCallbacks(libvlc_video_lock_cb lock, libvlc_video_unlock_cb unlock, libvlc_video_display_cb display, void * opaque)
{
    libvlc_video_set_callbacks(m_obj, lock, unlock, display, opaque);
}

void MediaPlayer::setFormat(const std::string& chroma, unsigned width, unsigned height, unsigned pitch)
{
    libvlc_video_set_format(m_obj, chroma.c_str(), width, height, pitch);
}

void MediaPlayer::setFormatCallbacks(libvlc_video_format_cb setup, libvlc_video_cleanup_cb cleanup)
{
    libvlc_video_set_format_callbacks(m_obj, setup, cleanup);
}

void MediaPlayer::setKeyInput(unsigned on)
{
    libvlc_video_set_key_input(m_obj, on);
}

void MediaPlayer::setMouseInput(unsigned on)
{
    libvlc_video_set_mouse_input(m_obj, on);
}

int MediaPlayer::size(unsigned num, unsigned * px, unsigned * py)
{
    return libvlc_video_get_size(m_obj, num, px, py);
}

int MediaPlayer::cursor(unsigned num, int * px, int * py)
{
    return libvlc_video_get_cursor(m_obj, num, px, py);
}

float MediaPlayer::scale()
{
    return libvlc_video_get_scale(m_obj);
}

void MediaPlayer::setScale(float f_factor)
{
    libvlc_video_set_scale(m_obj, f_factor);
}

std::string MediaPlayer::aspectRatio()
{
    char * c_result = libvlc_video_get_aspect_ratio(m_obj);
    std::string result = c_result;
    libvlc_free(c_result);
    return result;
}

void MediaPlayer::setAspectRatio(const std::string& psz_aspect)
{
    libvlc_video_set_aspect_ratio(m_obj, psz_aspect.c_str());
}

int MediaPlayer::spu()
{
    return libvlc_video_get_spu(m_obj);
}

int MediaPlayer::spuCount()
{
    return libvlc_video_get_spu_count(m_obj);
}

std::vector<TrackDescription> MediaPlayer::spuDescription()
{
    libvlc_track_description_t* result = libvlc_video_get_spu_description( m_obj );
    return getTracksDescription( result );
}

int MediaPlayer::setSpu(int i_spu)
{
    return libvlc_video_set_spu(m_obj, i_spu);
}

int MediaPlayer::setSubtitleFile(const std::string& psz_subtitle)
{
    return libvlc_video_set_subtitle_file(m_obj, psz_subtitle.c_str());
}

int64_t MediaPlayer::spuDelay()
{
    return libvlc_video_get_spu_delay(m_obj);
}

int MediaPlayer::setSpuDelay(int64_t i_delay)
{
    return libvlc_video_set_spu_delay(m_obj, i_delay);
}

std::vector<TrackDescription> MediaPlayer::titleDescription()
{
    libvlc_track_description_t* result = libvlc_video_get_title_description( m_obj );
    return getTracksDescription( result );
}

std::vector<TrackDescription> MediaPlayer::chapterDescription(int i_title)
{
    libvlc_track_description_t* result = libvlc_video_get_chapter_description( m_obj, i_title );
    return getTracksDescription( result );
}

std::string MediaPlayer::cropGeometry()
{
    char * c_result = libvlc_video_get_crop_geometry(m_obj);
    std::string result = c_result;
    libvlc_free(c_result);
    return result;
}

void MediaPlayer::setCropGeometry(const std::string& psz_geometry)
{
    libvlc_video_set_crop_geometry(m_obj, psz_geometry.c_str());
}

int MediaPlayer::teletext()
{
    return libvlc_video_get_teletext(m_obj);
}

void MediaPlayer::setTeletext(int i_page)
{
    libvlc_video_set_teletext(m_obj, i_page);
}

int MediaPlayer::videoTrackCount()
{
    return libvlc_video_get_track_count(m_obj);
}

std::vector<TrackDescription> MediaPlayer::videoTrackDescription()
{
    libvlc_track_description_t* result = libvlc_video_get_track_description( m_obj );
    return getTracksDescription( result );
}

int MediaPlayer::videoTrack()
{
    return libvlc_video_get_track(m_obj);
}

int MediaPlayer::setVideoTrack(int i_track)
{
    return libvlc_video_set_track(m_obj, i_track);
}

int MediaPlayer::takeSnapshot(unsigned num, const std::string& psz_filepath, unsigned int i_width, unsigned int i_height)
{
    return libvlc_video_take_snapshot(m_obj, num, psz_filepath.c_str(), i_width, i_height);
}

void MediaPlayer::setDeinterlace(const std::string& psz_mode)
{
    libvlc_video_set_deinterlace(m_obj, psz_mode.c_str());
}

int MediaPlayer::marqueeInt(unsigned option)
{
    return libvlc_video_get_marquee_int(m_obj, option);
}

std::string MediaPlayer::marqueeString(unsigned option)
{
    char * c_result = libvlc_video_get_marquee_string(m_obj, option);
    std::string result = c_result;
    libvlc_free(c_result);
    return result;
}

void MediaPlayer::setMarqueeInt(unsigned option, int i_val)
{
    libvlc_video_set_marquee_int(m_obj, option, i_val);
}

void MediaPlayer::setMarqueeString(unsigned option, const std::string& psz_text)
{
    libvlc_video_set_marquee_string(m_obj, option, psz_text.c_str());
}

int MediaPlayer::logoInt(unsigned option)
{
    return libvlc_video_get_logo_int(m_obj, option);
}

void MediaPlayer::setLogoInt(unsigned option, int value)
{
    libvlc_video_set_logo_int(m_obj, option, value);
}

void MediaPlayer::setLogoString(unsigned option, const std::string& psz_value)
{
    libvlc_video_set_logo_string(m_obj, option, psz_value.c_str());
}

int MediaPlayer::adjustInt(unsigned option)
{
    return libvlc_video_get_adjust_int(m_obj, option);
}

void MediaPlayer::setAdjustInt(unsigned option, int value)
{
    libvlc_video_set_adjust_int(m_obj, option, value);
}

float MediaPlayer::adjustFloat(unsigned option)
{
    float c_result = libvlc_video_get_adjust_float(m_obj, option);
    float result = c_result;
    return result;
}

void MediaPlayer::setAdjustFloat(unsigned option, float value)
{
    libvlc_video_set_adjust_float(m_obj, option, value);
}

MediaPlayer::MediaPlayer( InternalPtr ptr )
    : Internal( ptr )
    , m_eventManager( NULL )
{
}

void MediaPlayer::retain()
{
    libvlc_media_player_retain( m_obj );
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
    return result;
}


void MediaPlayer::release() {
    libvlc_media_player_release(m_obj);
}

} // namespace VLC

