/*****************************************************************************
* EventManagerCX.hpp: EventManager API
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
#include "StructuresCX.hpp"
#include "MediaCX.hpp"

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

    public delegate void MediaListItemAdded(Media^, int);
    public delegate void MediaListWillAddItem(Media^, int);
    public delegate void MediaListItemDeleted(Media^, int);
    public delegate void MediaListWillDeleteItem(Media^, int);

	public delegate void ParsedChanged(ParsedStatus);
    ref class EventManager;

    private ref class EventRemover sealed
    {
    internal:
        static void removeToken(std::vector<VLC::EventManager::RegisteredEvent>& events, Windows::Foundation::EventRegistrationToken token);
    };
    

    public ref class MediaPlayerEventManager sealed
    {
    private:
        std::vector<VLC::EventManager::RegisteredEvent> m_events;
    public:
        event MediaChanged^ OnMediaChanged
        {
            Windows::Foundation::EventRegistrationToken add(MediaChanged^ handler)
            {
                auto h = m_em.onMediaChanged([handler](VLC::MediaPtr media) {
                    handler(ref new Media(*media.get()));
                });
                m_events.push_back(h);
                return Windows::Foundation::EventRegistrationToken{ (int64) h };
            }
            void remove(Windows::Foundation::EventRegistrationToken token)
            {
                EventRemover::removeToken(m_events, token);
            }
        }

        event Opening^ OnOpening
        {
            Windows::Foundation::EventRegistrationToken add(Opening^ handler)
            {
                auto h = m_em.onOpening([handler]() {
                    handler();
                });
                m_events.push_back(h);
                return Windows::Foundation::EventRegistrationToken{ (int64) h };
            }

            void remove(Windows::Foundation::EventRegistrationToken token)
            {
                EventRemover::removeToken(m_events, token);
            }
        }

        event Buffering^ OnBuffering
        {
            Windows::Foundation::EventRegistrationToken add(Buffering^ handler)
            {
                auto h = m_em.onBuffering([handler](float b) {
                    handler(b);
                });
                m_events.push_back(h);
                return Windows::Foundation::EventRegistrationToken{ (int64) h };
            }
            
            void remove(Windows::Foundation::EventRegistrationToken token)
            {
                EventRemover::removeToken(m_events, token);
            }
        }

        event Playing^ OnPlaying
        {
            Windows::Foundation::EventRegistrationToken add(Playing^ handler)
            {
                auto h = m_em.onPlaying([handler]() {
                    handler();
                });
                m_events.push_back(h);
                return Windows::Foundation::EventRegistrationToken{ (int64) h };
            }

            void remove(Windows::Foundation::EventRegistrationToken token)
            {
                EventRemover::removeToken(m_events, token);
            }
        }

        event Paused^ OnPaused
        {
            Windows::Foundation::EventRegistrationToken add(Paused^ handler)
            {
                auto h = m_em.onPaused([handler]() {
                    handler();
                });
                m_events.push_back(h);
                return Windows::Foundation::EventRegistrationToken{ (int64) h };
            }

            void remove(Windows::Foundation::EventRegistrationToken token)
            {
                EventRemover::removeToken(m_events, token);
            }
        }

        event Stopped^ OnStopped
        {
            Windows::Foundation::EventRegistrationToken add(Stopped^ handler)
            {
                auto h = m_em.onStopped([handler]() {
                    handler();
                });
                m_events.push_back(h);
                return Windows::Foundation::EventRegistrationToken{ (int64) h };
            }

            void remove(Windows::Foundation::EventRegistrationToken token)
            {
                EventRemover::removeToken(m_events, token);
            }
        }

        event Forward^ OnForward
        {
            Windows::Foundation::EventRegistrationToken add(Forward^ handler)
            {
                auto h = m_em.onForward([handler]() {
                    handler();
                });
                m_events.push_back(h);
                return Windows::Foundation::EventRegistrationToken{ (int64) h };
            }

            void remove(Windows::Foundation::EventRegistrationToken token)
            {
                EventRemover::removeToken(m_events, token);
            }
        }

        event Backward^ OnBackward
        {
            Windows::Foundation::EventRegistrationToken add(Backward^ handler)
            {
                auto h = m_em.onBackward([handler]() {
                    handler();
                });
                m_events.push_back(h);
                return Windows::Foundation::EventRegistrationToken{ (int64) h };
            }

            void remove(Windows::Foundation::EventRegistrationToken token)
            {
                EventRemover::removeToken(m_events, token);
            }
        }

        event EndReached^ OnEndReached
        {
            Windows::Foundation::EventRegistrationToken add(EndReached^ handler)
            {
                auto h = m_em.onEndReached([handler]() {
                    handler();
                });
                m_events.push_back(h);
                return Windows::Foundation::EventRegistrationToken{ (int64) h };
            }
            
            void remove(Windows::Foundation::EventRegistrationToken token)
            {
                EventRemover::removeToken(m_events, token);
            }
        }

        event EncounteredError^ OnEncounteredError
        {
            Windows::Foundation::EventRegistrationToken add(EncounteredError^ handler)
            {
                auto h = m_em.onEncounteredError([handler]() {
                    handler();
                });
                m_events.push_back(h);
                return Windows::Foundation::EventRegistrationToken{ (int64) h };
            }

            void remove(Windows::Foundation::EventRegistrationToken token)
            {
                EventRemover::removeToken(m_events, token);
            }
        }

        event TimeChanged^ OnTimeChanged
        {
            Windows::Foundation::EventRegistrationToken add(TimeChanged^ handler)
            {
                auto h = m_em.onTimeChanged([handler](int64_t time) {
                    handler(time);
                });
                m_events.push_back(h);
                return Windows::Foundation::EventRegistrationToken{ (int64) h };
            }

            void remove(Windows::Foundation::EventRegistrationToken token)
            {
                EventRemover::removeToken(m_events, token);
            }
        }

        event PositionChanged^ OnPositionChanged
        {
            Windows::Foundation::EventRegistrationToken add(PositionChanged^ handler)
            {
                auto h = m_em.onPositionChanged([handler](float p) {
                    handler(p);
                });
                m_events.push_back(h);
                return Windows::Foundation::EventRegistrationToken{ (int64) h };
            }

            void remove(Windows::Foundation::EventRegistrationToken token)
            {
                EventRemover::removeToken(m_events, token);
            }
        }

        event SeekableChanged^ OnSeekableChanged
        {
            Windows::Foundation::EventRegistrationToken add(SeekableChanged^ handler)
            {
                auto h = m_em.onSeekableChanged([handler](bool b) {
                    handler(b);
                });
                m_events.push_back(h);
                return Windows::Foundation::EventRegistrationToken{ (int64) h };
            }

            void remove(Windows::Foundation::EventRegistrationToken token)
            {
                EventRemover::removeToken(m_events, token);
            }
        }

        event PausableChanged^ OnPausableChanged
        {
            Windows::Foundation::EventRegistrationToken add(PausableChanged^ handler)
            {
                auto h = m_em.onPausableChanged([handler](bool b) {
                    handler(b);
                });
                m_events.push_back(h);
                return Windows::Foundation::EventRegistrationToken{ (int64) h };
            }

            void remove(Windows::Foundation::EventRegistrationToken token)
            {
                EventRemover::removeToken(m_events, token);
            }
        }

        event TitleChanged^ OnTitleChanged
        {
            Windows::Foundation::EventRegistrationToken add(TitleChanged^ handler)
            {
                auto h = m_em.onTitleChanged([handler](int t) {
                    handler(t);
                });
                m_events.push_back(h);
                return Windows::Foundation::EventRegistrationToken{ (int64) h };
            }

            void remove(Windows::Foundation::EventRegistrationToken token)
            {
                EventRemover::removeToken(m_events, token);
            }
        }

        event SnapshotTaken^ OnSnapshotTaken
        {
            Windows::Foundation::EventRegistrationToken add(SnapshotTaken^ handler)
            {
                auto h = m_em.onSnapshotTaken([handler](const std::string& path) {
                    handler(ToPlatformString( path ) );
                });
                m_events.push_back(h);
                return Windows::Foundation::EventRegistrationToken{ (int64) h };
            }

            void remove(Windows::Foundation::EventRegistrationToken token)
            {
                EventRemover::removeToken(m_events, token);
            }
        }

        event LengthChanged^ OnLengthChanged
        {
            Windows::Foundation::EventRegistrationToken add(LengthChanged^ handler)
            {
                auto h = m_em.onLengthChanged([handler](int64 length) {
                    handler(length);
                });
                m_events.push_back(h);
                return Windows::Foundation::EventRegistrationToken{ (int64) h };
            }

            void remove(Windows::Foundation::EventRegistrationToken token)
            {
                EventRemover::removeToken(m_events, token);
            }
        }

        event Vout^ OnVoutCountChanged
        {
            Windows::Foundation::EventRegistrationToken add(Vout^ handler)
            {
                auto h = m_em.onVout([handler](int nbVout) {
                    handler(nbVout);
                });
                m_events.push_back(h);
                return Windows::Foundation::EventRegistrationToken{ (int64) h };
            }

            void remove(Windows::Foundation::EventRegistrationToken token)
            {
                EventRemover::removeToken(m_events, token);
            }
        }

        event ScrambledChanged^ OnScrambledChanged
        {
            Windows::Foundation::EventRegistrationToken add(ScrambledChanged^ handler)
            {
                auto h = m_em.onScrambledChanged([handler](bool b) {
                    handler(b);
                });
                m_events.push_back(h);
                return Windows::Foundation::EventRegistrationToken{ (int64) h };
            }

            void remove(Windows::Foundation::EventRegistrationToken token)
            {
                EventRemover::removeToken(m_events, token);
            }
        }

        event ESAdded^ OnTrackAdded
        {
            Windows::Foundation::EventRegistrationToken add(ESAdded^ handler)
            {
                auto h = m_em.onESAdded([handler](libvlc_track_type_t type, int id) {
                    handler((libVLCX::TrackType)type, id);
                });
                m_events.push_back(h);
                return Windows::Foundation::EventRegistrationToken{ (int64) h };
            }

            void remove(Windows::Foundation::EventRegistrationToken token)
            {
                EventRemover::removeToken(m_events, token);
            }
        }

        event ESDeleted^ OnTrackDeleted
        {
            Windows::Foundation::EventRegistrationToken add(ESDeleted^ handler)
            {
                auto h = m_em.onESDeleted([handler](libvlc_track_type_t type, int id) {
                    handler((TrackType)type, id);
                });
                m_events.push_back(h);
                return Windows::Foundation::EventRegistrationToken{ (int64) h };
            }

            void remove(Windows::Foundation::EventRegistrationToken token)
            {
                EventRemover::removeToken(m_events, token);
            }
        }

        event ESSelected^ OnTrackSelected
        {
            Windows::Foundation::EventRegistrationToken add(ESSelected^ handler)
            {
                auto h = m_em.onESSelected([handler](libvlc_track_type_t type, int id) {
                    handler((TrackType)type, id);
                });
                m_events.push_back(h);
                return Windows::Foundation::EventRegistrationToken{ (int64) h };
            }

            void remove(Windows::Foundation::EventRegistrationToken token)
            {
                EventRemover::removeToken(m_events, token);
            }
        }

    internal:
        MediaPlayerEventManager(VLC::MediaPlayerEventManager& em);

    private:
        VLC::MediaPlayerEventManager& m_em;
    };

    public ref class MediaEventManager sealed
    {
    private:
        std::vector<VLC::EventManager::RegisteredEvent> m_events;
    internal:
        MediaEventManager(VLC::MediaEventManager& em);

    private:
        VLC::MediaEventManager& m_em;

    public:
        event ParsedChanged^ OnParsedChanged
        {
            Windows::Foundation::EventRegistrationToken add(ParsedChanged^ handler)
            {
                auto h = m_em.onParsedChanged([handler](VLC::Media::ParsedStatus s) {
                    handler((ParsedStatus)s);
                });
                m_events.push_back(h);
                return Windows::Foundation::EventRegistrationToken{ (int64)h };
            }

            void remove(Windows::Foundation::EventRegistrationToken token) {
                EventRemover::removeToken(m_events, token);
            }
        }
    };

    public ref class MediaListEventManager sealed
    {
    private:
        std::vector<VLC::EventManager::RegisteredEvent> m_events;
    internal:
        MediaListEventManager(VLC::MediaListEventManager& em);

    private:
        VLC::MediaListEventManager& m_em;

    public:
        event MediaListItemAdded^ onItemAdded
        {
            Windows::Foundation::EventRegistrationToken add(MediaListItemAdded^ handler)
            {
                auto h = m_em.onItemAdded([handler](VLC::MediaPtr native, int val) {
                    handler(ref new Media(*native), val);
                });
                m_events.push_back(h);
                return Windows::Foundation::EventRegistrationToken{ (int64)h };
            }

            void remove(Windows::Foundation::EventRegistrationToken token)
            {
                EventRemover::removeToken(m_events, token);
            }
        }

        event MediaListWillAddItem^ onWillAddItem
        {
            Windows::Foundation::EventRegistrationToken add(MediaListWillAddItem^ handler)
            {
                auto h = m_em.onWillAddItem([handler](VLC::MediaPtr native, int val) {
                    handler(ref new Media(*native), val);
                });
                m_events.push_back(h);
                return Windows::Foundation::EventRegistrationToken{ (int64)h };
            }

            void remove(Windows::Foundation::EventRegistrationToken token)
            {
                EventRemover::removeToken(m_events, token);
            }
        }

        event MediaListItemDeleted^ onItemDeleted
        {
            Windows::Foundation::EventRegistrationToken add(MediaListItemDeleted^ handler)
            {
                auto h = m_em.onItemDeleted([handler](VLC::MediaPtr native, int val) {
                    handler(ref new Media(*native), val);
                });
                m_events.push_back(h);
                return Windows::Foundation::EventRegistrationToken{ (int64)h };
            }

            void remove(Windows::Foundation::EventRegistrationToken token)
            {
                EventRemover::removeToken(m_events, token);
            }
        }

        event MediaListWillDeleteItem^ onWillDeleteItem
        {
            Windows::Foundation::EventRegistrationToken add(MediaListWillDeleteItem^ handler)
            {
                auto h = m_em.onWillDeleteItem([handler](VLC::MediaPtr native, int val) {
                    handler(ref new Media(*native), val);
                });
                m_events.push_back(h);
                return Windows::Foundation::EventRegistrationToken{ (int64)h };
            }

            void remove(Windows::Foundation::EventRegistrationToken token)
            {
                EventRemover::removeToken(m_events, token);
            }
        }
    };
}
