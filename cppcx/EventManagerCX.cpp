/*****************************************************************************
* EventManagerCX.cpp: EventManager API
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
#include <vlcpp/vlc.hpp>

namespace libVLCX
{
    MediaPlayerEventManager::MediaPlayerEventManager(VLC::MediaPlayerEventManager& em)
        : m_em(em)
    {
    }


    MediaEventManager::MediaEventManager(VLC::MediaEventManager& em)
        : m_em(em)
    {
    }

    MediaListEventManager::MediaListEventManager(VLC::MediaListEventManager& em)
        : m_em(em)
    {
    }

    void EventRemover::removeToken(std::vector<VLC::EventManager::RegisteredEvent>& events, Windows::Foundation::EventRegistrationToken token)
    {
        auto h = (VLC::EventManager::RegisteredEvent)token.Value;
        auto it = std::find(begin(events), end(events), h);
        assert(it != end(events));
        (*it)->unregister();
        events.erase(it);
    }
}
