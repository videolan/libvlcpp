/*****************************************************************************
 * libvlc_EventManager.cpp: Exposes libvlc events
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
#include <vlc/libvlc_events.h>

namespace VLC
{

EventManager::EventManager(const EventManager& em)
    : Internal( em )
{
}

bool EventManager::attach(libvlc_event_type_t type, IMediaEventCb* cb)
{
    if ( cb == NULL )
        return false;
    if ( type < libvlc_MediaMetaChanged || type > libvlc_MediaSubItemTreeAdded )
        return false;
    libvlc_event_attach( *this, type, &handleMediaEvent, cb );
    return true;
}

bool EventManager::attach(libvlc_event_type_t type, IMediaPlayerEventCb* cb)
{
    if ( cb == NULL )
        return false;
    if ( type < libvlc_MediaPlayerMediaChanged || type > libvlc_MediaPlayerESSelected )
        return false;
    libvlc_event_attach( *this, type, &handleMediaPlayerEvent, cb );
    return true;
}

bool EventManager::attach(libvlc_event_type_t type, IMediaListEventCb* cb)
{
    if ( cb == NULL )
        return false;
    if ( type < libvlc_MediaListItemAdded || type > libvlc_MediaListWillDeleteItem )
        return false;
    libvlc_event_attach( *this, type, &handleMediaListEvent, cb );
    return true;
}

bool EventManager::attach(libvlc_event_type_t type, IMediaListPlayerEventCb* cb)
{
    if ( cb == NULL )
        return false;
    if ( type < libvlc_MediaListPlayerPlayed || type > libvlc_MediaListPlayerStopped )
        return false;
    libvlc_event_attach( *this, type, &handleMediaListPlayerEvent, cb );
    return true;
}

bool EventManager::attach(libvlc_event_type_t type, IMediaDiscovererEventCb* cb)
{
    if ( cb == NULL )
        return false;
    if ( type < libvlc_MediaDiscovererStarted || type > libvlc_MediaDiscovererEnded )
        return false;
    libvlc_event_attach( *this, type, &handleMediaDiscovererEvent, cb );
    return true;
}

bool EventManager::attach( libvlc_event_type_t type, IVLMEventCb* cb )
{
    if ( cb == NULL )
        return false;
    if ( type < libvlc_VlmMediaAdded || type > libvlc_VlmMediaInstanceStatusError )
        return false;
    libvlc_event_attach( *this, type, &handleVLMEvent, cb );
    return true;
}

void EventManager::detach(libvlc_event_type_t type, IMediaEventCb* cb)
{
    libvlc_event_detach( *this, type, &handleMediaEvent, cb );
}

void EventManager::detach(libvlc_event_type_t type, IMediaPlayerEventCb* cb)
{
    libvlc_event_detach( *this, type, &handleMediaPlayerEvent, cb );
}

void EventManager::detach(libvlc_event_type_t type, IMediaListEventCb* cb)
{
    libvlc_event_detach( *this, type, &handleMediaListEvent, cb );
}

void EventManager::detach(libvlc_event_type_t type, IMediaListPlayerEventCb* cb)
{
    libvlc_event_detach( *this, type, &handleMediaListPlayerEvent, cb );
}

void EventManager::detach(libvlc_event_type_t type, IMediaDiscovererEventCb* cb)
{
    libvlc_event_detach( *this, type, &handleMediaDiscovererEvent, cb );
}

void EventManager::detach(libvlc_event_type_t type, IVLMEventCb* cb)
{
    libvlc_event_detach( *this, type, &handleVLMEvent, cb );
}

void EventManager::handleMediaEvent(const libvlc_event_t* event, void* data)
{
    IMediaEventCb* cb = reinterpret_cast<IMediaEventCb*>( data );
    switch ( event->type )
    {
        case libvlc_MediaMetaChanged:
            cb->metaChanged( event->u.media_meta_changed.meta_type );
            break;
        case libvlc_MediaSubItemAdded:
            cb->subItemAdded( Media( event->u.media_subitem_added.new_child, true ) );
            break;
        case libvlc_MediaDurationChanged:
            cb->durationChanged( event->u.media_duration_changed.new_duration );
            break;
        case libvlc_MediaParsedChanged:
            cb->parsedChanged( event->u.media_parsed_changed.new_status );
            break;
        case libvlc_MediaFreed:
            cb->freed( Media( event->u.media_freed.md, true ) );
            break;
        case libvlc_MediaStateChanged:
            cb->stateChanged( event->u.media_state_changed.new_state );
            break;
        case libvlc_MediaSubItemTreeAdded:
            cb->subItemTreeAdded( Media( event->u.media_subitemtree_added.item, true ) );
            break;
        default:
            assert(false);
    }
}

void EventManager::handleMediaPlayerEvent(const libvlc_event_t* event, void* data)
{
    IMediaPlayerEventCb* cb = reinterpret_cast<IMediaPlayerEventCb*>( data );
    switch ( event->type )
    {
        case libvlc_MediaPlayerMediaChanged:
            cb->mediaChanged( Media( event->u.media_player_media_changed.new_media, true ) );
            break;
        case libvlc_MediaPlayerNothingSpecial:
            cb->nothingSpecial();
            break;
        case libvlc_MediaPlayerOpening:
            cb->opening();
             break;
        case libvlc_MediaPlayerBuffering:
            cb->buffering( event->u.media_player_buffering.new_cache );
            break;
        case libvlc_MediaPlayerPlaying:
            cb->playing();
            break;
        case libvlc_MediaPlayerPaused:
            cb->paused();
            break;
        case libvlc_MediaPlayerStopped:
            cb->stopped();
            break;
        case libvlc_MediaPlayerForward:
            cb->forward();
            break;
        case libvlc_MediaPlayerBackward:
            cb->backward();
            break;
        case libvlc_MediaPlayerEndReached:
            cb->endReached();
            break;
        case libvlc_MediaPlayerEncounteredError:
            cb->encounteredError();
            break;
        case libvlc_MediaPlayerTimeChanged:
            cb->timeChanged( event->u.media_player_time_changed.new_time );
            break;
        case libvlc_MediaPlayerPositionChanged:
            cb->positionChanged( event->u.media_player_position_changed.new_position );
            break;
        case libvlc_MediaPlayerSeekableChanged:
            cb->seekableChanged( event->u.media_player_seekable_changed.new_seekable );
            break;
        case libvlc_MediaPlayerPausableChanged:
            cb->pausableChanged( event->u.media_player_seekable_changed.new_seekable );
            break;
        case libvlc_MediaPlayerTitleChanged:
            cb->titleChanged( event->u.media_player_title_changed.new_title );
            break;
        case libvlc_MediaPlayerSnapshotTaken:
            cb->snapshotTaken( event->u.media_player_snapshot_taken.psz_filename );
            break;
        case libvlc_MediaPlayerLengthChanged:
            cb->lengthChanged( event->u.media_player_length_changed.new_length );
            break;
        case libvlc_MediaPlayerVout:
            cb->vout( event->u.media_player_vout.new_count );
            break;
        case libvlc_MediaPlayerScrambledChanged:
            cb->scrambledChanged( event->u.media_player_scrambled_changed.new_scrambled );
            break;
        case libvlc_MediaPlayerESAdded:
            cb->eSAdded( event->u.media_player_es_changed.i_type, event->u.media_player_es_changed.i_id );
            break;
        case libvlc_MediaPlayerESDeleted:
            cb->eSDeleted( event->u.media_player_es_changed.i_type, event->u.media_player_es_changed.i_id );
            break;
        case libvlc_MediaPlayerESSelected:
            cb->eSSelected( event->u.media_player_es_changed.i_type, event->u.media_player_es_changed.i_id );
            break;
        default:
            assert(false);
    }
}

void EventManager::handleMediaListEvent(const libvlc_event_t* event, void* data)
{
    IMediaListEventCb* cb = reinterpret_cast<IMediaListEventCb*>( data );
    switch ( event->type )
    {
        case libvlc_MediaListItemAdded:
            cb->itemAdded( Media( event->u.media_list_item_added.item, true ), event->u.media_list_item_added.index );
            break;
        case libvlc_MediaListWillAddItem:
            cb->willAddItem( Media( event->u.media_list_will_add_item.item, true ), event->u.media_list_will_add_item.index );
            break;
        case libvlc_MediaListItemDeleted:
            cb->itemDeleted( Media( event->u.media_list_item_deleted.item, true ), event->u.media_list_item_deleted.index );
            break;
        case libvlc_MediaListWillDeleteItem:
            cb->willDeleteItem( Media( event->u.media_list_will_delete_item.item, true ), event->u.media_list_will_delete_item.index );
            break;
        default:
            assert(false);
    }
}

void EventManager::handleMediaListPlayerEvent(const libvlc_event_t* event, void* data)
{
    IMediaListPlayerEventCb* cb = reinterpret_cast<IMediaListPlayerEventCb*>( data );
    switch ( event->type )
    {
        case libvlc_MediaListPlayerPlayed:
            cb->played();
            break;
        case libvlc_MediaListPlayerNextItemSet:
            cb->nextItemSet( Media( event->u.media_list_player_next_item_set.item, true ) );
            break;
        case libvlc_MediaListPlayerStopped:
            cb->stopped();
            break;
        default:
            assert(false);
    }
}

void EventManager::handleMediaDiscovererEvent(const libvlc_event_t* event, void* data)
{
    IMediaDiscovererEventCb* cb = reinterpret_cast<IMediaDiscovererEventCb*>( data );
    switch ( event->type )
    {
        case libvlc_MediaDiscovererStarted:
            cb->started();
            break;
        case libvlc_MediaDiscovererEnded:
            cb->stopped();
            break;
        default:
            assert(false);
    }
}

void EventManager::handleVLMEvent(const libvlc_event_t* event, void* data)
{
    IVLMEventCb* cb = reinterpret_cast<IVLMEventCb*>( data );
    std::string mediaName = event->u.vlm_media_event.psz_media_name ? event->u.vlm_media_event.psz_media_name : "";
    std::string instance = event->u.vlm_media_event.psz_instance_name ? event->u.vlm_media_event.psz_instance_name : "";
    switch ( event->type )
    {
        case libvlc_VlmMediaAdded:
            cb->mediaAdded( mediaName );
            break;
        case libvlc_VlmMediaRemoved:
            cb->mediaRemoved( mediaName );
            break;
        case libvlc_VlmMediaChanged:
            cb->mediaChanged( mediaName );
            break;
        case libvlc_VlmMediaInstanceStarted:
            cb->mediaInstanceStarted( instance, mediaName );
            break;
        case libvlc_VlmMediaInstanceStopped:
            cb->mediaInstanceStopped( instance, mediaName );
            break;
        case libvlc_VlmMediaInstanceStatusInit:
            cb->mediaInstanceStatusInit( instance, mediaName );
            break;
        case libvlc_VlmMediaInstanceStatusOpening:
            cb->mediaInstanceStatusOpening( instance, mediaName );
            break;
        case libvlc_VlmMediaInstanceStatusPlaying:
            cb->mediaInstanceStatusPlaying( instance, mediaName );
            break;
        case libvlc_VlmMediaInstanceStatusPause:
            cb->mediaInstanceStatusPause( instance, mediaName );
            break;
        case libvlc_VlmMediaInstanceStatusEnd:
            cb->mediaInstanceStatusEnd( instance, mediaName );
            break;
        case libvlc_VlmMediaInstanceStatusError:
            cb->mediaInstanceStatusError( instance, mediaName );
            break;
        default:
            assert(false);
    }
}

EventManager::EventManager( InternalPtr obj)
    : Internal( obj )
{
}

}
