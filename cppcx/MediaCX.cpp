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

#include "structuresCX.hpp"
#include "InstanceCX.hpp"
#include "EventManagerCX.hpp"
#include "MediaCX.hpp"
#include "MediaListCX.hpp"
#include <collection.h>

namespace libVLCX
{
    Media::Media(Instance^ instance, Platform::String^ location, FromType from_type)
        : m_media(VLC::Media( instance->m_instance, VLCString(location), (VLC::Media::FromType)from_type ))
    {
    }

    void Media::addOption(Platform::String^ options)
    {
        m_media.addOption(VLCString(options));
    }

    void Media::addOptionFlag(Platform::String^ options, unsigned i_flags)
    {
        m_media.addOptionFlag(VLCString(options), i_flags);
    }

    Platform::String^ Media::mrl()
    {
        return ToPlatformString(m_media.mrl());
    }

    Media^ Media::duplicate()
    {
        return ref new Media(m_media.duplicate());
    }

    Platform::String^ Media::meta(MediaMeta e_meta)
    {
        return ToPlatformString(m_media.meta((libvlc_meta_t)e_meta));
    }

    void Media::setMeta(MediaMeta e_meta, Platform::String^ psz_value)
    {
        m_media.setMeta((libvlc_meta_t)e_meta, VLCString(psz_value));
    }

    int Media::saveMeta()
    {
        return m_media.saveMeta();
    }

    MediaState Media::state()
    {
        return (MediaState)m_media.state();
    }

    //bool Media::stats(libvlc_media_stats_t * p_stats);

    MediaEventManager^ Media::eventManager()
    {
        if (m_eventManager == nullptr)
            m_eventManager = ref new MediaEventManager(m_media.eventManager());
        return m_eventManager;
    }

    libvlc_time_t Media::duration()
    {
        return m_media.duration();
    }

    void Media::parseWithOptions(ParseFlags flags, int timeoutMs)
    {
        m_media.parseWithOptions(static_cast<VLC::Media::ParseFlags>( flags ), timeoutMs);
    }

    IAsyncOperation<ParsedStatus>^ Media::parseWithOptionsAsync(ParseFlags flags, int timeoutMs)
    {
        return concurrency::create_async([=](concurrency::cancellation_token ct) {
            auto res = std::make_shared<Concurrency::task_completion_event<ParsedStatus>>();
            m_media.eventManager().onParsedChanged([res](VLC::Media::ParsedStatus status) {
                res->set(static_cast<ParsedStatus>(status));
            });
            if (m_media.parseWithOptions(static_cast<VLC::Media::ParseFlags>(flags), timeoutMs) == false)
                res->set(ParsedStatus::Failed);
            return concurrency::create_task(*res, ct);
        });
    }

    ParsedStatus Media::parsedStatus()
    {
        return (ParsedStatus)m_media.parsedStatus();
    }

    Windows::Foundation::Collections::IVector<MediaTrack^>^ Media::tracks()
    {
        return MarshallVector<MediaTrack, VLC::MediaTrack>(m_media.tracks());
    }

    MediaList^ Media::subItems()
    {
        return ref new MediaList(*m_media.subitems());
    }

    MediaType Media::type()
    {
        return static_cast<MediaType>(m_media.type());
    }

    Media::Media(const VLC::Media& media)
        : m_media(media)
    {
    }

    Media::Media(VLC::Media&& media)
        : m_media( std::move( media ) )
    {
    }

    bool Media::addSlave(SlaveType type, unsigned priority, Platform::String^ uri)
    {
        return m_media.addSlave((VLC::MediaSlave::Type)type, priority, FromPlatformString(uri));
    }

    void Media::slavesClear()
    {
        m_media.slavesClear();
    }

    Windows::Foundation::Collections::IVector<MediaSlave^>^ Media::slaves()
    {
        return MarshallVector<MediaSlave, VLC::MediaSlave>(m_media.slaves());
    }

} // namespace VLC


