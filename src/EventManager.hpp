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

#include <algorithm>
#include <functional>
#include <vector>
#include <memory>

#define EXPECT_SIGNATURE(sig) static_assert(signature_match<decltype(f), sig>::value, "Expected a function with prototype " #sig)

namespace VLC
{

class Media;

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
    using DecayPtr = typename std::add_pointer<typename std::decay<T>::type>::type;

    template <typename, typename, typename = void>
    struct signature_match : std::false_type {};

    // Kudos to 3xxO
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
        virtual void unregister() = 0;
    };

    using RegisteredEvent = EventHandlerBase&;

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
            // We unregister only when the object actually goes out of scope, IE. when it is
            // removed from EventManager's event handler vector
            libvlc_event_detach( *m_eventManager, m_eventType, m_wrapper, &m_userCallback );
        }

        virtual void unregister() override
        {
            m_eventManager->unregister(*this);
        }

        EventHandler(const EventHandler&) = delete;

    private:
        // Deduced type is Func& in case of lvalue; Func in case of rvalue.
        // So we hold a reference to the callback when passed a lvalue, we
        // copy construct it when passing a rvalue
        Func m_userCallback;
        // EventManager always outlive EventHandler, no need for smart pointer
        EventManager* m_eventManager;
        Wrapper m_wrapper;
        libvlc_event_e m_eventType;
    };

private:
    // variadic template recursion termination
    void unregister(){}

public:
    template <typename T, typename... Args>
    void unregister(const T& e, const Args&... args)
    {
        static_assert(std::is_convertible<decltype(e), const EventHandlerBase&>::value, "Expected const RegisteredEvent");

        {
            auto it = std::find_if(begin(m_lambdas), end(m_lambdas), [&e](decltype(m_lambdas)::value_type &value) {
                return &e == value.get();
            });
            if (it != end(m_lambdas))
                m_lambdas.erase( it );
        }

        unregister(args...);
    }

protected:
    EventManager(InternalPtr ptr)
        : Internal{ ptr, [](InternalPtr){ /* No-op; EventManager's are handled by their respective objects */ } }
    {
    }

public:
    EventManager(const EventManager& em)
        : Internal( em )
    {
        // Use an empty list of events
    }

    EventManager& operator=(const EventManager& em)
    {
        Internal::operator=(em);
        return *this;
    }

    EventManager(EventManager&&) = default;
    EventManager& operator=(EventManager&&) = default;

protected:
    /**
     * @brief handle        Provides the common behavior for all event handlers
     * @param eventType     The libvlc type of event
     * @param f             The user provided std::function. This has to either match the expected parameter list
     *                      exactly, or it can take no arguments.
     * @param wrapper       Our implementation defined wrapper around the user's callback. It is expected to be able
     *                      able to decay to a regular C-style function pointer. It is currently implemented as a
     *                      captureless lambda (§5.1.2)
     * @return              A reference to an abstract EventHandler type. It is assumed that the EventManager will
     *                      outlive this reference. When EventManager::~EventManager is called, it will destroy all
     *                      the registered event handler, this making this reference a dangling reference, which is
     *                      undefined behavior.
     *                      When calling unregister() on this object, the reference should immediatly be considered invalid.
     */
    template <typename Func>
    RegisteredEvent handle(libvlc_event_e eventType, Func&& f, EventHandlerBase::Wrapper wrapper)
    {
        auto ptr = std::unique_ptr<EventHandlerBase>( new EventHandler<Func>(
                                *this, eventType, std::forward<Func>( f ), wrapper ) );
        auto raw = ptr.get();
        m_lambdas.push_back( std::move( ptr ) );
        return *raw;
    }

    template <typename Func>
    RegisteredEvent handle(libvlc_event_e eventType, Func&& f)
    {
        EXPECT_SIGNATURE(void());
        return handle(eventType, std::forward<Func>( f ), [](const libvlc_event_t*, void* data)
        {
            auto callback = static_cast<DecayPtr<Func>>( data );
            (*callback)();
        });
    }

