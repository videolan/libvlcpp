/*****************************************************************************
 * RendererDiscoverer.hpp: Renderer Discoverer API
 *****************************************************************************
 * Copyright © 2015-2018 libvlcpp authors & VideoLAN
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

#ifndef LIBVLC_CXX_RENDERERDISCOVERER_HPP
#define LIBVLC_CXX_RENDERERDISCOVERER_HPP

#include "common.hpp"

namespace VLC
{

class RendererDiscovererEventManager;

class RendererDiscoverer : public Internal<libvlc_renderer_discoverer_t>
{
public:
    class Item : public Internal<libvlc_renderer_item_t>
    {
    private:
        Item( InternalPtr item ) : Internal( item, libvlc_renderer_item_release )
        {
            libvlc_renderer_item_retain( *this );
        }

    public:
        std::string name() const
        {
            return libvlc_renderer_item_name( *this );
        }

        std::string type() const
        {
            return libvlc_renderer_item_type( *this );
        }

        std::string iconUri() const
        {
            auto uri = libvlc_renderer_item_icon_uri( *this );
            return uri != nullptr ? uri : std::string{};
        }

        bool canRenderVideo() const
        {
            return ( libvlc_renderer_item_flags( *this ) &
                     LIBVLC_RENDERER_CAN_VIDEO ) != 0;
        }

        bool canRenderAudio() const
        {
            return ( libvlc_renderer_item_flags( *this ) &
                     LIBVLC_RENDERER_CAN_AUDIO ) != 0;
        }

        template <size_t, typename ...>
        friend struct CallbackWrapper;
    };

    /**
     * Callback prototype that notify when the discoverer added an item
     *
     * \param item the new added item
     */
    using ExpectedItemAddedCb = void(const Item& item);

    /**
     * Callback prototype that notify when the discoverer removed an item
     *
     * \param item the removed item
     */
    using ExpectedItemRemovedCb = void(const Item& item);

    class Callbacks : protected CallbackOwner<2>
    {
    private:
        enum class CallbackIdx : unsigned int
        {
            ItemAdded,
            ItemRemoved,
        };

        libvlc_renderer_discoverer_cbs m_cbs;
        friend class RendererDiscoverer;

    public:
        Callbacks() = delete;

        /**
         * Constructor with the mandatory on_item_added callback.
         * 
         * \param itemAddedCb the callback to be called when an item is added to the discoverer
         * The callback must match the prototype defined by \ref ExpectedItemAddedCb.
         */
        template <typename ItemAddedCb>
        Callbacks( ItemAddedCb&& itemAddedCb )
        {
            static_assert( signature_match<ItemAddedCb, ExpectedItemAddedCb>::value,
                           "Mismatched on_item_added callback prototype" );
            m_cbs = {};
            m_cbs.version = 0;
            m_cbs.on_item_added = CallbackWrapper<(unsigned int)CallbackIdx::ItemAdded,
                                  decltype(libvlc_renderer_discoverer_cbs::on_item_added)>::
                                  wrap<Item>( *m_callbacks, std::forward<ItemAddedCb>( itemAddedCb ) );
        }

        /**
         * Sets the on_item_removed callback.
         *
         * \param cb the callback to be called when an item is removed from the discoverer.
         * The callback must match the prototype defined by \ref ExpectedItemRemovedCb.
         * \return reference to this Callbacks object for chaining
         */
        template <typename ItemRemovedCb>
        Callbacks& onItemRemoved( ItemRemovedCb&& itemRemovedCb )
        {
            static_assert( signature_match<ItemRemovedCb, ExpectedItemRemovedCb>::value,
                           "Mismatched on_item_removed callback prototype" );
            m_cbs.on_item_removed = CallbackWrapper<(unsigned int)CallbackIdx::ItemRemoved,
                                    decltype(libvlc_renderer_discoverer_cbs::on_item_removed)>::
                                    wrap<Item>( *m_callbacks, std::forward<ItemRemovedCb>( itemRemovedCb ) );
            return *this;
        }
    };

    /**
     * Create a renderer discoverer object by name and callback to listen to events.
     *
     * \param instance libvlc instance
     * \param name service name
     * \param cbs pre-built \ref Callbacks object
     *
     * \warning The application must ensure that the Callbacks object supplied
     * remains valid and unmodified until the renderer discoverer is destroyed.
     */
    RendererDiscoverer( const Instance& instance, const std::string& name, const Callbacks& cbs )
    {
        auto ptr = libvlc_renderer_discoverer_new( getInternalPtr<libvlc_instance_t>( instance ),
                                                   name.c_str(), &cbs.m_cbs, cbs.m_callbacks.get() );
        if ( ptr == nullptr )
            throw std::runtime_error( "Failed to create renderer discoverer" );
        m_obj.reset( ptr, libvlc_renderer_discoverer_destroy );
    }

    bool start()
    {
        return libvlc_renderer_discoverer_start( *this ) == 0;
    }

    void stop()
    {
        libvlc_renderer_discoverer_stop( *this );
    }

    RendererDiscovererEventManager& eventManager()
    {
        if ( m_eventManager == nullptr )
        {
            libvlc_event_manager_t* obj = libvlc_renderer_discoverer_event_manager( *this );
            m_eventManager = std::make_shared<RendererDiscovererEventManager>( obj, *this );
        }
        return *m_eventManager;
    }

private:
    std::shared_ptr<RendererDiscovererEventManager> m_eventManager;
};

}

#endif // LIBVLC_CXX_RENDERERDISCOVERER_HPP
