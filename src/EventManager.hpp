/*****************************************************************************
 * libvlc_EventManager.hpp: Exposes libvlc events
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

#ifndef LIBVLC_EVENTMANAGER_HPP
#define LIBVLC_EVENTMANAGER_HPP

#include <string>
#include <iostream>

#include "common.hpp"
#include "Internal.hpp"

#include <functional>
#include <unordered_map>
#include <memory>
#include <atomic>

namespace VLC
{

class Media;

using EventToken = unsigned int;

/**
 * @brief This class serves as a base for all event managers.
 *
 * All events can be handled by providing a std::function.
 * libvlcpp will take ownership (ie. the function will be moved inside an internal list)
 * If the provided std::function is a lambda, it may capture anything you desire
 */
class EventManager : public Internal<libvlc_event_manager_t>
{
private:
    // Polymorphic base to hold a templated implementation
    struct EventHandlerBase
    {
        using Wrapper = std::add_pointer<void(const libvlc_event_t*, void*)>::type;
        virtual ~EventHandlerBase() {}
    };

    template <typename Ret, typename... Args>
    class EventHandler : public EventHandlerBase
    {
    public:
        using UserCallback = std::function<Ret(Args...)>;
        EventHandler(EventManager& em, libvlc_event_e eventType, UserCallback&& userCallback, Wrapper wrapper)
            : m_userCallback( userCallback )
            , m_eventManager(&em)
            , m_wrapper(wrapper)
            , m_eventType( eventType )
        {
            if (libvlc_event_attach( *m_eventManager, m_eventType, m_wrapper, &m_userCallback ) != 0)
                throw std::bad_alloc();
        }

        ~EventHandler()
        {
            libvlc_event_detach( *m_eventManager, m_eventType, m_wrapper, &m_userCallback );
        }

        EventHandler(const EventHandler&) = delete;

    private:
        UserCallback m_userCallback;
        // EventManager always outlive EventHandler, no need for smart pointer
        EventManager* m_eventManager;
        Wrapper m_wrapper;
        libvlc_event_e m_eventType;
    };

protected:
    EventManager(InternalPtr ptr)
        : Internal{ ptr, [](InternalPtr){ /* No-op; EventManager's are handled by their respective objects */ } }
        , m_lastEvent(0)
    {
    }

    template <typename Ret, typename... Args>
    EventToken handle(libvlc_event_e eventType, std::function<Ret(Args...)>&& f, EventHandlerBase::Wrapper wrapper)
    {
        auto token = ++m_lastEvent;
        m_lambdas[token] = std::unique_ptr<EventHandlerBase>( new EventHandler<Ret, Args...>(
                                                                  *this, eventType, std::move( f ), wrapper ) );
        return token;
    }

    EventToken handle(libvlc_event_e eventType, std::function<void()>&& f)
    {
        return handle(eventType, std::move( f ), [](const libvlc_event_t*, void* data)
        {
            auto callback = static_cast<std::function<void()>*>( data );
            (*callback)();
        });
    }

    void unregister(EventToken t)
    {
        if (t == 0)
            return;
        m_lambdas.erase(t);
    }

protected:
    // We store the EventHandlerBase's as unique_ptr in order for the function stored within
    // EventHandler<T> not to move to another memory location
    std::unordered_map<EventToken, std::unique_ptr<EventHandlerBase>> m_lambdas;
    std::atomic_uint m_lastEvent;
};

class MediaEventManager : public EventManager
{
    public:
        MediaEventManager(InternalPtr ptr) : EventManager( ptr ) {}

        EventToken onMetaChanged( std::function<void(libvlc_meta_t)>&& f)
        {
            return handle( libvlc_MediaMetaChanged, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(libvlc_meta_t)>*>(data);
                (*callback)( e->u.media_meta_changed.meta_type );
            });
        }

        EventToken onSubItemAdded( std::function<void(MediaPtr)>&& f )
        {
            return handle(libvlc_MediaSubItemAdded, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(MediaPtr)>*>(data);
                (*callback)( std::make_shared<Media>( e->u.media_subitem_added.new_child, true ) );
            });
        }

        EventToken onDurationChanged( std::function<void(int64_t)>&& f )
        {
            return handle(libvlc_MediaDurationChanged, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(int64_t)>*>(data);
                (*callback)( e->u.media_duration_changed.new_duration );
            });
        }

        EventToken onParsedChanged( std::function<void(bool)>&& f )
        {
            return handle( libvlc_MediaParsedChanged, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(bool)>*>(data);
                (*callback)( e->u.media_parsed_changed.new_status );
            });
        }

        EventToken onFreed( std::function<void(MediaPtr)>&& f)
        {
            return handle(libvlc_MediaFreed, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(MediaPtr)>*>(data);
                (*callback)( std::make_shared<Media>( e->u.media_freed.md, true ) );
            });
        }

        EventToken onStateChanged( std::function<void(libvlc_state_t)>&& f)
        {
            return handle(libvlc_MediaStateChanged, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(libvlc_state_t)>*>(data);
                (*callback)( e->u.media_state_changed.new_state );
            });
        }

        EventToken onSubItemTreeAdded( std::function<void(MediaPtr)>&& f)
        {
            return handle(libvlc_MediaSubItemAdded, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(MediaPtr)>*>(data);
                (*callback)( std::make_shared<Media>( e->u.media_subitemtree_added.item, true ) );
            });
        }
};

