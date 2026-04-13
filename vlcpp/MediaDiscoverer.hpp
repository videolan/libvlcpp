/*****************************************************************************
 * MediaDiscoverer.hpp: MediaDiscoverer API
 *****************************************************************************
 * Copyright © 2015 libvlcpp authors & VideoLAN
 *
 * Authors: Alexey Sokolov <alexey+vlc@asokolov.org>
 *          Hugo Beauzée-Luyssen <hugo@beauzee.fr>
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

#ifndef LIBVLC_CXX_MEDIADISCOVERER_H
#define LIBVLC_CXX_MEDIADISCOVERER_H

#include "common.hpp"

#include <string>

namespace VLC
{

class Instance;

class MediaDiscoverer : public Internal<libvlc_media_discoverer_t>
{
public:
    enum class Category
    {
        Devices = libvlc_media_discoverer_devices,
        Lan = libvlc_media_discoverer_lan,
        Podcasts = libvlc_media_discoverer_podcasts,
        Localdirs = libvlc_media_discoverer_localdirs,
    };

    class Description
    {
    public:
        explicit Description( const std::string& name, const std::string& longName, libvlc_media_discoverer_category_t cat )
            : m_name( name )
            , m_longName( longName )
            , m_category( static_cast<Category>( cat ) )
        {
        }

        const std::string& name() const
        {
            return m_name;
        }

        const std::string& longName() const
        {
            return m_longName;
        }

        Category category() const
        {
            return m_category;
        }

    private:
        std::string m_name;
        std::string m_longName;
        Category m_category;
    };

    /**
     * Callback prototype that notify when the discoverer added a media
     *
     * \param parent parent of the new added media or nullptr if there is no
     * parents (more likely)
     * \param media the new added media
     */
    using ExpectedMediaAddedCb = void(MediaPtr parent, MediaPtr media);

    /**
     * Callback prototype that notify when the discoverer removed a media
     *
     * \param media the removed media
     */
    using ExpectedMediaRemovedCb = void(MediaPtr media);

    class Callbacks : protected CallbackOwner<2>
    {
    private:
        enum class CallbackIdx : unsigned int
        {
            MediaAdded,
            MediaRemoved,
        };

        friend class MediaDiscoverer;
        libvlc_media_discoverer_cbs m_cbs;

    public:

        /**
         * Default constructor to initialize all callbacks to nullptr.
         */
        Callbacks()
        {
            m_cbs = {};
            m_cbs.version = 0;
        }

        /**
         * Sets the on media added callback.
         * 
         * \param cb callback to notify when a media is added to the discoverer
         * The callback must match the \ref ExpectedMediaAddedCb prototype
         * \return reference to this Callbacks object for chaining
         */
        template <typename MediaAddedCb>
        Callbacks& onMediaAdded( MediaAddedCb&& cb )
        {
            static_assert( signature_match<MediaAddedCb, ExpectedMediaAddedCb>::value,
                           "Mismatched onMediaAdded callback prototype" );
            m_cbs.on_media_added = CallbackWrapper<(unsigned int)CallbackIdx::MediaAdded,
                                   decltype(libvlc_media_discoverer_cbs::on_media_added)>::
                                   wrap<MediaPtr, MediaPtr>( *m_callbacks, std::forward<MediaAddedCb>( cb ) );
            return *this;
        }

        /**
         * Sets the on media removed callback.
         * 
         * \param cb callback to notify when a media is removed from the discoverer
         * The callback must match the \ref ExpectedMediaRemovedCb prototype
         * \return reference to this Callbacks object for chaining
         */
        template <typename MediaRemovedCb>
        Callbacks& onMediaRemoved( MediaRemovedCb&& cb )
        {
            static_assert( signature_match<MediaRemovedCb, ExpectedMediaRemovedCb>::value,
                           "Mismatched onMediaRemoved callback prototype" );
            m_cbs.on_media_removed = CallbackWrapper<(unsigned int)CallbackIdx::MediaRemoved,
                                     decltype(libvlc_media_discoverer_cbs::on_media_removed)>::
                                     wrap<MediaPtr>( *m_callbacks, std::forward<MediaRemovedCb>( cb ) );
            return *this;
        }
    };

    /**
     * Discover media service by name.
     *
     * \param p_inst  libvlc instance
     *
     * \param psz_name  service name
     */
    MediaDiscoverer(const Instance& inst, const std::string& name)
        : Internal{ libvlc_media_discoverer_new(getInternalPtr<libvlc_instance_t>( inst ), name.c_str(),
                                                nullptr, nullptr), libvlc_media_discoverer_destroy }
    {
    }

    /**
     * Discover media service by name and callback to listen to events.
     *
     * \param p_inst libvlc instance
     * \param psz_name service name
     * \param cbs pre-built \ref Callbacks object
     *
     * \warning The application must ensure that the Callbacks object supplied
     * remains valid and unmodified until the media discoverer is destroyed.
     */
    MediaDiscoverer( const Instance& inst, const std::string& name, const Callbacks& cbs )
    {
        auto ptr = libvlc_media_discoverer_new( getInternalPtr<libvlc_instance_t>( inst ),
                                                name.c_str(), &cbs.m_cbs, cbs.m_callbacks.get() );
        if ( ptr == nullptr )
            throw std::runtime_error( "Failed to create media discoverer" );
        m_obj.reset( ptr, libvlc_media_discoverer_destroy );
    }

    /**
     * Create an empty Media Discoverer instance.
     *
     * Calling any method on such an instance is undefined.
    */
    MediaDiscoverer() = default;

    /**
     * Start media discovery.
     *
     * To stop it, call MediaDiscover::stop() or
     * destroy the object directly.
     *
     * \see MediaDiscover::stop
     *
     * \return false in case of error, true otherwise
     */
    bool start()
    {
        return libvlc_media_discoverer_start( *this ) == 0;
    }

    /**
     * Stop media discovery.
     *
     * \see MediaDiscoverer::start()
     */
    void stop()
    {
        libvlc_media_discoverer_stop( *this );
    }

    /**
     * Query if media service discover object is running.
     *
     * \return true if running, false if not
     */
    bool isRunning()
    {
        return libvlc_media_discoverer_is_running(*this) != 0;
    }
};

} // namespace VLC

#endif