protected:
    // We store the EventHandlerBase's as unique_ptr in order for the function stored within
    // EventHandler<T> not to move to another memory location
    std::vector<std::unique_ptr<EventHandlerBase>> m_lambdas;
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
        RegisteredEvent onMetaChanged( Func&& f)
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
        RegisteredEvent onSubItemAdded( Func&& f )
        {
            EXPECT_SIGNATURE(void(MediaPtr));
            return handle(libvlc_MediaSubItemAdded, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>(data);
                (*callback)( std::make_shared<Media>( e->u.media_subitem_added.new_child, true ) );
            });
        }

        template <typename Func>
        RegisteredEvent onDurationChanged( Func&& f )
        {
            EXPECT_SIGNATURE(void(int64_t));
            return handle(libvlc_MediaDurationChanged, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>(data);
                (*callback)( e->u.media_duration_changed.new_duration );
            });
        }

        template <typename Func>
        RegisteredEvent onParsedChanged( Func&& f )
        {
            EXPECT_SIGNATURE(void(bool));
            return handle( libvlc_MediaParsedChanged, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>(data);
                (*callback)( e->u.media_parsed_changed.new_status );
            });
        }

        template <typename Func>
        RegisteredEvent onFreed( Func&& f)
        {
            EXPECT_SIGNATURE(void(MediaPtr));
            return handle(libvlc_MediaFreed, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>(data);
                (*callback)( std::make_shared<Media>( e->u.media_freed.md, true ) );
            });
        }

        template <typename Func>
        RegisteredEvent onStateChanged( Func&& f)
        {
            EXPECT_SIGNATURE(void(libvlc_state_t));
            return handle(libvlc_MediaStateChanged, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>(data);
                (*callback)( e->u.media_state_changed.new_state );
            });
        }

        template <typename Func>
        RegisteredEvent onSubItemTreeAdded( Func&& f)
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
        RegisteredEvent onMediaChanged( Func&& f )
        {
            EXPECT_SIGNATURE(void(MediaPtr));
            return handle(libvlc_MediaPlayerMediaChanged, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( std::make_shared<Media>( e->u.media_player_media_changed.new_media, true ) );
            });
        }

        template <typename Func>
        RegisteredEvent onNothingSpecial( Func&& f )
        {
            return handle(libvlc_MediaPlayerNothingSpecial, std::forward<Func>( f ));
        }

        template <typename Func>
        RegisteredEvent onOpening( Func&& f )
        {
            return handle(libvlc_MediaPlayerOpening, std::forward<Func>( f ) );
        }

        template <typename Func>
        RegisteredEvent onBuffering( Func&& f )
        {
            return handle(libvlc_MediaPlayerBuffering, std::forward<Func>( f ) );
        }

        template <typename Func>
        RegisteredEvent onPlaying( Func&& f )
        {
            return handle( libvlc_MediaPlayerPlaying, std::forward<Func>( f ) );
        }

        template <typename Func>
        RegisteredEvent onPaused(Func&& f)
        {
            return handle( libvlc_MediaPlayerPaused, std::forward<Func>( f ) );
        }

        template <typename Func>
        RegisteredEvent onStopped(Func&& f)
        {
            return handle( libvlc_MediaPlayerStopped, std::forward<Func>( f ) );
        }

        template <typename Func>
        RegisteredEvent onForward(Func&& f)
        {
            return handle( libvlc_MediaPlayerForward, std::forward<Func>( f ) );
        }

        template <typename Func>
        RegisteredEvent onBackward(Func&& f)
        {
            return handle( libvlc_MediaPlayerBackward, std::forward<Func>( f ) );
        }

        template <typename Func>
        RegisteredEvent onEndReached(Func&& f)
        {
            return handle( libvlc_MediaPlayerEndReached, std::forward<Func>( f ) );
        }

        template <typename Func>
        RegisteredEvent onEncounteredError(Func&& f)
        {
            return handle( libvlc_MediaPlayerEncounteredError, std::forward<Func>( f ) );
        }

        template <typename Func>
        RegisteredEvent onTimeChanged( Func&& f )
        {
            EXPECT_SIGNATURE(void(libvlc_time_t));
            return handle( libvlc_MediaPlayerTimeChanged, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.media_player_time_changed.new_time );
            });
        }

        template <typename Func>
        RegisteredEvent onPositionChanged( Func&& f )
        {
            EXPECT_SIGNATURE(void(float));
            return handle( libvlc_MediaPlayerPositionChanged, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.media_player_position_changed.new_position );
            });
        }

        template <typename Func>
        RegisteredEvent onSeekableChanged( Func&& f )
        {
            EXPECT_SIGNATURE(void(bool));
            return handle( libvlc_MediaPlayerSeekableChanged, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.media_player_seekable_changed.new_seekable );
            });
        }

        template <typename Func>
        RegisteredEvent onPausableChanged( Func&& f )
        {
            EXPECT_SIGNATURE(void(bool));
            return handle( libvlc_MediaPlayerSeekableChanged, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.media_player_seekable_changed.new_seekable );
            });
        }

        template <typename Func>
        RegisteredEvent onTitleChanged( Func&& f )
        {
            EXPECT_SIGNATURE(void(int));
            return handle( libvlc_MediaPlayerTitleChanged, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.media_player_title_changed.new_title );
            });
        }

        template <typename Func>
        RegisteredEvent onSnapshotTaken( Func&& f )
        {
            EXPECT_SIGNATURE(void(std::string));
            return handle( libvlc_MediaPlayerSnapshotTaken, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.media_player_snapshot_taken.psz_filename );
            });
        }

        template <typename Func>
        RegisteredEvent onLengthChanged( Func&& f )
        {
            EXPECT_SIGNATURE(void(libvlc_time_t));
            return handle( libvlc_MediaPlayerLengthChanged, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.media_player_length_changed.new_length );
            });
        }

        template <typename Func>
        RegisteredEvent onVout( Func&& f )
        {
            EXPECT_SIGNATURE(void(int));
            return handle( libvlc_MediaPlayerVout, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.media_player_vout.new_count );
            });
        }

        template <typename Func>
        RegisteredEvent onScrambledChanged( Func&& f )
        {
            EXPECT_SIGNATURE(void(int));
            return handle( libvlc_MediaPlayerScrambledChanged, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.media_player_scrambled_changed.new_scrambled );
            });
        }

        template <typename Func>
        RegisteredEvent onESAdded( Func&& f )
        {
            EXPECT_SIGNATURE(void(libvlc_track_type_t, int));
            return handle( libvlc_MediaPlayerESAdded, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.media_player_es_changed.i_type, e->u.media_player_es_changed.i_id );
            });
        }

        template <typename Func>
        RegisteredEvent onESDeleted( Func&& f )
        {
            EXPECT_SIGNATURE(void(libvlc_track_type_t, int));
            return handle( libvlc_MediaPlayerESDeleted, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.media_player_es_changed.i_type, e->u.media_player_es_changed.i_id );
            });
        }

        template <typename Func>
        RegisteredEvent onESSelected( Func&& f )
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
        RegisteredEvent onItemAdded( Func&& f )
        {
            EXPECT_SIGNATURE(void(MediaPtr, int));
            return handle(libvlc_MediaListItemAdded, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( std::make_shared<Media>( e->u.media_list_item_added.item, true ), e->u.media_list_item_added.index );
            });
        }

        template <typename Func>
        RegisteredEvent onWillAddItem( Func&& f )
        {
            EXPECT_SIGNATURE(void(MediaPtr, int));
            return handle( libvlc_MediaListWillAddItem, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)(std::make_shared<Media>( e->u.media_list_will_add_item.item, true ), e->u.media_list_will_add_item.index );
            });
        }

        template <typename Func>
        RegisteredEvent onItemDeleted( Func&& f )
        {
            EXPECT_SIGNATURE(void(MediaPtr, int));
            return handle(libvlc_MediaListItemDeleted, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( std::make_shared<Media>( e->u.media_list_item_deleted.item, true ), e->u.media_list_item_deleted.index );
            });
        }

        template <typename Func>
        RegisteredEvent onWillDeleteItem( Func&& f )
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
        RegisteredEvent onPlayed(Func&& f)
        {
            return handle(libvlc_MediaListPlayerPlayed, std::forward<Func>( f ) );
        }

        template <typename Func>
        RegisteredEvent onNextItemSet( Func&& f )
        {
            EXPECT_SIGNATURE(void(MediaPtr));
            return handle(libvlc_MediaListPlayerNextItemSet, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( std::make_shared<Media>( e->u.media_list_player_next_item_set.item, true ) );
            });
        }

        template <typename Func>
        RegisteredEvent onStopped( Func&& f )
        {
            return handle(libvlc_MediaListPlayerStopped, std::forward<Func>( f ) );
        }
};

