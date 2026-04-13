/*****************************************************************************
 * Parser.hpp: Parser API
 *****************************************************************************
 * Copyright © 2026 libvlcpp authors & VideoLAN
 *
 * Author: Ayush Dey <deyayush6@gmail.com>
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

#ifndef LIBVLC_CXX_PARSER_HPP
#define LIBVLC_CXX_PARSER_HPP

#include "common.hpp"

namespace VLC
{
class Parser : public Internal<libvlc_parser_t>
{
public:
    /**
     * Terminal outcome of a finished parse request, reported by the
     * onParsed callback.
     */
    enum class Status
    {
        Failed = libvlc_parser_status_failed,
        Timeout = libvlc_parser_status_timeout,
        Cancelled = libvlc_parser_status_cancelled,
        Done = libvlc_parser_status_done,
    };

    class Task : public Internal<libvlc_parser_task>
    {
    private:
        explicit Task( libvlc_parser_task *task ) : Internal( task, libvlc_parser_task_release )
        {
        }

    public:
        /**
         * Fetch the media associated with the task handle.
         *
         * \return the media associated with the task
         */
        MediaPtr getMedia() const
        {
            auto media = libvlc_parser_task_get_media( *this );
            return std::make_shared<Media>( media, true );
        }

        template <size_t, typename ...>
        friend struct CallbackWrapper;
    };

    class TaskIdentifier
    {
    private:
        libvlc_parser_task *m_task;

        explicit TaskIdentifier( libvlc_parser_task *task )
            : m_task( task )
        {
        }

    public:
        friend bool operator==( const TaskIdentifier& id, const Task& task )
        {
            return id.m_task == task.get();
        }

        friend bool operator==( const Task& task, const TaskIdentifier& id )
        {
            return id == task;
        }

        bool operator==(const TaskIdentifier& another) const
        {
            return m_task == another.m_task;
        }

        /**
         * Fetch the media associated with the task handle.
         *
         * \return the media associated with the task
         */
        MediaPtr getMedia() const
        {
            auto media = libvlc_parser_task_get_media( m_task );
            return std::make_shared<Media>( media, true );
        }

        friend class Parser;
        template <size_t, typename ...>
        friend struct CallbackWrapper;
    };

    /**
     * Callback prototype that notifies when a parser request finishes
     *
     * \param task opaque handle of the task that finished, can be compared to the TaskIdentifier
     * returned by Parser::queue() using operator==, to identify which task finished
     * \param status terminal parse outcome, \ref Parser::Status
     */
    using ExpectedOnParsedCb = void(Parser::Task&& task, Parser::Status status);

    /**
     * Callback prototype that notify when the parser add new attachments to
     * the media.
     *
     * Called before onParsed, if there are valid attachments.
     *
     * \param task opaque handle of the task, can be compared to the TaskIdentifier
     * returned by Parser::queue() using operator==, to identify which task added attachments
     * \param list list of pictures, the list is only valid from this
     * callback, each pictures can be held separately with list.at(index) method
     */
    using ExpectedOnAttachmentsAddedCb = void(Parser::TaskIdentifier task, const Picture::List& list);

    enum class ParseFlags
    {
        /**
         * Parse media
         */
        Parse = libvlc_media_parse,
        /**
         * Fetch meta and cover art using local resources
         */
        FetchLocal = libvlc_media_fetch_local,
        /**
         * Fetch meta and cover art using network resources
         */
        FetchNetwork = libvlc_media_fetch_network,
        /**
         * Interact with the user (via libvlc_dialog_cbs) when preparsing this item
         * (and not its sub items). Set this flag in order to receive a callback
         * when the input is asking for credentials.
         */
        Interact = libvlc_media_do_interact,
    };

    class Config
    {
    public:
        /**
         * Default constructor initializes the parserconfig with default values (0 or equivalent).
         */
        Config()
        {
            m_cfg = {};
            m_cfg.version = 0;
        }

        /**
         * Set the timeout of the parser in milliseconds. 0 means no limits.
         * 
         * \param timeout the timeout to set for the parser
         * \return reference to this Config object for chaining
         */
        Config& setTimeout( std::chrono::milliseconds timeout )
        {
            m_cfg.timeout = timeout.count();
            return *this;
        }

        /**
         * Set the maximum number of parser threads, 0 for default(1 thread).
         * 
         * \param maxThreads the maximum number of threads used by the parser
         * \return reference to this Config object for chaining
         */
        Config& setMaxParserThreads( uint32_t maxThreads )
        {
            m_cfg.max_parser_threads = maxThreads;
            return *this;
        }

    private:
        friend class Parser;
        libvlc_parser_cfg m_cfg;
    };

    class Request
    {
    public:
        Request() = delete;

        /**
         * Constructor to initialize the parser request with default values (0 or equivalent)
         * and the provided media.
         *
         * \param media the media to parse
         *
         * \warning The media object must remain valid until the parser request is queued.
         */
        Request( Media& media )
        {
            m_req = {};
            m_req.version = 0;
            m_req.media = getInternalPtr<libvlc_media_t>( media );
        }

        /**
         * Set the parse flags for the parser request.
         *
         * \param flags the parse flags to set using \ref ParseFlags
         * \return reference to this Request object for chaining
         */
        Request& setParseFlags( Parser::ParseFlags flags )
        {
            m_req.parse_flags = static_cast<libvlc_media_parse_flag_t>( flags );
            return *this;
        }

    private:
        friend class Parser;
        libvlc_parser_request_t m_req;
    };

    class Callbacks : protected CallbackOwner<2>
    {
    private:
        enum class CallbackIdx : unsigned int
        {
            OnParsed,
            OnAttachmentsAdded,
        };

        friend class Parser;
        libvlc_parser_cbs m_cbs;

    public:
        Callbacks() = delete;

        /**
         * Constructor with the mandatory on_parsed callback.
         * 
         * \param onParsedCb the callback to be called when a parser request finishes.
         * The callback must match the prototype defined by \ref ExpectedOnParsedCb.
         */
        template <typename OnParsedCb>
        Callbacks( OnParsedCb&& onParsedCb )
        {
            static_assert( signature_match<OnParsedCb, ExpectedOnParsedCb>::value,
                           "Mismatched on_parsed callback prototype" );
            m_cbs = {};
            m_cbs.version = 0;
            m_cbs.on_parsed = CallbackWrapper<(unsigned int)CallbackIdx::OnParsed,
                              decltype(libvlc_parser_cbs::on_parsed)>::wrap<Parser::Task, Parser::Status>(
                              *m_callbacks, std::forward<OnParsedCb>( onParsedCb ) );
        }

        /**
         * Sets the on_attachments_added callback.
         *
         * \param cb the callback to be called when new attachments are added to the media during parsing.
         * The callback must match the prototype defined by \ref ExpectedOnAttachmentsAddedCb.
         * \return reference to this Callbacks object for chaining
         */
        template <typename OnAttachmentsAddedCb>
        Callbacks& onAttachmentsAdded( OnAttachmentsAddedCb&& cb )
        {
            static_assert( signature_match<OnAttachmentsAddedCb, ExpectedOnAttachmentsAddedCb>::value,
                           "Mismatched on_attachments_added callback prototype" );
            m_cbs.on_attachments_added = CallbackWrapper<(unsigned int)CallbackIdx::OnAttachmentsAdded,
                                         decltype(libvlc_parser_cbs::on_attachments_added)>::wrap<
                                         Parser::TaskIdentifier, Picture::List>(
                                         *m_callbacks, std::forward<OnAttachmentsAddedCb>( cb ) );
            return *this;
        }
    };

    /**
     * Create a new Parser instance with the provided configuration.
     *
     * \param instance the VLC instance
     * \param config the configuration for the parser, if not provided, default values will be used
     */
    Parser( const Instance& instance, const Config& config = Config() )
    {
        auto ptr = libvlc_parser_new( getInternalPtr<libvlc_instance_t>( instance ),
                                      &config.m_cfg );
        if ( ptr == nullptr )
            throw std::runtime_error( "Failed to create parser" );
        m_obj.reset( ptr, libvlc_parser_destroy );
    }

    /**
     * Queue a parsing request.
     *
     * \param request the parsing request
     * \param cbs pre-built \ref Parser::Callbacks object
     * \return the queued task \ref TaskIdentifier that can be used to identify the
     * task in callbacks and to cancel the task if needed
     *
     * \warning The application must ensure that the Callbacks object supplied
     * remains valid and unmodified until the parser request terminates and the onParsedCb callback is called
     * on that Task (the returned TaskIdentifier can be used to identify the Task in the onParsedCb
     * using ==).
     *
     * \warning The returned TaskIdentifier is only valid till the parser request finishes
     * and the onParsedCb callback is called, after that it should not be used anymore
     * as the underlying task object is released by libVLC and the TaskIdentifier will hold a
     * dangling pointer.
     */
    TaskIdentifier queue( const Request& request, const Callbacks& cbs )
    {
        auto task = libvlc_parser_queue( *this, &request.m_req, &cbs.m_cbs, cbs.m_callbacks.get() );
        if ( task == nullptr )
            throw std::runtime_error( "Failed to queue parser task" );
        return TaskIdentifier( task );
    }

    /**
     * Cancel a parser request.
     *
     * \param task the task to cancel \ref TaskIdentifier
     * \return the number of cancelled tasks
     *
     * \warning The TaskIdentifier supplied must be valid and not expired,
     * otherwise the behavior is undefined.
     */
    size_t cancelRequest( TaskIdentifier& task )
    {
        return libvlc_parser_cancel_request( *this, task.m_task );
    }

    /**
     * Cancel all parser requests.
     *
     * \return the number of cancelled tasks
     */
    size_t cancelAll()
    {
        return libvlc_parser_cancel_request( *this, nullptr );
    }
};

inline Parser::ParseFlags operator|(Parser::ParseFlags l, Parser::ParseFlags r)
{
    using T = typename std::underlying_type<Parser::ParseFlags>::type;
    return static_cast<Parser::ParseFlags>( static_cast<T>( l ) | static_cast<T>( r ) );
}

} // namespace VLC

#endif // LIBVLC_CXX_PARSER_HPP
