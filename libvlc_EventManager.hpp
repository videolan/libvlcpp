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
#include <vlc/vlc.h>
#include "libvlcpp.hpp"

namespace VLC
{

class Media;

class IMediaEventCb
{
    public:
        virtual ~IMediaEventCb() {}
        virtual void metaChanged( libvlc_meta_t ) {}
        virtual void subItemAdded( const Media& ) {}
        virtual void durationChanged( int64_t ) {}
        virtual void parsedChanged( bool ) {}
        virtual void freed( const Media& ) {}
        virtual void stateChanged( libvlc_state_t ) {}
        virtual void subItemTreeAdded( const Media& ) {}
};

class IMediaPlayerEventCb
{
    public:
        virtual ~IMediaPlayerEventCb(){}
        virtual void mediaChanged( const Media& ) {}
        virtual void nothingSpecial() {}
        virtual void opening() {}
        virtual void buffering( float ) {}
        virtual void playing() {}
        virtual void paused() {}
        virtual void stopped() {}
        virtual void forward() {}
        virtual void backward() {}
        virtual void endReached() {}
        virtual void encounteredError() {}
        virtual void timeChanged( libvlc_time_t ) {}
        virtual void positionChanged( float ) {}
        virtual void seekableChanged( bool ) {}
        virtual void pausableChanged( bool ) {}
        virtual void titleChanged( int ) {}
        virtual void snapshotTaken( const std::string& ) {}
        virtual void lengthChanged( libvlc_time_t ) {}
        virtual void vout( int ) {}
        virtual void scrambledChanged( int ) {}
        virtual void eSAdded( libvlc_track_type_t, int ) {}
        virtual void eSDeleted( libvlc_track_type_t, int ) {}
        virtual void eSSelected( libvlc_track_type_t, int ) {}
};

class IMediaListEventCb
{
    public:
        virtual ~IMediaListEventCb() {}
        virtual void itemAdded( const Media&, int ) {}
        virtual void willAddItem( const Media&, int ) {}
        virtual void itemDeleted( const Media&, int ) {}
        virtual void willDeleteItem( const Media&, int ) {}
};

// MediaListView events are not being sent by VLC, so we don't implement them here

class IMediaListPlayerEventCb
{
    public:
        virtual ~IMediaListPlayerEventCb() {}
        virtual void played() {}
        virtual void nextItemSet( const Media& ) {}
        virtual void stopped() {}
};

class IMediaDiscovererEventCb
{
    public:
        virtual ~IMediaDiscovererEventCb() {}
        virtual void started() {}
        virtual void stopped() {}
};

class IVLMEventCb
{
    public:
        virtual ~IVLMEventCb() {}
        virtual void mediaAdded( const std::string& ) {}
        virtual void mediaRemoved( const std::string& ) {}
        virtual void mediaChanged( const std::string& ) {}
        virtual void mediaInstanceStarted( const std::string&, const std::string& ) {}
        virtual void mediaInstanceStopped( const std::string&, const std::string& ) {}
        virtual void mediaInstanceStatusInit( const std::string&, const std::string& ) {}
        virtual void mediaInstanceStatusOpening( const std::string&, const std::string& ) {}
        virtual void mediaInstanceStatusPlaying( const std::string&, const std::string& ) {}
        virtual void mediaInstanceStatusPause( const std::string&, const std::string& ) {}
        virtual void mediaInstanceStatusEnd( const std::string&, const std::string& ) {}
        virtual void mediaInstanceStatusError( const std::string&, const std::string& ) {}
};

class EventManager : Internal<libvlc_event_manager_t>
{
    public:
        EventManager( const EventManager& em );
        bool attach( libvlc_event_type_t type, IMediaEventCb* cb );
        bool attach( libvlc_event_type_t type, IMediaPlayerEventCb* cb );
        bool attach( libvlc_event_type_t type, IMediaListEventCb* cb );
        bool attach( libvlc_event_type_t type, IMediaListPlayerEventCb* cb );
        bool attach( libvlc_event_type_t type, IMediaDiscovererEventCb* cb );
        bool attach( libvlc_event_type_t type, IVLMEventCb* cb );

        void detach( libvlc_event_type_t type, IMediaEventCb* cb );
        void detach( libvlc_event_type_t type, IMediaPlayerEventCb* cb );
        void detach( libvlc_event_type_t type, IMediaListEventCb* cb );
        void detach( libvlc_event_type_t type, IMediaListPlayerEventCb* cb );
        void detach( libvlc_event_type_t type, IMediaDiscovererEventCb* cb );
        void detach( libvlc_event_type_t type, IVLMEventCb* cb );

    private:
        static void handleMediaEvent( const libvlc_event_t* event, void* data );
        static void handleMediaPlayerEvent( const libvlc_event_t* event, void* data );
        static void handleMediaListEvent( const libvlc_event_t* event, void* data );
        static void handleMediaListPlayerEvent( const libvlc_event_t* event, void* data );
        static void handleMediaDiscovererEvent( const libvlc_event_t* event, void* data );
        static void handleVLMEvent( const libvlc_event_t* event, void* data );

        static void handleEvent( const libvlc_event_t* event, void* data );

        EventManager(InternalPtr obj );
        friend class Media;
        friend class MediaDiscoverer;
        friend class MediaList;
        friend class MediaListPlayer;
        friend class MediaPlayer;
};

}

#endif // LIBVLC_EVENTMANAGER_HPP
