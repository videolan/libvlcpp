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

#define EXPECT_SIGNATURE(sig) static_assert(signature_match<decltype(f), sig>::value, "Expected a function with prototype " #sig)

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
protected:
    template <typename T>
    using Decay = typename std::decay<T>::type;

    template <typename T>
    using DecayPtr = typename std::add_pointer<typename std::decay<T>::type>::type;

    template <typename, typename, typename = void>
    struct signature_match : std::false_type {};

    template <typename Func, typename Ret, typename... Args>
    struct signature_match<Func, Ret(Args...),
        typename std::enable_if<
            std::is_convertible<
                decltype(std::declval<Func>()(std::declval<Args>()...)),
                Ret
            >::value
        >::type
    > : std::true_type {};

private:
    // Polymorphic base to hold a templated implementation
    struct EventHandlerBase
    {
        using Wrapper = std::add_pointer<void(const libvlc_event_t*, void*)>::type;
        virtual ~EventHandlerBase() {}
    };

    template <typename Func>
    class EventHandler : public EventHandlerBase
    {
    public:
        EventHandler(EventManager& em, libvlc_event_e eventType, Func&& userCallback, Wrapper wrapper)
            : m_userCallback( std::forward<Func>(userCallback) )
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
        Decay<Func> m_userCallback;
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

    template <typename Func>
    EventToken handle(libvlc_event_e eventType, Func&& f, EventHandlerBase::Wrapper wrapper)
    {
        auto token = ++m_lastEvent;
        m_lambdas[token] = std::unique_ptr<EventHandlerBase>( new EventHandler<Func>(
                                                                  *this, eventType, std::forward<Func>( f ), wrapper ) );
        return token;
    }

    template <typename Func>
    EventToken handle(libvlc_event_e eventType, Func&& f)
    {
        EXPECT_SIGNATURE(void());
        return handle(eventType, std::forward<Func>( f ), [](const libvlc_event_t*, void* data)
        {
            auto callback = static_cast<DecayPtr<Func>>( data );
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

        /**
         * @brief onMetaChanged
         * @param f A std::function<void(libvlc_meta_t)>
         */
        template <typename Func>
        EventToken onMetaChanged( Func&& f)
        {
            EXPECT_SIGNATURE(void(libvlc_meta_t));
            return handle( libvlc_MediaMetaChanged, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>(data);
                (*callback)( e->u.media_meta_changed.meta_type );
            });
        }

        /**
         * @brief onSubItemAdded
         * @param f A std::function<void(MediaPtr)>
         */
        template <typename Func>
        EventToken onSubItemAdded( Func&& f )
        {
            EXPECT_SIGNATURE(void(MediaPtr));
            return handle(libvlc_MediaSubItemAdded, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>(data);
                (*callback)( std::make_shared<Media>( e->u.media_subitem_added.new_child, true ) );
            });
        }

        template <typename Func>
        EventToken onDurationChanged( Func&& f )
        {
            EXPECT_SIGNATURE(void(int64_t));
            return handle(libvlc_MediaDurationChanged, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>(data);
                (*callback)( e->u.media_duration_changed.new_duration );
            });
        }

        template <typename Func>
        EventToken onParsedChanged( Func&& f )
        {
            EXPECT_SIGNATURE(void(bool));
            return handle( libvlc_MediaParsedChanged, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>(data);
                (*callback)( e->u.media_parsed_changed.new_status );
            });
        }

        template <typename Func>
        EventToken onFreed( Func&& f)
        {
            EXPECT_SIGNATURE(void(MediaPtr));
            return handle(libvlc_MediaFreed, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>(data);
                (*callback)( std::make_shared<Media>( e->u.media_freed.md, true ) );
            });
        }

        template <typename Func>
        EventToken onStateChanged( Func&& f)
        {
            EXPECT_SIGNATURE(void(libvlc_state_t));
            return handle(libvlc_MediaStateChanged, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>(data);
                (*callback)( e->u.media_state_changed.new_state );
            });
        }

        template <typename Func>
        EventToken onSubItemTreeAdded( Func&& f)
        {
            EXPECT_SIGNATURE(void(MediaPtr));
            return handle(libvlc_MediaSubItemAdded, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>(data);
                (*callback)( std::make_shared<Media>( e->u.media_subitemtree_added.item, true ) );
            });
        }
};