class MediaPlayerEventManager : public EventManager
{
    public:
        MediaPlayerEventManager(InternalPtr ptr) : EventManager( ptr ) {}

        EventToken onMediaChanged( std::function<void(MediaPtr)>&& f )
        {
            return handle(libvlc_MediaPlayerMediaChanged, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(MediaPtr)>*>( data );
                (*callback)( std::make_shared<Media>( e->u.media_player_media_changed.new_media, true ) );
            });
        }
        EventToken onNothingSpecial(std::function<void()>&& f)
        {
            return handle(libvlc_MediaPlayerNothingSpecial, std::move( f ));
        }

        EventToken onOpening(std::function<void()>&& f)
        {
            return handle(libvlc_MediaPlayerOpening, std::move( f ) );
        }

        EventToken onBuffering( std::function<void()>&& f)
        {
            return handle(libvlc_MediaPlayerBuffering, std::move( f ) );
        }

        EventToken onPlaying(std::function<void()>&& f)
        {
            return handle( libvlc_MediaPlayerPlaying, std::move( f ) );
        }

        EventToken onPaused(std::function<void()>&& f)
        {
            return handle( libvlc_MediaPlayerPaused, std::move( f ) );
        }

        EventToken onStopped(std::function<void()>&& f)
        {
            return handle( libvlc_MediaPlayerStopped, std::move( f ) );
        }

        EventToken onForward(std::function<void()>&& f)
        {
            return handle( libvlc_MediaPlayerForward, std::move( f ) );
        }

        EventToken onBackward(std::function<void()>&& f)
        {
            return handle( libvlc_MediaPlayerBackward, std::move( f ) );
        }

        EventToken onEndReached(std::function<void()>&& f)
        {
            return handle( libvlc_MediaPlayerEndReached, std::move( f ) );
        }

        EventToken onEncounteredError(std::function<void()>&& f)
        {
            return handle( libvlc_MediaPlayerEncounteredError, std::move( f ) );
        }

        EventToken onTimeChanged( std::function<void(libvlc_time_t)>&& f )
        {
            return handle( libvlc_MediaPlayerTimeChanged, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(libvlc_time_t)>*>( data );
                (*callback)( e->u.media_player_time_changed.new_time );
            });
        }

        EventToken onPositionChanged( std::function<void(float)>&& f )
        {
            return handle( libvlc_MediaPlayerPositionChanged, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(float)>*>( data );
                (*callback)( e->u.media_player_position_changed.new_position );
            });
        }

        EventToken onSeekableChanged( std::function<void(bool)>&& f )
        {
            return handle( libvlc_MediaPlayerSeekableChanged, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(bool)>*>( data );
                (*callback)( e->u.media_player_seekable_changed.new_seekable );
            });
        }

        EventToken onPausableChanged( std::function<void(bool)>&& f )
        {
            return handle( libvlc_MediaPlayerSeekableChanged, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(bool)>*>( data );
                (*callback)( e->u.media_player_seekable_changed.new_seekable );
            });
        }

        EventToken onTitleChanged( std::function<void(int)>&& f )
        {
            return handle( libvlc_MediaPlayerTitleChanged, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(int)>*>( data );
                (*callback)( e->u.media_player_title_changed.new_title );
            });
        }

        EventToken onSnapshotTaken( std::function<void(std::string)>&& f )
        {
            return handle( libvlc_MediaPlayerSnapshotTaken, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(std::string)>*>( data );
                (*callback)( e->u.media_player_snapshot_taken.psz_filename );
            });
        }

        EventToken onLengthChanged( std::function<void(libvlc_time_t)>&& f )
        {
            return handle( libvlc_MediaPlayerLengthChanged, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(libvlc_time_t)>*>( data );
                (*callback)( e->u.media_player_length_changed.new_length );
            });
        }

        EventToken onVout( std::function<void(int)>&& f )
        {
            return handle( libvlc_MediaPlayerVout, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(int)>*>( data );
                (*callback)( e->u.media_player_vout.new_count );
            });
        }

        EventToken onScrambledChanged( std::function<void(int)>&& f )
        {
            return handle( libvlc_MediaPlayerScrambledChanged, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(int)>*>( data );
                (*callback)( e->u.media_player_scrambled_changed.new_scrambled );
            });
        }

        EventToken onESAdded( std::function<void(libvlc_track_type_t, int)>&& f )
        {
            return handle( libvlc_MediaPlayerESAdded, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(libvlc_track_type_t, int)>*>( data );
                (*callback)( e->u.media_player_es_changed.i_type, e->u.media_player_es_changed.i_id );
            });
        }

        EventToken onESDeleted( std::function<void(libvlc_track_type_t, int)>&& f )
        {
            return handle( libvlc_MediaPlayerESDeleted, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(libvlc_track_type_t, int)>*>( data );
                (*callback)( e->u.media_player_es_changed.i_type, e->u.media_player_es_changed.i_id );
            });
        }

        EventToken onESSelected( std::function<void(libvlc_track_type_t, int)>&& f )
        {
            return handle( libvlc_MediaPlayerESSelected, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(libvlc_track_type_t, int)>*>( data );
                (*callback)( e->u.media_player_es_changed.i_type, e->u.media_player_es_changed.i_id );
            });
        }
};

