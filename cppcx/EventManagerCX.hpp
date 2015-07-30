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

    ref class EventManager;

    public ref class MediaPlayerEventManager sealed
    {
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
                auto h = (VLC::EventManager::RegisteredEvent)token.Value;
                auto it = std::find(begin(m_events), end(m_events), h);
                assert(it != end(m_events));
                (*it)->unregister();
                m_events.erase(it);
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
                auto h = (VLC::EventManager::RegisteredEvent)token.Value;
                auto it = std::find(begin(m_events), end(m_events), h);
                assert(it != end(m_events));
                (*it)->unregister();
                m_events.erase(it);
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
                auto h = (VLC::EventManager::RegisteredEvent)token.Value;
                auto it = std::find(begin(m_events), end(m_events), h);
                assert(it != end(m_events));
                (*it)->unregister();
                m_events.erase(it);
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
                auto h = (VLC::EventManager::RegisteredEvent)token.Value;
                auto it = std::find(begin(m_events), end(m_events), h);
                assert(it != end(m_events));
                (*it)->unregister();
                m_events.erase(it);
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
                auto h = (VLC::EventManager::RegisteredEvent)token.Value;
                auto it = std::find(begin(m_events), end(m_events), h);
                assert(it != end(m_events));
                (*it)->unregister();
                m_events.erase(it);
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
                auto h = (VLC::EventManager::RegisteredEvent)token.Value;
                auto it = std::find(begin(m_events), end(m_events), h);
                assert(it != end(m_events));
                (*it)->unregister();
                m_events.erase(it);
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
                auto h = (VLC::EventManager::RegisteredEvent)token.Value;
                auto it = std::find(begin(m_events), end(m_events), h);
                assert(it != end(m_events));
                (*it)->unregister();
                m_events.erase(it);
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
                auto h = (VLC::EventManager::RegisteredEvent)token.Value;
                auto it = std::find(begin(m_events), end(m_events), h);
                assert(it != end(m_events));
                (*it)->unregister();
                m_events.erase(it);
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
                auto h = (VLC::EventManager::RegisteredEvent)token.Value;
                auto it = std::find(begin(m_events), end(m_events), h);
                assert(it != end(m_events));
                (*it)->unregister();
                m_events.erase(it);
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
                auto h = (VLC::EventManager::RegisteredEvent)token.Value;
                auto it = std::find(begin(m_events), end(m_events), h);
                assert(it != end(m_events));
                (*it)->unregister();
                m_events.erase(it);
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
                auto h = (VLC::EventManager::RegisteredEvent)token.Value;
                auto it = std::find(begin(m_events), end(m_events), h);
                assert(it != end(m_events));
                (*it)->unregister();
                m_events.erase(it);
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
                auto h = (VLC::EventManager::RegisteredEvent)token.Value;
                auto it = std::find(begin(m_events), end(m_events), h);
                assert(it != end(m_events));
                (*it)->unregister();
                m_events.erase(it);
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
                auto h = (VLC::EventManager::RegisteredEvent)token.Value;
                auto it = std::find(begin(m_events), end(m_events), h);
                assert(it != end(m_events));
                (*it)->unregister();
                m_events.erase(it);
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
                auto h = (VLC::EventManager::RegisteredEvent)token.Value;
                auto it = std::find(begin(m_events), end(m_events), h);
                assert(it != end(m_events));
                (*it)->unregister();
                m_events.erase(it);
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
                auto h = (VLC::EventManager::RegisteredEvent)token.Value;
                auto it = std::find(begin(m_events), end(m_events), h);
                assert(it != end(m_events));
                (*it)->unregister();
                m_events.erase(it);
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
                auto h = (VLC::EventManager::RegisteredEvent)token.Value;
                auto it = std::find(begin(m_events), end(m_events), h);
                assert(it != end(m_events));
                (*it)->unregister();
                m_events.erase(it);
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
                auto h = (VLC::EventManager::RegisteredEvent)token.Value;
                auto it = std::find(begin(m_events), end(m_events), h);
                assert(it != end(m_events));
                (*it)->unregister();
                m_events.erase(it);
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
                auto h = (VLC::EventManager::RegisteredEvent)token.Value;
                auto it = std::find(begin(m_events), end(m_events), h);
                assert(it != end(m_events));
                (*it)->unregister();
                m_events.erase(it);
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
                auto h = (VLC::EventManager::RegisteredEvent)token.Value;
                auto it = std::find(begin(m_events), end(m_events), h);
                assert(it != end(m_events));
                (*it)->unregister();
                m_events.erase(it);
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
                auto h = (VLC::EventManager::RegisteredEvent)token.Value;
                auto it = std::find(begin(m_events), end(m_events), h);
                assert(it != end(m_events));
                (*it)->unregister();
                m_events.erase(it);
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
                auto h = (VLC::EventManager::RegisteredEvent)token.Value;
                auto it = std::find(begin(m_events), end(m_events), h);
                assert(it != end(m_events));
                (*it)->unregister();
                m_events.erase(it);
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
                auto h = (VLC::EventManager::RegisteredEvent)token.Value;
                auto it = std::find(begin(m_events), end(m_events), h);
                assert(it != end(m_events));
                (*it)->unregister();
                m_events.erase(it);
            }
        }

    internal:
        MediaPlayerEventManager(VLC::MediaPlayerEventManager& em);

    private:
        VLC::MediaPlayerEventManager& m_em;
        std::vector<VLC::EventManager::RegisteredEvent> m_events;
    };

    public ref class MediaEventManager sealed
    {
    internal:
        MediaEventManager(VLC::MediaEventManager& em);
    private:
        VLC::MediaEventManager m_em;
    };
}