class MediaDiscovererEventManager : public EventManager
{
    public:
        MediaDiscovererEventManager(InternalPtr ptr) : EventManager( ptr ) {}

        template <typename Func>
        RegisteredEvent onStarted(Func&& f)
        {
            return handle(libvlc_MediaDiscovererStarted, std::forward<Func>( f ) );
        }

        template <typename Func>
        RegisteredEvent onStopped(Func&& f)
        {
            return handle(libvlc_MediaDiscovererEnded, std::forward<Func>( f ) );
        }
};

class VLMEventManager : public EventManager
{
    public:
        using EventManager::EventManager;

        template <typename Func>
        RegisteredEvent onMediaAdded( Func&& f )
        {
            EXPECT_SIGNATURE(void(std::string));
            return handle(libvlc_VlmMediaAdded, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.vlm_media_event.psz_media_name ? e->u.vlm_media_event.psz_media_name : "" );
            });
        }

        template <typename Func>
        RegisteredEvent onMediaRemoved( Func&& f )
        {
            EXPECT_SIGNATURE(void(std::string));
            return handle(libvlc_VlmMediaRemoved, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.vlm_media_event.psz_media_name ? e->u.vlm_media_event.psz_media_name : "" );
            });
        }

        template <typename Func>
        RegisteredEvent onMediaChanged( Func&& f )
        {
            EXPECT_SIGNATURE(void(std::string));
            return handle(libvlc_VlmMediaChanged, std::forward<Func>( f ), [](const libvlc_event_t* e, void* data)
            {
                auto callback = static_cast<DecayPtr<Func>>( data );
                (*callback)( e->u.vlm_media_event.psz_media_name ? e->u.vlm_media_event.psz_media_name : "" );
            });
        }

        template <typename Func>
        RegisteredEvent onMediaInstanceStarted( Func&& f )
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
        RegisteredEvent onMediaInstanceStopped( Func&& f )
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
        RegisteredEvent onMediaInstanceStatusInit( Func&& f )
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
        RegisteredEvent onMediaInstanceStatusOpening( Func&& f )
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
        RegisteredEvent onMediaInstanceStatusPlaying( Func&& f )
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
        RegisteredEvent onMediaInstanceStatusPause( Func&& f )
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
        RegisteredEvent onMediaInstanceStatusEnd( Func&& f )
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
        RegisteredEvent onMediaInstanceStatusError( Func&& f )
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