class MediaListEventManager : public EventManager
{
    public:
        MediaListEventManager(InternalPtr ptr) : EventManager( ptr ) {}

        EventToken onItemAdded( std::function<void(MediaPtr, int)>&& f )
        {
            return handle(libvlc_MediaListItemAdded, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(MediaPtr, int)>*>( data );
                (*callback)( std::make_shared<Media>( e->u.media_list_item_added.item, true ), e->u.media_list_item_added.index );
            });
        }

        EventToken onWillAddItem( std::function<void(MediaPtr, int)>&& f )
        {
            return handle( libvlc_MediaListWillAddItem, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(MediaPtr, int)>*>( data );
                (*callback)(std::make_shared<Media>( e->u.media_list_will_add_item.item, true ), e->u.media_list_will_add_item.index );
            });
        }

        EventToken onItemDeleted( std::function<void(MediaPtr, int)>&& f )
        {
            return handle(libvlc_MediaListItemDeleted, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(MediaPtr, int)>*>( data );
                (*callback)( std::make_shared<Media>( e->u.media_list_item_deleted.item, true ), e->u.media_list_item_deleted.index );
            });
        }

        EventToken onWillDeleteItem( std::function<void(MediaPtr, int)>&& f )
        {
            return handle(libvlc_MediaListWillDeleteItem, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(MediaPtr, int)>*>( data );
                (*callback)( std::make_shared<Media>( e->u.media_list_will_delete_item.item, true ), e->u.media_list_will_delete_item.index );
            });
        }
};

// MediaListView events are not being sent by VLC, so we don't implement them here

class MediaListPlayerEventManager : public EventManager
{
    public:
        MediaListPlayerEventManager(InternalPtr ptr) : EventManager( ptr ) {}

        EventToken onPlayed(std::function<void()>&& f)
        {
            return handle(libvlc_MediaListPlayerPlayed, std::move( f ) );
        }

        EventToken onNextItemSet( std::function<void(MediaPtr)>&& f )
        {
            return handle(libvlc_MediaListPlayerNextItemSet, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(MediaPtr)>*>( data );
                (*callback)( std::make_shared<Media>( e->u.media_list_player_next_item_set.item, true ) );
            });
        }

        EventToken onStopped(std::function<void()>&& f)
        {
            return handle(libvlc_MediaListPlayerStopped, std::move( f ) );
        }
};

class MediaDiscovererEventManager : public EventManager
{
    public:
        MediaDiscovererEventManager(InternalPtr ptr) : EventManager( ptr ) {}

        EventToken onStarted(std::function<void()>&& f)
        {
            return handle(libvlc_MediaDiscovererStarted, std::move( f ) );
        }

        EventToken onStopped(std::function<void()>&& f)
        {
            return handle(libvlc_MediaDiscovererEnded, std::move( f ) );
        }
};

class VLMEventManager : public EventManager
{
    public:
        using EventManager::EventManager;