class MediaPlayerEventManager : public EventManager
{
    public:
        MediaPlayerEventManager(InternalPtr ptr) : EventManager( ptr ) {}

        template <typename Func>
        EventToken onMediaChanged( Func&& f )
        {
            EXPECT_SIGNATURE(void(MediaPtr));
            return handle(libvlc_MediaPlayerMediaChanged, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( std::make_shared<Media>( e->u.media_player_media_changed.new_media, true ) );
            });
        }

        template <typename Func>
        EventToken onNothingSpecial( Func&& f )
        {
            return handle(libvlc_MediaPlayerNothingSpecial, std::forward<Func>( f ));
        }

        template <typename Func>
        EventToken onOpening( Func&& f )
        {
            return handle(libvlc_MediaPlayerOpening, std::forward<Func>( f ) );
        }

        template <typename Func>
        EventToken onBuffering( Func&& f )
        {
            return handle(libvlc_MediaPlayerBuffering, std::forward<Func>( f ) );
        }

        template <typename Func>
        EventToken onPlaying( Func&& f )
        {
            return handle( libvlc_MediaPlayerPlaying, std::forward<Func>( f ) );
        }

        template <typename Func>
        EventToken onPaused(Func&& f)
        {
            return handle( libvlc_MediaPlayerPaused, std::forward<Func>( f ) );
        }

        template <typename Func>
        EventToken onStopped(Func&& f)
        {
            return handle( libvlc_MediaPlayerStopped, std::forward<Func>( f ) );
        }

        template <typename Func>
        EventToken onForward(Func&& f)
        {
            return handle( libvlc_MediaPlayerForward, std::forward<Func>( f ) );
        }

        template <typename Func>
        EventToken onBackward(Func&& f)
        {
            return handle( libvlc_MediaPlayerBackward, std::forward<Func>( f ) );
        }

        template <typename Func>
        EventToken onEndReached(Func&& f)
        {
            return handle( libvlc_MediaPlayerEndReached, std::forward<Func>( f ) );
        }

        template <typename Func>
        EventToken onEncounteredError(Func&& f)
        {
            return handle( libvlc_MediaPlayerEncounteredError, std::forward<Func>( f ) );
        }

        template <typename Func>
        EventToken onTimeChanged( Func&& f )
        {
            EXPECT_SIGNATURE(void(libvlc_time_t));
            return handle( libvlc_MediaPlayerTimeChanged, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.media_player_time_changed.new_time );
            });
        }

        template <typename Func>
        EventToken onPositionChanged( Func&& f )
        {
            EXPECT_SIGNATURE(void(float));
            return handle( libvlc_MediaPlayerPositionChanged, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.media_player_position_changed.new_position );
            });
        }

        template <typename Func>
        EventToken onSeekableChanged( Func&& f )
        {
            EXPECT_SIGNATURE(void(bool));
            return handle( libvlc_MediaPlayerSeekableChanged, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.media_player_seekable_changed.new_seekable );
            });
        }

        template <typename Func>
        EventToken onPausableChanged( Func&& f )
        {
            EXPECT_SIGNATURE(void(bool));
            return handle( libvlc_MediaPlayerSeekableChanged, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.media_player_seekable_changed.new_seekable );
            });
        }

        template <typename Func>
        EventToken onTitleChanged( Func&& f )
        {
            EXPECT_SIGNATURE(void(int));
            return handle( libvlc_MediaPlayerTitleChanged, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.media_player_title_changed.new_title );
            });
        }

        template <typename Func>
        EventToken onSnapshotTaken( Func&& f )
        {
            EXPECT_SIGNATURE(void(std::string));
            return handle( libvlc_MediaPlayerSnapshotTaken, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.media_player_snapshot_taken.psz_filename );
            });
        }

        template <typename Func>
        EventToken onLengthChanged( Func&& f )
        {
            EXPECT_SIGNATURE(void(libvlc_time_t));
            return handle( libvlc_MediaPlayerLengthChanged, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.media_player_length_changed.new_length );
            });
        }

        template <typename Func>
        EventToken onVout( Func&& f )
        {
            EXPECT_SIGNATURE(void(int));
            return handle( libvlc_MediaPlayerVout, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.media_player_vout.new_count );
            });
        }

        template <typename Func>
        EventToken onScrambledChanged( Func&& f )
        {
            EXPECT_SIGNATURE(void(int));
            return handle( libvlc_MediaPlayerScrambledChanged, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.media_player_scrambled_changed.new_scrambled );
            });
        }

        template <typename Func>
        EventToken onESAdded( Func&& f )
        {
            EXPECT_SIGNATURE(void(libvlc_track_type_t, int));
            return handle( libvlc_MediaPlayerESAdded, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.media_player_es_changed.i_type, e->u.media_player_es_changed.i_id );
            });
        }

        template <typename Func>
        EventToken onESDeleted( Func&& f )
        {
            EXPECT_SIGNATURE(void(libvlc_track_type_t, int));
            return handle( libvlc_MediaPlayerESDeleted, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.media_player_es_changed.i_type, e->u.media_player_es_changed.i_id );
            });
        }

        template <typename Func>
        EventToken onESSelected( Func&& f )
        {
            EXPECT_SIGNATURE(void(libvlc_track_type_t, int));
            return handle( libvlc_MediaPlayerESSelected, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.media_player_es_changed.i_type, e->u.media_player_es_changed.i_id );
            });
        }
};

