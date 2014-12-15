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

#include "EventManager.hpp"
#include "Media.hpp"
#include "StructuresCX.hpp"

#include <memory>

namespace libVLCX
{
    ref class Media;

    public delegate void MediaChanged(Media^);
    public delegate void NothingSpecial();
    public delegate void Opening();
    public delegate void Buffering(float);
    public delegate void Playing();
    public delegate void Paused();
    public delegate void Stopped();
    public delegate void Forward();
    public delegate void Backward();
    public delegate void EndReached();
    public delegate void EncounteredError();
    public delegate void TimeChanged(libvlc_time_t);
    public delegate void PositionChanged(float);
    public delegate void SeekableChanged(bool);
    public delegate void PausableChanged(bool);
    public delegate void TitleChanged(int);
    public delegate void SnapshotTaken(Platform::String^);
    public delegate void LengthChanged(libvlc_time_t);
    public delegate void Vout(int);
    public delegate void ScrambledChanged(int);
    public delegate void ESAdded(TrackType, int);
    public delegate void ESDeleted(TrackType, int);
    public delegate void ESSelected(TrackType, int);

    ref class EventManager;

    class MediaPlayerEventCb : public VLC::IMediaPlayerEventCb
    {
    public:
        MediaPlayerEventCb(EventManager^ em);
        virtual void mediaChanged(const VLC::Media&);
        virtual void nothingSpecial();
        virtual void opening();
        virtual void buffering(float);
        virtual void playing();
        virtual void paused();
        virtual void stopped();
        virtual void forward();
        virtual void backward();
        virtual void endReached();
        virtual void encounteredError();
        virtual void timeChanged(libvlc_time_t);
        virtual void positionChanged(float);
        virtual void seekableChanged(bool);
        virtual void pausableChanged(bool);
        virtual void titleChanged(int);
        virtual void snapshotTaken(const std::string&);
        virtual void lengthChanged(libvlc_time_t);
        virtual void vout(int);
        virtual void scrambledChanged(int);
        virtual void eSAdded(libvlc_track_type_t, int);
        virtual void eSDeleted(libvlc_track_type_t, int);
        virtual void eSSelected(libvlc_track_type_t, int);

    private:
        EventManager^ m_em;
    };

    public ref class EventManager sealed
    {
    public:
        event MediaChanged^ OnMediaChanged;
        event Opening^ OnOpening;
        event Buffering^ OnBuffering;
        event Playing^ OnPlaying;
        event Paused^ OnPaused;
        event Stopped^ OnStopped;
        event Forward^ OnForward;
        event Backward^ OnBackward;
        event EndReached^ OnEndReached;
        event EncounteredError^ OnEncounteredError;
        event TimeChanged^ OnTimeChanged;
        event PositionChanged^ OnPositionChanged;
        event SeekableChanged^ OnSeekableChanged;
        event PausableChanged^ OnPausableChanged;
        event TitleChanged^ OnTitleChanged;
        event SnapshotTaken^ OnSnapshotTaken;
        event LengthChanged^ OnLengthChanged;
        event Vout^ OnVoutCountChanged;
        event ScrambledChanged^ OnScrambledChanged;
        event ESAdded^ OnTrackAdded;
        event ESDeleted^ OnTrackDeleted;
        event ESSelected^ OnTrackSelected;

    internal:
        EventManager(VLC::EventManager& em);

    private:
        ~EventManager(){}

    private:
        VLC::EventManager m_em;
        std::unique_ptr<MediaPlayerEventCb> m_cb;

        friend class MediaPlayerEventCb;
    };
}