        EventToken onMediaAdded( std::function<void(const std::string&)>&& f )
        {
            return handle(libvlc_VlmMediaAdded, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(const std::string&)>*>( data );
                (*callback)( e->u.vlm_media_event.psz_media_name ? e->u.vlm_media_event.psz_media_name : "" );
            });
        }

        EventToken onMediaRemoved( std::function<void(const std::string&)>&& f )
        {
            return handle(libvlc_VlmMediaRemoved, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(const std::string&)>*>( data );
                (*callback)( e->u.vlm_media_event.psz_media_name ? e->u.vlm_media_event.psz_media_name : "" );
            });
        }

        EventToken onMediaChanged( std::function<void(const std::string&)>&& f )
        {
            return handle(libvlc_VlmMediaChanged, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(const std::string&)>*>( data );
                (*callback)( e->u.vlm_media_event.psz_media_name ? e->u.vlm_media_event.psz_media_name : "" );
            });
        }
        EventToken onMediaInstanceStarted( std::function<void(const std::string&, const std::string&)>&& f )
        {
            return handle(libvlc_VlmMediaInstanceStarted, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(const std::string&, const std::string&)>*>( data );
                (*callback)( e->u.vlm_media_event.psz_media_name ? e->u.vlm_media_event.psz_media_name : "",
                             e->u.vlm_media_event.psz_instance_name ? e->u.vlm_media_event.psz_instance_name : "" );
            });
        }

        EventToken onMediaInstanceStopped( std::function<void(const std::string&, const std::string&)>&& f )
        {
            return handle(libvlc_VlmMediaInstanceStopped, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(const std::string&, const std::string&)>*>( data );
                (*callback)( e->u.vlm_media_event.psz_media_name ? e->u.vlm_media_event.psz_media_name : "",
                             e->u.vlm_media_event.psz_instance_name ? e->u.vlm_media_event.psz_instance_name : "" );
            });
        }

        EventToken onMediaInstanceStatusInit( std::function<void(const std::string&, const std::string&)>&& f )
        {
            return handle(libvlc_VlmMediaInstanceStatusInit, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(const std::string&, const std::string&)>*>( data );
                (*callback)( e->u.vlm_media_event.psz_media_name ? e->u.vlm_media_event.psz_media_name : "",
                             e->u.vlm_media_event.psz_instance_name ? e->u.vlm_media_event.psz_instance_name : "" );
            });
        }

        EventToken onMediaInstanceStatusOpening( std::function<void(const std::string&, const std::string&)>&& f )
        {
            return handle(libvlc_VlmMediaInstanceStatusOpening, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(const std::string&, const std::string&)>*>( data );
                (*callback)( e->u.vlm_media_event.psz_media_name ? e->u.vlm_media_event.psz_media_name : "",
                             e->u.vlm_media_event.psz_instance_name ? e->u.vlm_media_event.psz_instance_name : "" );
            });
        }

        EventToken onMediaInstanceStatusPlaying( std::function<void(const std::string&, const std::string&)>&& f )
        {
            return handle(libvlc_VlmMediaInstanceStatusPlaying, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(const std::string&, const std::string&)>*>( data );
                (*callback)( e->u.vlm_media_event.psz_media_name ? e->u.vlm_media_event.psz_media_name : "",
                             e->u.vlm_media_event.psz_instance_name ? e->u.vlm_media_event.psz_instance_name : "" );
            });
        }

        EventToken onMediaInstanceStatusPause( std::function<void(const std::string&, const std::string&)>&& f )
        {
            return handle(libvlc_VlmMediaInstanceStatusPause, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(const std::string&, const std::string&)>*>( data );
                (*callback)( e->u.vlm_media_event.psz_media_name ? e->u.vlm_media_event.psz_media_name : "",
                             e->u.vlm_media_event.psz_instance_name ? e->u.vlm_media_event.psz_instance_name : "" );
            });
        }

        EventToken onMediaInstanceStatusEnd( std::function<void(const std::string&, const std::string&)>&& f )
        {
            return handle(libvlc_VlmMediaInstanceStatusEnd, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(const std::string&, const std::string&)>*>( data );
                (*callback)( e->u.vlm_media_event.psz_media_name ? e->u.vlm_media_event.psz_media_name : "",
                             e->u.vlm_media_event.psz_instance_name ? e->u.vlm_media_event.psz_instance_name : "" );
            });
        }

        EventToken onMediaInstanceStatusError( std::function<void(const std::string&, const std::string&)>&& f )
        {
            return handle(libvlc_VlmMediaInstanceStatusError, std::move( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<std::function<void(const std::string&, const std::string&)>*>( data );
                (*callback)( e->u.vlm_media_event.psz_media_name ? e->u.vlm_media_event.psz_media_name : "",
                             e->u.vlm_media_event.psz_instance_name ? e->u.vlm_media_event.psz_instance_name : "" );
            });
        }
};
}

#endif // LIBVLC_EVENTMANAGER_HPP
