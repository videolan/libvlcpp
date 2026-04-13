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

    /**
     * Callback prototype that notify when a thumbnailer request finishes
     *
     * \param task opaque handle of the task that finished, can be compared to the TaskIdentifier
     * returned by Parser::queueThumbnailing() using operator==, to identify which task finished
     * \param picture generated thumbnail, the thumbnail is only valid for the duration
     * of the callback, but can be safely copied if needed. It is an empty Picture object in case
     * of an error, timeout or request was cancelled. User should check if the Picture is valid by
     * calling picture.isValid()
     */
    using ExpectedOnThumbnailerEndedCb = void(Parser::Task&& task, const Picture& picture);

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

    enum class ThumbnailSeekSpeed
    {
        /**
         * Precise, but potentially slow
         */
        Precise = libvlc_media_thumbnail_seek_precise,
        /**
         * Fast, but potentially imprecise
         */
        Fast = libvlc_media_thumbnail_seek_fast,
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

        /**
         * Set the maximum number of thumbnailer threads, 0 for default(1 thread).
         * 
         * \param maxThreads the maximum number of threads
         * \return reference to this Config object for chaining
         */
        Config& setMaxThumbnailerThreads( uint32_t maxThreads )
        {
            m_cfg.max_thumbnailer_threads = maxThreads;
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

    class ThumbnailerRequest
    {
    public:
        ThumbnailerRequest() = delete;

        /**
         * Constructor to initialize the thumbnailer request with default values (0 or equivalent)
         * and the provided media.
         *
         * \param media the media for which to generate thumbnails
         *
         * \warning The media object must remain valid until the thumnailer request is queued.
         */
        ThumbnailerRequest( Media& media )
        {
            m_req = {};
            m_req.version = 0;
            m_req.media = getInternalPtr<libvlc_media_t>( media );
        }

        /**
         * Set the size of the thumbnail to generate.
         *
         * The resulting thumbnail size depends on the dimensions provided:
         * - If both \p width and \p height are non-zero, then if \p crop is false,
         *   the image is stretched to match the requested aspect ratio, or
         *   cropped instead if \p crop is true.
         * - If only one of \p width or \p height is non-zero (the other left
         *   to 0), the missing dimension is derived from the media aspect
         *   ratio. \p crop is ignored in this case.
         * - If both are 0 (the default), the thumbnail keeps the media size.
         *   \p crop is ignored in this case.
         *
         * \param width the width of the thumbnail, or 0 to derive it
         * \param height the height of the thumbnail, or 0 to derive it
         * \param crop true to crop the thumbnail to the requested aspect ratio,
         * false to stretch it. Only meaningful when both \p width and \p height
         * are non-zero; ignored otherwise. (false by default)
         * \return reference to this ThumbnailerRequest object for chaining
         */
        ThumbnailerRequest& setSize( unsigned int width, unsigned int height, bool crop = false )
        {
            m_req.width = width;
            m_req.height = height;
            m_req.crop = crop;
            return *this;
        }

        /**
         * Set the image format of the generated thumbnail.
         *
         * \param type the image format of the thumbnail \ref Picture::Type
         * \return reference to this ThumbnailerRequest object for chaining
         */
        ThumbnailerRequest& setPictureType( Picture::Type type )
        {
            m_req.type = static_cast<libvlc_picture_type_t>( type );
            return *this;
        }

        /**
         * Seek to a given time before generating the thumbnail.
         *
         * \param time the seek time (in milliseconds)
         * \param speed the seek speed \ref ThumbnailSeekSpeed (Precise by default)
         * \return reference to this ThumbnailerRequest object for chaining
         */
        ThumbnailerRequest& setSeekTime( std::chrono::milliseconds time,
                                         ThumbnailSeekSpeed speed = ThumbnailSeekSpeed::Precise )
        {
            m_req.seek.type = libvlc_thumbnailer_seek_time;
            m_req.seek.value.time = time.count();
            m_req.seek.speed = static_cast<libvlc_thumbnailer_seek_speed_t>( speed );
            return *this;
        }

        /**
         * Seek to a given position before generating the thumbnail.
         *
         * \param pos the seek position, in the [0., 1.] range
         * \param speed the seek speed \ref ThumbnailSeekSpeed (Precise by default)
         * \return reference to this ThumbnailerRequest object for chaining
         */
        ThumbnailerRequest& setSeekPosition( double pos,
                                             ThumbnailSeekSpeed speed = ThumbnailSeekSpeed::Precise )
        {
            m_req.seek.type = libvlc_thumbnailer_seek_pos;
            m_req.seek.value.pos = pos;
            m_req.seek.speed = static_cast<libvlc_thumbnailer_seek_speed_t>( speed );
            return *this;
        }

        /**
         * Set whether to enable hardware decoding for thumbnail generation.
         *
         * \param hwDec true to enable hardware decoding, false otherwise
         * \return reference to this ThumbnailerRequest object for chaining
         */
        ThumbnailerRequest& setHwDec( bool hwDec )
        {
            m_req.hw_dec = hwDec;
            return *this;
        }

    private:
        friend class Parser;
        libvlc_thumbnailer_request_t m_req;
    };

    class ThumbnailerCallbacks : protected CallbackOwner<1>
    {
    private:
        enum class CallbackIdx : unsigned int
        {
            OnThumbnailerEnded
        };

        friend class Parser;
        libvlc_thumbnailer_cbs m_cbs;

    public:
        ThumbnailerCallbacks() = delete;

        /**
         * Constructor with the mandatory on_thumbnailer_ended callback.
         * 
         * \param onThumbnailerEnded the callback to be called when a thumbnailer request finishes.
         * The callback must match the prototype defined by \ref ExpectedOnThumbnailerEndedCb.
         */
        template <typename OnThumbnailerEnded>
        ThumbnailerCallbacks( OnThumbnailerEnded&& onThumbnailerEnded )
        {
            static_assert( signature_match<OnThumbnailerEnded, ExpectedOnThumbnailerEndedCb>::value,
                           "Mismatched on_thumbnailer_ended callback prototype" );
            m_cbs = {};
            m_cbs.version = 0;
            m_cbs.on_ended = CallbackWrapper<(unsigned int)CallbackIdx::OnThumbnailerEnded,
                             decltype(libvlc_thumbnailer_cbs::on_ended)>::wrap<Parser::Task, Picture>(
                             *m_callbacks, std::forward<OnThumbnailerEnded>( onThumbnailerEnded ) );
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
     * Queue a thumbnail generation request.
     *
     * \param request the thumbnail generation request
     * \param cbs pre-built \ref Parser::ThumbnailerCallbacks object
     * \return the queued task \ref TaskIdentifier that can be used to identify the
     * task in callbacks and to cancel the task if needed
     *
     * \warning The application must ensure that the ThumbnailerCallbacks object supplied
     * remains valid and unmodified until the request terminates and the onThumbnailerEnded callback is called
     * on that Task (the returned TaskIdentifier can be used to identify the Task in the onThumbnailerEnded
     * callback using ==).
     *
     * \warning The returned TaskIdentifier is only valid till the thumbnailer request finishes
     * and the onThumbnailerEnded callback is called, after that it should not be used anymore
     * as the underlying task object is released by libVLC and the TaskIdentifier will hold a dangling pointer.
     */
    TaskIdentifier queueThumbnailing( const ThumbnailerRequest& request, const ThumbnailerCallbacks& cbs )
    {
        auto task = libvlc_parser_queue_thumbnailing( *this, &request.m_req, &cbs.m_cbs, cbs.m_callbacks.get() );
        if ( task == nullptr )
            throw std::runtime_error( "Failed to queue thumbnailer task" );
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
