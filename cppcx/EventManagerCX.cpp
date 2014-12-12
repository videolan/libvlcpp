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

#include "EventManagerCX.hpp"
#include "MediaCX.hpp"

namespace libVLCX
{
    MediaPlayerEventCb::MediaPlayerEventCb(EventManager^ em)
        : m_em(em) 
    {
    }

    void MediaPlayerEventCb::mediaChanged(const VLC::Media& media)
    {
        m_em->OnMediaChanged(ref new Media(media));
    }

    void MediaPlayerEventCb::nothingSpecial()
    {
    }

    void MediaPlayerEventCb::opening()
    {
        m_em->OnOpening();
    }

    void MediaPlayerEventCb::buffering(float percent)
    {
        m_em->OnBuffering(percent);
    }

    void MediaPlayerEventCb::playing()
    {
        m_em->OnPlaying();
    }

    void MediaPlayerEventCb::paused()
    {
        m_em->OnPaused();
    }

    void MediaPlayerEventCb::stopped()
    {
        m_em->OnStopped();
    }

    void MediaPlayerEventCb::forward()
    {
        m_em->OnForward();
    }

    void MediaPlayerEventCb::backward()
    {
        m_em->OnBackward();
    }

    void MediaPlayerEventCb::endReached()
    {
        m_em->OnEndReached();
    }

    void MediaPlayerEventCb::encounteredError()
    {
        m_em->OnEncounteredError();
    }

    void MediaPlayerEventCb::timeChanged(libvlc_time_t time)
    {
        m_em->OnTimeChanged(time);
    }

    void MediaPlayerEventCb::positionChanged(float pos)
    {
        m_em->OnPositionChanged(pos);
    }

    void MediaPlayerEventCb::seekableChanged(bool val)
    {
        m_em->OnSeekableChanged(val);
    }

    void MediaPlayerEventCb::pausableChanged(bool val)
    {
        m_em->OnPausableChanged(val);
    }

    void MediaPlayerEventCb::titleChanged(int title)
    {
        m_em->OnTitleChanged(title);
    }

    void MediaPlayerEventCb::snapshotTaken(const std::string& str)
    {
        m_em->OnSnapshotTaken(ToPlatformString(str));
    }

    void MediaPlayerEventCb::lengthChanged(libvlc_time_t length)
    {
        m_em->OnLengthChanged(length);
    }

    void MediaPlayerEventCb::vout(int nbVout)
    {
        m_em->OnVoutCountChanged(nbVout);
    }

    void MediaPlayerEventCb::scrambledChanged(int val)
    {
        m_em->OnScrambledChanged(val);
    }

    void MediaPlayerEventCb::eSAdded(libvlc_track_type_t, int)
    {

    }
    void MediaPlayerEventCb::eSDeleted(libvlc_track_type_t, int)
    {

    }
    void MediaPlayerEventCb::eSSelected(libvlc_track_type_t, int)
    {

    }

    EventManager::EventManager(VLC::EventManager& em)
        : m_em(em)
        , m_cb(new MediaPlayerEventCb(this))
    {
        em.attach(libvlc_MediaPlayerMediaChanged, m_cb.get());
        em.attach(libvlc_MediaPlayerNothingSpecial, m_cb.get());
        em.attach(libvlc_MediaPlayerOpening, m_cb.get());
        em.attach(libvlc_MediaPlayerBuffering, m_cb.get());
        em.attach(libvlc_MediaPlayerPlaying, m_cb.get());
        em.attach(libvlc_MediaPlayerPaused, m_cb.get());
        em.attach(libvlc_MediaPlayerStopped, m_cb.get());
        em.attach(libvlc_MediaPlayerForward, m_cb.get());
        em.attach(libvlc_MediaPlayerBackward, m_cb.get());
        em.attach(libvlc_MediaPlayerEndReached, m_cb.get());
        em.attach(libvlc_MediaPlayerEncounteredError, m_cb.get());
        em.attach(libvlc_MediaPlayerTimeChanged, m_cb.get());
        em.attach(libvlc_MediaPlayerPositionChanged, m_cb.get());
        em.attach(libvlc_MediaPlayerSeekableChanged, m_cb.get());
        em.attach(libvlc_MediaPlayerPausableChanged, m_cb.get());
        em.attach(libvlc_MediaPlayerTitleChanged, m_cb.get());
        em.attach(libvlc_MediaPlayerSnapshotTaken, m_cb.get());
        em.attach(libvlc_MediaPlayerLengthChanged, m_cb.get());
        em.attach(libvlc_MediaPlayerVout, m_cb.get());
        em.attach(libvlc_MediaPlayerScrambledChanged, m_cb.get());
        em.attach(libvlc_MediaPlayerESAdded, m_cb.get());
        em.attach(libvlc_MediaPlayerESDeleted, m_cb.get());
        em.attach(libvlc_MediaPlayerESSelected, m_cb.get());
    }
}