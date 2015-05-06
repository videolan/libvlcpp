/*****************************************************************************
 * Instance.hpp: Instance API
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

#ifndef LIBVLC_CXX_INSTANCE_H
#define LIBVLC_CXX_INSTANCE_H

#include "common.hpp"
#include "Internal.hpp"
#include "structures.hpp"

#include <string>
#include <vector>

namespace VLC
{

class Instance : public Internal<libvlc_instance_t>, private CallbackOwner<2>
{
private:
    enum class CallbackIdx : unsigned int
    {
        Exit,
        Log
    };
public:
    /**
     * Create and initialize a libvlc instance. This functions accept a list
     * of "command line" arguments similar to the main(). These arguments
     * affect the LibVLC instance default configuration.
     *
     * \version Arguments are meant to be passed from the command line to
     * LibVLC, just like VLC media player does. The list of valid arguments
     * depends on the LibVLC version, the operating system and platform, and
     * set of available LibVLC plugins. Invalid or unsupported arguments will
     * cause the function to fail (i.e. return NULL). Also, some arguments
     * may alter the behaviour or otherwise interfere with other LibVLC
     * functions.
     *
     * \warning There is absolutely no warranty or promise of forward,
     * backward and cross-platform compatibility with regards to
     * Instance::Instance() arguments. We recommend that you do not use them,
     * other than when debugging.
     *
     * \param argc  the number of arguments (should be 0)
     *
     * \param argv  list of arguments (should be NULL)
     */
    Instance(int argc, const char *const * argv)
        : Internal{ libvlc_new( argc, argv ), libvlc_release }
    {
    }

    /**
     * Create an empty VLC instance.
     *
     * Calling any method on such an instance is undefined.
    */
    Instance() = default;

    /**
     * Check if 2 Instance objects contain the same libvlc_instance_t.
     * \param another another Instance
     * \return true if they contain the same libvlc_instance_t
     */
    bool operator==(const Instance& another) const
    {
        return m_obj == another.m_obj;
    }


    /**
     * Try to start a user interface for the libvlc instance.
     *
     * \param name  interface name, or empty string for default
     */
    bool addIntf(const std::string& name)
    {
        return libvlc_add_intf( *this, name.length() > 0 ? name.c_str() : nullptr ) == 0;
    }

    /**
     * Registers a callback for the LibVLC exit event. This is mostly useful
     * if the VLC playlist and/or at least one interface are started with
     * libvlc_playlist_play() or Instance::addIntf() respectively. Typically,
     * this function will wake up your application main loop (from another
     * thread).
     *
     * \note This function should be called before the playlist or interface
     * are started. Otherwise, there is a small race condition: the exit
     * event could be raised before the handler is registered.
     *
     * \param cb  callback to invoke when LibVLC wants to exit, or nullptr to
     * disable the exit handler (as by default). It is expected to be a
     * std::function<void()>, or an equivalent Callable type
     */
    template <typename ExitCb>
    void setExitHandler(ExitCb&& exitCb)
    {
        static_assert(signature_match_or_nullptr<ExitCb, void()>::value, "Mismatched exit callback" );
        libvlc_set_exit_handler( *this,
            CallbackWrapper<(unsigned int)CallbackIdx::Exit, void(*)(void*)>::wrap( this, std::forward<ExitCb>( exitCb ) ),
            static_cast<CallbackOwner<2>*>( this ) );
    }

    /**
     * Sets the application name. LibVLC passes this as the user agent string
     * when a protocol requires it.
     *
     * \param name  human-readable application name, e.g. "FooBar player
     * 1.2.3"
     *
     * \param http  HTTP User Agent, e.g. "FooBar/1.2.3 Python/2.6.0"
     *
     * \version LibVLC 1.1.1 or later
     */
    void setUserAgent(const std::string& name, const std::string& http)
    {
        libvlc_set_user_agent( *this, name.c_str(), http.c_str() );
    }

    /**
     * Sets some meta-information about the application. See also
     * Instance::setUserAgent() .
     *
     * \param id  Java-style application identifier, e.g. "com.acme.foobar"
     *
     * \param version  application version numbers, e.g. "1.2.3"
     *
     * \param icon  application icon name, e.g. "foobar"
     *
     * \version LibVLC 2.1.0 or later.
     */
    void setAppId(const std::string& id, const std::string& version, const std::string& icon)
    {
        libvlc_set_app_id( *this, id.c_str(), version.c_str(), icon.c_str() );
    }

    /**
     * Unsets the logging callback for a LibVLC instance. This is rarely
     * needed: the callback is implicitly unset when the instance is
     * destroyed. This function will wait for any pending callbacks
     * invocation to complete (causing a deadlock if called from within the
     * callback).
     *
     * \version LibVLC 2.1.0 or later
     */
    void logUnset()
    {
        libvlc_log_unset( *this );
    }

    /**
     * Sets the logging callback for a LibVLC instance. This function is
     * thread-safe: it will wait for any pending callbacks invocation to
     * complete.
     *
     * \note Some log messages (especially debug) are emitted by LibVLC while
     * is being initialized. These messages cannot be captured with this
     * interface.
     *
     * \param logCb A std::function<void(int, const libvlc_log_t*, std::string)>
     *              or an equivalent Callable type instance.
     *
     * \warning A deadlock may occur if this function is called from the
     * callback.
     *
     * \version LibVLC 2.1.0 or later
     */
    template <typename LogCb>
    void logSet(LogCb&& logCb)
    {
        static_assert(signature_match<LogCb, void(int, const libvlc_log_t*, std::string)>::value,
                      "Mismatched log callback" );
        auto wrapper = [logCb](int level, const libvlc_log_t* ctx, const char* format, va_list va) {
            VaCopy vaCopy( va );
            int len = vsnprintf( nullptr, 0, format, vaCopy.va );
            if ( len > 0 )
            {
                std::unique_ptr<char[]> message{ new char[len] };
                if ( vsnprintf( message.get(), len, format, va ) != -1 )
                    logCb( level, ctx, std::string{ message.get() } );
            }
        };
        libvlc_log_set( *this, CallbackWrapper<(unsigned int)CallbackIdx::Log, libvlc_log_cb>::wrap( this, std::move( wrapper ) ),
                        static_cast<CallbackOwner<2>*>( this ) );
    }

    /**
     * Sets up logging to a file.
     *
     * \param stream  FILE pointer opened for writing (the FILE pointer must
     * remain valid until Instance::logUnset() )
     *
     * \version LibVLC 2.1.0 or later
     */
    void logSetFile(FILE * stream)
    {
        libvlc_log_set_file( *this, stream );
    }

    /**
     * Returns a list of audio filters that are available.
     *
     * \see ModuleDescription
     */
    std::vector<ModuleDescription> audioFilterList()
    {
        std::unique_ptr<libvlc_module_description_t, decltype(&libvlc_module_description_list_release)>
                ptr( libvlc_audio_filter_list_get(*this), libvlc_module_description_list_release );
        if ( ptr == nullptr )
            return {};
        libvlc_module_description_t* p = ptr.get();
        std::vector<ModuleDescription> res;
        while ( p != NULL )
        {
            res.emplace_back( p );
            p = p->p_next;
        }
        return res;
    }


    /**
     * Returns a list of video filters that are available.
     *
     * \see ModuleDescription
     */
    std::vector<ModuleDescription> videoFilterList()
    {
        std::unique_ptr<libvlc_module_description_t, decltype(&libvlc_module_description_list_release)>
                ptr( libvlc_video_filter_list_get(*this), &libvlc_module_description_list_release );
        if ( ptr == nullptr )
            return {};
        libvlc_module_description_t* p = ptr.get();
        std::vector<ModuleDescription> res;
        while ( p != NULL )
        {
            res.emplace_back( p );
            p = p->p_next;
        }
        return res;
    }

    /**
     * Gets the list of available audio output modules.
     *
     * \see AudioOutputDescription
     */
    std::vector<AudioOutputDescription> audioOutputList()
    {
        std::unique_ptr<libvlc_audio_output_t, decltype(&libvlc_audio_output_list_release)>
                result( libvlc_audio_output_list_get(*this), libvlc_audio_output_list_release );
        if ( result == nullptr )
            return {};
        std::vector<AudioOutputDescription> res;

        libvlc_audio_output_t* p = result.get();
        while ( p != NULL )
        {
            res.emplace_back( p );
            p = p->p_next;
        }
        return res;
    }

    /**
     * Gets a list of audio output devices for a given audio output module,
     *
     * \see Audio::outputDeviceSet() .
     *
     * \note Not all audio outputs support this. In particular, an empty
     * (NULL) list of devices does imply that the specified audio output does
     * not work.
     *
     * \note The list might not be exhaustive.
     *
     * \warning Some audio output devices in the list might not actually work
     * in some circumstances. By default, it is recommended to not specify
     * any explicit audio device.
     *
     * \param psz_aout  audio output name (as returned by
     * Instance::audioOutputList() )
     *
     * \return A vector containing all audio output devices for this module
     *
     * \version LibVLC 2.1.0 or later.
     */
    std::vector<AudioOutputDeviceDescription> audioOutputDeviceList(const std::string& aout)
    {
        std::unique_ptr<libvlc_audio_output_device_t, decltype(&libvlc_audio_output_device_list_release)>
                devices(  libvlc_audio_output_device_list_get( *this, aout.c_str() ), libvlc_audio_output_device_list_release );
        if ( devices == nullptr )
            return {};
        std::vector<AudioOutputDeviceDescription> res;

        for ( auto p = devices.get(); p != nullptr; p = p->p_next )
            res.emplace_back( p );
        return res;
    }

};

} // namespace VLC

#endif