class MediaListEventManager : public EventManager
{
    public:
        MediaListEventManager(InternalPtr ptr) : EventManager( ptr ) {}

        template <typename Func>
        EventToken onItemAdded( Func&& f )
        {
            EXPECT_SIGNATURE(void(MediaPtr, int));
            return handle(libvlc_MediaListItemAdded, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( std::make_shared<Media>( e->u.media_list_item_added.item, true ), e->u.media_list_item_added.index );
            });
        }

        template <typename Func>
        EventToken onWillAddItem( Func&& f )
        {
            EXPECT_SIGNATURE(void(MediaPtr, int));
            return handle( libvlc_MediaListWillAddItem, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)(std::make_shared<Media>( e->u.media_list_will_add_item.item, true ), e->u.media_list_will_add_item.index );
            });
        }

        template <typename Func>
        EventToken onItemDeleted( Func&& f )
        {
            EXPECT_SIGNATURE(void(MediaPtr, int));
            return handle(libvlc_MediaListItemDeleted, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( std::make_shared<Media>( e->u.media_list_item_deleted.item, true ), e->u.media_list_item_deleted.index );
            });
        }

        template <typename Func>
        EventToken onWillDeleteItem( Func&& f )
        {
            EXPECT_SIGNATURE(void(MediaPtr, int));
            return handle(libvlc_MediaListWillDeleteItem, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( std::make_shared<Media>( e->u.media_list_will_delete_item.item, true ), e->u.media_list_will_delete_item.index );
            });
        }
};

// MediaListView events are not being sent by VLC, so we don't implement them here

class MediaListPlayerEventManager : public EventManager
{
    public:
        MediaListPlayerEventManager(InternalPtr ptr) : EventManager( ptr ) {}

        template <typename Func>
        EventToken onPlayed(Func&& f)
        {
            return handle(libvlc_MediaListPlayerPlayed, std::forward<Func>( f ) );
        }

        template <typename Func>
        EventToken onNextItemSet( Func&& f )
        {
            EXPECT_SIGNATURE(void(MediaPtr));
            return handle(libvlc_MediaListPlayerNextItemSet, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( std::make_shared<Media>( e->u.media_list_player_next_item_set.item, true ) );
            });
        }

        template <typename Func>
        EventToken onStopped( Func&& f )
        {
            return handle(libvlc_MediaListPlayerStopped, std::forward<Func>( f ) );
        }
};

class MediaDiscovererEventManager : public EventManager
{
    public:
        MediaDiscovererEventManager(InternalPtr ptr) : EventManager( ptr ) {}

        template <typename Func>
        EventToken onStarted(Func&& f)
        {
            return handle(libvlc_MediaDiscovererStarted, std::forward<Func>( f ) );
        }

        template <typename Func>
        EventToken onStopped(Func&& f)
        {
            return handle(libvlc_MediaDiscovererEnded, std::forward<Func>( f ) );
        }
};

class VLMEventManager : public EventManager
{
    public:
        using EventManager::EventManager;

