/*****************************************************************************
* MediaPlayerCX.cpp: MediaPlayer API
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

#include "MediaPlayerCX.hpp"
#include "MediaCX.hpp"
#include "InstanceCX.hpp"
#include "EventManagerCX.hpp"
#include "EqualizerCX.hpp"

#include "Helpers.h"

namespace libVLCX
{
    MediaPlayer::MediaPlayer(Instance^ instance)
        : m_mp(VLC::MediaPlayer(instance->m_instance))
    {
    }

    MediaPlayer::MediaPlayer(Media^ md)
        : m_mp(VLC::MediaPlayer(md->m_media))
    {
    }

    void MediaPlayer::setMedia(Media^ md)
    {
        m_mp.setMedia(md->m_media);
    }

    Media^ MediaPlayer::media()
    {
        return ref new Media(*m_mp.media().get());
    }

    MediaPlayerEventManager^ MediaPlayer::eventManager()
    {
        if (m_eventManager == nullptr)
            m_eventManager = ref new MediaPlayerEventManager(m_mp.eventManager());
        return m_eventManager;

    }

    bool MediaPlayer::isPlaying()
    {
        return m_mp.isPlaying();
    }

    int MediaPlayer::play()
    {
        return m_mp.play();
    }

    void MediaPlayer::setPause(int do_pause)
    {
        m_mp.setPause((bool)do_pause);
    }

    void MediaPlayer::pause()
    {
        m_mp.pause();
    }

    void MediaPlayer::stop()
    {
        m_mp.stop();
    }

    void MediaPlayer::setHwnd(uint64 drawable)
    {
        m_mp.setHwnd((void*) drawable);
    }

    uint64 MediaPlayer::hwnd()
    {
        return (uint64)m_mp.hwnd();
    }

    libvlc_time_t MediaPlayer::length()
    {
        return m_mp.length();
    }

    libvlc_time_t MediaPlayer::time()
    {
        return m_mp.time();
    }

    void MediaPlayer::setTime(libvlc_time_t i_time)
    {
        m_mp.setTime(i_time);
    }

    float MediaPlayer::position()
    {
        return m_mp.position();
    }

    void MediaPlayer::setPosition(float f_pos)
    {
        m_mp.setPosition(f_pos);
    }

    void MediaPlayer::setChapter(int i_chapter)
    {
        m_mp.setChapter(i_chapter);
    }

    int MediaPlayer::chapter()
    {
        return m_mp.chapter();
    }

    int MediaPlayer::chapterCount()
    {
        return m_mp.chapterCount();
    }

    bool MediaPlayer::willPlay()
    {
        return m_mp.willPlay();
    }

    int MediaPlayer::chapterCountForTitle(int i_title)
    {
        return m_mp.chapterCountForTitle(i_title);
    }

    void MediaPlayer::setTitle(int i_title)
    {
        m_mp.setTitle(i_title);
    }

    int MediaPlayer::title()
    {
        return m_mp.title();
    }

    int MediaPlayer::titleCount()
    {
        return m_mp.titleCount();
    }

    void MediaPlayer::previousChapter()
    {
        m_mp.previousChapter();
    }
    void MediaPlayer::nextChapter()
    {
        m_mp.nextChapter();
    }

    float MediaPlayer::rate()
    {
        return m_mp.rate();
    }

    int MediaPlayer::setRate(float rate)
    {
        return m_mp.setRate(rate);
    }

    MediaState MediaPlayer::state()
    {
        return (MediaState)m_mp.state();
    }

    unsigned MediaPlayer::hasVout()
    {
        return m_mp.hasVout();
    }

    bool MediaPlayer::isSeekable()
    {
        return m_mp.isSeekable();
    }

    bool MediaPlayer::canPause()
    {
        return m_mp.canPause();
    }

    bool MediaPlayer::programScrambled()
    {
        return m_mp.programScrambled();
    }

    void MediaPlayer::nextFrame()
    {
        m_mp.nextFrame();
    }

    void MediaPlayer::navigate(unsigned int nav)
    {
        m_mp.navigate(nav);
    }

    void MediaPlayer::setVideoTitleDisplay(TitlePosition position, unsigned int timeout)
    {
        m_mp.setVideoTitleDisplay((libvlc_position_t)position, timeout);
    }

    void MediaPlayer::toggleFullscreen()
    {
        m_mp.toggleFullscreen();
    }

    void MediaPlayer::setFullscreen(int b_fullscreen)
    {
        m_mp.setFullscreen(b_fullscreen);
    }

    bool MediaPlayer::fullscreen()
    {
        return m_mp.fullscreen();
    }

    void MediaPlayer::toggleTeletext()
    {
        m_mp.toggleTeletext();
    }

    int MediaPlayer::setEqualizer(Equalizer^ eq)
    {
        return m_mp.setEqualizer(eq->m_eq);
    }

    bool MediaPlayer::unsetEqualizer()
    {
        return m_mp.unsetEqualizer();
    }

    //void setAudioCallbacks(libvlc_audio_play_cb play, libvlc_audio_pause_cb pause, libvlc_audio_resume_cb resume, libvlc_audio_flush_cb flush, libvlc_audio_drain_cb drain, void * opaque);

    //void setVolumeCallback(libvlc_audio_set_volume_cb set_volume);

    //void setAudioFormatCallbacks(libvlc_audio_setup_cb setup, libvlc_audio_cleanup_cb cleanup);

    //void setAudioFormat(Platform::String^ format, unsigned rate, unsigned channels);


    int MediaPlayer::setAudioOutput(Platform::String^ psz_name)
    {
        return m_mp.setAudioOutput(VLCString(psz_name));
    }

    Windows::Foundation::Collections::IVector<AudioOutputDeviceDescription^>^ MediaPlayer::outputDeviceEnum()
    {
        return MarshallVector<AudioOutputDeviceDescription, VLC::AudioOutputDeviceDescription>(m_mp.outputDeviceEnum());
    }

    void MediaPlayer::outputDeviceSet(Platform::String^ device_id)
    {
        m_mp.outputDeviceSet(VLCString(device_id));
    }

    void MediaPlayer::toggleMute()
    {
        m_mp.toggleMute();
    }

    int MediaPlayer::mute()
    {
        return m_mp.mute();
    }

    void MediaPlayer::setMute(int status)
    {
        m_mp.setMute(status);
    }

    int MediaPlayer::volume()
    {
        return m_mp.volume();
    }

    int MediaPlayer::setVolume(int i_volume)
    {
        return m_mp.setVolume(i_volume);
    }

    int MediaPlayer::audioTrackCount()
    {
        return m_mp.audioTrackCount();
    }

    Windows::Foundation::Collections::IVector<TrackDescription^>^ MediaPlayer::audioTrackDescription()
    {
        return MarshallVector<TrackDescription, VLC::TrackDescription>(m_mp.audioTrackDescription());
    }

    int MediaPlayer::audioTrack()
    {
        return m_mp.audioTrack();
    }

    int MediaPlayer::setAudioTrack(int i_track)
    {
        return m_mp.setAudioTrack(i_track);
    }

    int MediaPlayer::channel()
    {
        return m_mp.channel();
    }


    int MediaPlayer::setChannel(int channel)
    {
        return m_mp.setChannel(channel);
    }

    int64_t MediaPlayer::audioDelay()
    {
        return m_mp.audioDelay();
    }

    int MediaPlayer::setAudioDelay(int64_t i_delay)
    {
        return m_mp.setAudioDelay(i_delay);
    }

    //void setVideoCallbacks(libvlc_video_lock_cb lock, libvlc_video_unlock_cb unlock, libvlc_video_display_cb display, void * opaque);

    //void setVideoFormat(Platform::String^ chroma, unsigned width, unsigned height, unsigned pitch);

    //void setVideoFormatCallbacks(libvlc_video_format_cb setup, libvlc_video_cleanup_cb cleanup);

    void MediaPlayer::setKeyInput(unsigned on)
    {
        m_mp.setKeyInput(on);
    }

    void MediaPlayer::setMouseInput(unsigned on)
    {
        m_mp.setMouseInput(on);
    }

    int MediaPlayer::size(unsigned num, unsigned * px, unsigned * py)
    {
        return m_mp.size(num, px, py);
    }

    int MediaPlayer::cursor(unsigned num, int * px, int * py)
    {
        return m_mp.cursor(num, px, py);
    }

    float MediaPlayer::scale()
    {
        return m_mp.scale();
    }

    void MediaPlayer::setScale(float f_factor)
    {
        m_mp.setScale(f_factor);
    }

    Platform::String^ MediaPlayer::aspectRatio()
    {
        return ToPlatformString(m_mp.aspectRatio());
    }

    void MediaPlayer::setAspectRatio(Platform::String^ psz_aspect)
    {
        m_mp.setAspectRatio(VLCString(psz_aspect));
    }

    int MediaPlayer::spu()
    {
        return m_mp.spu();
    }

    int MediaPlayer::spuCount()
    {
        return m_mp.spuCount();
    }

    Windows::Foundation::Collections::IVector<TrackDescription^>^ MediaPlayer::spuDescription()
    {
        return MarshallVector<TrackDescription, VLC::TrackDescription>(m_mp.spuDescription());
    }

    int MediaPlayer::setSpu(int i_spu)
    {
        return m_mp.setSpu(i_spu);
    }

    bool MediaPlayer::addSlave(SlaveType type, Platform::String^ uri, bool select)
    {
        return m_mp.addSlave(static_cast<VLC::MediaSlave::Type>(type), VLCString(uri), select);
    }

    int64_t MediaPlayer::spuDelay()
    {
        return m_mp.spuDelay();
    }

    int MediaPlayer::setSpuDelay(int64_t i_delay)
    {
        return m_mp.setSpuDelay(i_delay);
    }

    Windows::Foundation::Collections::IVector<TitleDescription^>^ MediaPlayer::titleDescription()
    {
        return MarshallVector<TitleDescription, VLC::TitleDescription>(m_mp.titleDescription());
    }

    Windows::Foundation::Collections::IVector<ChapterDescription^>^ MediaPlayer::chapterDescription(int i_title)
    {
        return MarshallVector<ChapterDescription, VLC::ChapterDescription>(m_mp.chapterDescription(i_title));
    }

    Platform::String^ MediaPlayer::cropGeometry()
    {
        return ToPlatformString(m_mp.cropGeometry());
    }

    void MediaPlayer::setCropGeometry(Platform::String^ psz_geometry)
    {
        m_mp.setCropGeometry(VLCString(psz_geometry));
    }

    int MediaPlayer::teletext()
    {
        return m_mp.teletext();
    }

    void MediaPlayer::setTeletext(int i_page)
    {
        m_mp.setTeletext(i_page);
    }

    int MediaPlayer::videoTrackCount()
    {
        return m_mp.videoTrackCount();
    }

    Windows::Foundation::Collections::IVector<TrackDescription^>^ MediaPlayer::videoTrackDescription()
    {
        return MarshallVector<TrackDescription, VLC::TrackDescription>(m_mp.videoTrackDescription());
    }

    int MediaPlayer::videoTrack()
    {
        return m_mp.videoTrack();
    }


    int MediaPlayer::setVideoTrack(int i_track)
    {
        return m_mp.setVideoTrack(i_track);
    }

    int MediaPlayer::takeSnapshot(unsigned num, Platform::String^ psz_filepath, unsigned int i_width, unsigned int i_height)
    {
        return m_mp.takeSnapshot(num, VLCString(psz_filepath), i_width, i_height);
    }

    void MediaPlayer::setDeinterlace(Platform::String^ psz_mode)
    {
        m_mp.setDeinterlace(VLCString(psz_mode));
    }

    int MediaPlayer::marqueeInt(unsigned option)
    {
        return m_mp.marqueeInt(option);
    }

    Platform::String^ MediaPlayer::marqueeString(unsigned option)
    {
        return ToPlatformString(m_mp.marqueeString(option));
    }

    void MediaPlayer::setMarqueeInt(unsigned option, int i_val)
    {
        m_mp.setMarqueeInt(option, i_val);
    }

    void MediaPlayer::setMarqueeString(unsigned option, Platform::String^ psz_text)
    {
        m_mp.setMarqueeString(option, VLCString(psz_text));
    }

    int MediaPlayer::logoInt(unsigned option)
    {
        return m_mp.logoInt(option);
    }

    void MediaPlayer::setLogoInt(unsigned option, int value)
    {
        m_mp.setLogoInt(option, value);
    }

    void MediaPlayer::setLogoString(unsigned option, Platform::String^ psz_value)
    {
        m_mp.setLogoString(option, VLCString(psz_value));
    }

    int MediaPlayer::adjustInt(unsigned option)
    {
        return m_mp.adjustInt(option);
    }

    void MediaPlayer::setAdjustInt(unsigned option, int value)
    {
        m_mp.setAdjustInt(option, value);
    }

    float MediaPlayer::adjustFloat(unsigned option)
    {
        return m_mp.adjustFloat(option);
    }

    void MediaPlayer::setAdjustFloat(unsigned option, float value)
    {
        m_mp.setAdjustFloat(option, value);
    }

} // namespace VLC