        template <typename Func>
        EventToken onMediaAdded( Func&& f )
        {
            EXPECT_SIGNATURE(void(std::string));
            return handle(libvlc_VlmMediaAdded, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.vlm_media_event.psz_media_name ? e->u.vlm_media_event.psz_media_name : "" );
            });
        }

        template <typename Func>
        EventToken onMediaRemoved( Func&& f )
        {
            EXPECT_SIGNATURE(void(std::string));
            return handle(libvlc_VlmMediaRemoved, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.vlm_media_event.psz_media_name ? e->u.vlm_media_event.psz_media_name : "" );
            });
        }

        template <typename Func>
        EventToken onMediaChanged( Func&& f )
        {
            EXPECT_SIGNATURE(void(std::string));
            return handle(libvlc_VlmMediaChanged, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.vlm_media_event.psz_media_name ? e->u.vlm_media_event.psz_media_name : "" );
            });
        }

        template <typename Func>
        EventToken onMediaInstanceStarted( Func&& f )
        {
            EXPECT_SIGNATURE(void(std::string, std::string));
            return handle(libvlc_VlmMediaInstanceStarted, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.vlm_media_event.psz_media_name ? e->u.vlm_media_event.psz_media_name : "",
                             e->u.vlm_media_event.psz_instance_name ? e->u.vlm_media_event.psz_instance_name : "" );
            });
        }

        template <typename Func>
        EventToken onMediaInstanceStopped( Func&& f )
        {
            EXPECT_SIGNATURE(void(std::string, std::string));
            return handle(libvlc_VlmMediaInstanceStopped, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.vlm_media_event.psz_media_name ? e->u.vlm_media_event.psz_media_name : "",
                             e->u.vlm_media_event.psz_instance_name ? e->u.vlm_media_event.psz_instance_name : "" );
            });
        }

        template <typename Func>
        EventToken onMediaInstanceStatusInit( Func&& f )
        {
            EXPECT_SIGNATURE(void(std::string, std::string));
            return handle(libvlc_VlmMediaInstanceStatusInit, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.vlm_media_event.psz_media_name ? e->u.vlm_media_event.psz_media_name : "",
                             e->u.vlm_media_event.psz_instance_name ? e->u.vlm_media_event.psz_instance_name : "" );
            });
        }

        template <typename Func>
        EventToken onMediaInstanceStatusOpening( Func&& f )
        {
            EXPECT_SIGNATURE(void(std::string, std::string));
            return handle(libvlc_VlmMediaInstanceStatusOpening, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.vlm_media_event.psz_media_name ? e->u.vlm_media_event.psz_media_name : "",
                             e->u.vlm_media_event.psz_instance_name ? e->u.vlm_media_event.psz_instance_name : "" );
            });
        }

        template <typename Func>
        EventToken onMediaInstanceStatusPlaying( Func&& f )
        {
            EXPECT_SIGNATURE(void(std::string, std::string));
            return handle(libvlc_VlmMediaInstanceStatusPlaying, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.vlm_media_event.psz_media_name ? e->u.vlm_media_event.psz_media_name : "",
                             e->u.vlm_media_event.psz_instance_name ? e->u.vlm_media_event.psz_instance_name : "" );
            });
        }

        template <typename Func>
        EventToken onMediaInstanceStatusPause( Func&& f )
        {
            EXPECT_SIGNATURE(void(std::string, std::string));
            return handle(libvlc_VlmMediaInstanceStatusPause, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.vlm_media_event.psz_media_name ? e->u.vlm_media_event.psz_media_name : "",
                             e->u.vlm_media_event.psz_instance_name ? e->u.vlm_media_event.psz_instance_name : "" );
            });
        }

        template <typename Func>
        EventToken onMediaInstanceStatusEnd( Func&& f )
        {
            EXPECT_SIGNATURE(void(std::string, std::string));
            return handle(libvlc_VlmMediaInstanceStatusEnd, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.vlm_media_event.psz_media_name ? e->u.vlm_media_event.psz_media_name : "",
                             e->u.vlm_media_event.psz_instance_name ? e->u.vlm_media_event.psz_instance_name : "" );
            });
        }

        template <typename Func>
        EventToken onMediaInstanceStatusError( Func&& f )
        {
            EXPECT_SIGNATURE(void(std::string, std::string));
            return handle(libvlc_VlmMediaInstanceStatusError, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.vlm_media_event.psz_media_name ? e->u.vlm_media_event.psz_media_name : "",
                             e->u.vlm_media_event.psz_instance_name ? e->u.vlm_media_event.psz_instance_name : "" );
            });
        }
};
}

#endif // LIBVLC_EVENTMANAGER_HPP
