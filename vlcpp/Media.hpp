/*****************************************************************************
 * Media.hpp: Media API
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

#ifndef LIBVLC_CXX_MEDIA_H
#define LIBVLC_CXX_MEDIA_H

#include "common.hpp"

#include <vector>
#include <stdexcept>

namespace VLC
{

class Instance;
class MediaList;
class TrackList;

class Media : public Internal<libvlc_media_t>
{
private:
#if !defined(_MSC_VER) || _MSC_VER >= 1900
    static constexpr unsigned int NbEvents = 4;
#else
    static const unsigned int NbEvents = 4;
#endif

public:
    ///
    /// \brief The FromType enum is used to drive the media creation.
    /// A media is usually created using a string, which can represent one of 3 things:
    ///
    enum class FromType
    {
        /**
         * Create a media for a certain file path.
         */
        FromPath,
        /**
         * Create a media with a certain given media resource location,
         * for instance a valid URL.
         *
         * \note To refer to a local file with this function,
         * the file://... URI syntax <b>must</b> be used (see IETF RFC3986).
         * We recommend using FromPath instead when dealing with
         * local files.
         */
        FromLocation,
        /**
         * Create a media as an empty node with a given name.
         */
        AsNode,
    };
    // To be able to write Media::FromLocation
#if !defined(_MSC_VER) || _MSC_VER >= 1900
    constexpr static FromType FromPath = FromType::FromPath;
    constexpr static FromType FromLocation = FromType::FromLocation;
    constexpr static FromType AsNode = FromType::AsNode;
#else
    static const FromType FromPath = FromType::FromPath;
    static const FromType FromLocation = FromType::FromLocation;
    static const FromType AsNode = FromType::AsNode;
#endif

    enum class Type
    {
        Unknown = libvlc_media_type_unknown,
        File = libvlc_media_type_file,
        Directory = libvlc_media_type_directory,
        Disc = libvlc_media_type_disc,
        Stream = libvlc_media_type_stream,
        Playlist = libvlc_media_type_playlist,
    };

    /**
     * @brief Media Constructs a libvlc Media instance
     * @param instance  A libvlc instance
     * @param mrl       A path, location, or node name, depending on the 3rd parameter
     * @param type      The type of the 2nd argument. \sa{FromType}
     */
    Media(const std::string& mrl, FromType type)
        : Internal{ libvlc_media_release }
    {
        InternalPtr ptr = nullptr;
        switch (type)
        {
        case FromLocation:
            ptr = libvlc_media_new_location( mrl.c_str() );
            break;
        case FromPath:
            ptr = libvlc_media_new_path( mrl.c_str() );
            break;
        case AsNode:
            ptr = libvlc_media_new_as_node( mrl.c_str() );
            break;
        default:
            break;
        }
        if ( ptr == nullptr )
            throw std::runtime_error("Failed to construct a media");
        m_obj.reset( ptr, libvlc_media_release );
    }

    /**
     * Create a media for an already open file descriptor.
     * The file descriptor shall be open for reading (or reading and writing).
     *
     * Regular file descriptors, pipe read descriptors and character device
     * descriptors (including TTYs) are supported on all platforms.
     * Block device descriptors are supported where available.
     * Directory descriptors are supported on systems that provide fdopendir().
     * Sockets are supported on all platforms where they are file descriptors,
     * i.e. all except Windows.
     *
     * \note This library will <b>not</b> automatically close the file descriptor
     * under any circumstance. Nevertheless, a file descriptor can usually only be
     * rendered once in a media player. To render it a second time, the file
     * descriptor should probably be rewound to the beginning with lseek().
     *
     * \param fd open file descriptor
     * \return the newly created media
     */
    Media( int fd)
        : Internal { libvlc_media_new_fd( fd ),
                     libvlc_media_release }
    {
    }

    /**
     * Get media instance from this media list instance. This action will increase
     * the refcount on the media instance.
     * The libvlc_media_list_lock should NOT be held upon entering this function.
     *
     * \param list a media list instance
     * \return media instance
     */
    Media(MediaList& list)
        : Internal{ libvlc_media_list_media( getInternalPtr<libvlc_media_list_t>( list ) ),
                    libvlc_media_release }
    {
    }

    /**
     * Callback prototype to open a custom bitstream input media.
     *
     * The same media item can be opened multiple times. Each time, this callback
     * is invoked. It should allocate and initialize any instance-specific
     * resources, then store them in *datap. The instance resources can be freed
     * in the @ref libvlc_close_cb callback.
     *
     * \param opaque private pointer as passed to libvlc_media_new_callbacks()
     * \param datap storage space for a private data pointer [OUT]
     * \param sizep byte length of the bitstream or 0 if unknown [OUT]
     *
     * \note For convenience, *datap is initially NULL and *sizep is initially 0.
     *
     * \return 0 on success, non-zero on error. In case of failure, the other
     * callbacks will not be invoked and any value stored in *datap and *sizep is
     * discarded.
     */
    using ExpectedMediaOpenCb = int(void*, void** datap, uint64_t* sizep);

    /**
     * Callback prototype to read data from a custom bitstream input media.
     *
     * \param opaque private pointer as set by the @ref libvlc_media_open_cb
     *               callback
     * \param buf start address of the buffer to read data into
     * \param len bytes length of the buffer
     *
     * \return strictly positive number of bytes read, 0 on end-of-stream,
     *         or -1 on non-recoverable error
     *
     * \note If no data is immediately available, then the callback should sleep.
     * \warning The application is responsible for avoiding deadlock situations.
     * In particular, the callback should return an error if playback is stopped;
     * if it does not return, then libvlc_media_player_stop() will never return.
     */
    using ExpectedMediaReadCb = ptrdiff_t(void* opaque, unsigned char* buf, size_t len);

    /**
     * Callback prototype to seek a custom bitstream input media.
     *
     * \param opaque private pointer as set by the @ref libvlc_media_open_cb
     *               callback
     * \param offset absolute byte offset to seek to
     * \return 0 on success, -1 on error.
     */
    using ExpectedMediaSeekCb = int(void* opaque, uint64_t);

    /**
     * Callback prototype to close a custom bitstream input media.
     * \param opaque private pointer as set by the @ref libvlc_media_open_cb
     *               callback
     */
    using ExpectedMediaCloseCb = void(void* opaque);

    class Callbacks : protected CallbackOwner<NbEvents>
    {
    private:
        enum class CallbackIdx : unsigned int
        {
            Open,
            Read,
            Seek,
            Close,
        };

        friend class Media;

        /* The boxing strategy of the read/seek/close callbacks depends on
           whether an open callback is provided: with an open callback, the
           opaque is boxed (Unbox/Cleanup), without one libvlc forwards the
           raw opaque untouched (NoBoxing). Since the open callback may be set
           after read/seek/close, we cannot pick their strategy when they are
           provided. We therefore only store the user callbacks in the setter
           methods, and defer producing the actual C function pointers to build(),
           once the full set of callbacks (hence the presence of open) is known.
           m_cbs is mutable because build() finalizes it from a const Media
           constructor. */
        mutable libvlc_media_open_cbs m_cbs;

        using OpenFn  = decltype(libvlc_media_open_cbs::open);
        using ReadFn  = decltype(libvlc_media_open_cbs::read);
        using SeekFn  = decltype(libvlc_media_open_cbs::seek);
        using CloseFn = decltype(libvlc_media_open_cbs::close);

        bool m_hasOpen = false;
        OpenFn  (*m_makeOpen)()        = nullptr;
        ReadFn  (*m_makeRead)( bool )  = nullptr;
        SeekFn  (*m_makeSeek)( bool )  = nullptr;
        CloseFn (*m_makeClose)( bool ) = nullptr;

        template <typename Func>
        static OpenFn makeOpenCb()
        {
            using CW = imem::CallbackWrapper<(unsigned int)CallbackIdx::Open, OpenFn>;
            return CW::produce<imem::BoxingStrategy::Setup, NbEvents, Func>();
        }

        template <typename Func>
        static ReadFn makeReadCb( bool hasOpen )
        {
            using CW = imem::CallbackWrapper<(unsigned int)CallbackIdx::Read, ReadFn>;
            return hasOpen ? CW::produce<imem::BoxingStrategy::Unbox, NbEvents, Func>()
                           : CW::produce<imem::BoxingStrategy::NoBoxing, NbEvents, Func>();
        }

        template <typename Func>
        static SeekFn makeSeekCb( bool hasOpen )
        {
            using CW = imem::CallbackWrapper<(unsigned int)CallbackIdx::Seek, SeekFn>;
            return hasOpen ? CW::produce<imem::BoxingStrategy::Unbox, NbEvents, Func>()
                           : CW::produce<imem::BoxingStrategy::NoBoxing, NbEvents, Func>();
        }

        template <typename Func>
        static CloseFn makeCloseCb( bool hasOpen )
        {
            using CW = imem::CallbackWrapper<(unsigned int)CallbackIdx::Close, CloseFn>;
            return hasOpen ? CW::produce<imem::BoxingStrategy::Cleanup, NbEvents, Func>()
                           : CW::produce<imem::BoxingStrategy::NoBoxing, NbEvents, Func>();
        }

        void build() const
        {
            m_cbs.read = m_makeRead( m_hasOpen );

            if ( m_makeOpen )
                m_cbs.open = m_makeOpen();
            if ( m_makeSeek )
                m_cbs.seek = m_makeSeek( m_hasOpen );
            if ( m_makeClose )
                m_cbs.close = m_makeClose( m_hasOpen );
        }

    public:
        Callbacks() = delete;

        /**
         * Constructor with the mandatory read callback.
         *
         * \param readCb callback to read data (must not be nullptr)
         *
         * \see ExpectedMediaReadCb
         */
        template <typename ReadCb>
        Callbacks( ReadCb&& readCb )
        {
            static_assert( signature_match<ReadCb, ExpectedMediaReadCb>::value,
                           "Mismatched Read callback prototype" );
            m_cbs = {};
            m_cbs.version = 0;
            imem::CallbackWrapper<(unsigned int)CallbackIdx::Read, ReadFn>::
                store( *m_callbacks, std::forward<ReadCb>( readCb ) );
            m_makeRead = &makeReadCb<ReadCb>;
        }

        /**
         * Sets the open callback.
         *
         * \param openCb callback to open the custom bitstream input media
         *
         * \see ExpectedMediaOpenCb
         *
         * \return reference to this Callbacks object for chaining
         */
        template <typename OpenCb>
        Callbacks& open( OpenCb&& openCb )
        {
            static_assert( signature_match<OpenCb, ExpectedMediaOpenCb>::value,
                           "Mismatched Open callback prototype" );
            imem::CallbackWrapper<(unsigned int)CallbackIdx::Open, OpenFn>::
                store( *m_callbacks, std::forward<OpenCb>( openCb ) );
            m_makeOpen = &makeOpenCb<OpenCb>;
            m_hasOpen = true;
            return *this;
        }

        /**
         * Sets the seek callback.
         *
         * \param seekCb callback to seek
         *
         * \see ExpectedMediaSeekCb
         *
         * \return reference to this Callbacks object for chaining
         */
        template <typename SeekCb>
        Callbacks& seek( SeekCb&& seekCb )
        {
            static_assert( signature_match<SeekCb, ExpectedMediaSeekCb>::value,
                           "Mismatched Seek callback prototype" );
            imem::CallbackWrapper<(unsigned int)CallbackIdx::Seek, SeekFn>::
                store( *m_callbacks, std::forward<SeekCb>( seekCb ) );
            m_makeSeek = &makeSeekCb<SeekCb>;
            return *this;
        }

        /**
         * Sets the close callback.
         *
         * \param closeCb callback to close the media
         *
         * \see ExpectedMediaCloseCb
         *
         * \return reference to this Callbacks object for chaining
         */
        template <typename CloseCb>
        Callbacks& close( CloseCb&& closeCb )
        {
            static_assert( signature_match<CloseCb, ExpectedMediaCloseCb>::value,
                           "Mismatched Close callback prototype" );
            imem::CallbackWrapper<(unsigned int)CallbackIdx::Close, CloseFn>::
                store( *m_callbacks, std::forward<CloseCb>( closeCb ) );
            m_makeClose = &makeCloseCb<CloseCb>;
            return *this;
        }
    };

    /**
     * Create a media with custom callbacks to read the data from.
     *
     * \param cbs pre-built \ref Media::Callbacks object
     *
     * \throw std::runtime_error if the media creation fails
     *
     * \note If open_cb is NULL, the opaque pointer will be passed to read_cb,
     * seek_cb and close_cb, and the stream size will be treated as unknown.
     *
     * \note The callbacks may be called asynchronously (from another thread).
     * A single stream instance need not be reentrant. However the open_cb needs to
     * be reentrant if the media is used by multiple player instances.
     *
     * \warning The callbacks may be used until all or any player instances
     * that were supplied the media item are stopped.
     *
     * \warning The application must ensure that the Callbacks object supplied to this constructor
     * remains valid and unmodified until all player instances that were supplied the media item are stopped.
     *
     * \see ExpectedMediaOpenCb
     * \see ExpectedMediaReadCb
     * \see ExpectedMediaSeekCb
     * \see ExpectedMediaCloseCb
     *
     * \version LibVLC 4.0.0 and later.
     */
    Media( const Callbacks& cbs )
    {
        cbs.build();
        auto ptr = libvlc_media_new_callbacks( &cbs.m_cbs, cbs.m_callbacks.get() );
        if ( ptr == nullptr )
            throw std::runtime_error( "Failed to create media" );
        m_obj.reset( ptr, libvlc_media_release );
    }

    explicit Media( Internal::InternalPtr ptr, bool incrementRefCount)
        : Internal{ ptr, libvlc_media_release }
    {
        if ( incrementRefCount && ptr != nullptr )
            retain();
    }

    /**
     * Create an empty VLC Media instance.
     *
     * Calling any method on such an instance is undefined.
    */
    Media() = default;

    /**
     * Check if 2 Media objects contain the same libvlc_media_t.
     * \param another another Media
     * \return true if they contain the same libvlc_media_t
     */
    bool operator==(const Media& another) const
    {
        return m_obj == another.m_obj;
    }

    /**
     * Add an option to the media.
     *
     * This option will be used to determine how the media_player will read
     * the media. This allows to use VLC's advanced reading/streaming options
     * on a per-media basis.
     *
     * \note The options are listed in 'vlc long-help' from the command line,
     * e.g. "-sout-all". Keep in mind that available options and their
     * semantics vary across LibVLC versions and builds.
     *
     * \warning Not all options affects libvlc_media_t objects: Specifically,
     * due to architectural issues most audio and video options, such as text
     * renderer options, have no effects on an individual media. These
     * options must be set through Instance::Instance() instead.
     *
     * \param psz_options  the options (as a string)
     */
    void addOption(const std::string& psz_options)
    {
        libvlc_media_add_option(*this,psz_options.c_str());
    }

    /**
     * Add an option to the media with configurable flags.
     *
     * This option will be used to determine how the media_player will read
     * the media. This allows to use VLC's advanced reading/streaming options
     * on a per-media basis.
     *
     * The options are detailed in vlc long-help, for instance "--sout-all".
     * Note that all options are not usable on medias: specifically, due to
     * architectural issues, video-related options such as text renderer
     * options cannot be set on a single media. They must be set on the whole
     * libvlc instance instead.
     *
     * \param psz_options  the options (as a string)
     *
     * \param i_flags  the flags for this option
     */
    void addOptionFlag(const std::string& psz_options, unsigned i_flags)
    {
        libvlc_media_add_option_flag(*this,psz_options.c_str(), i_flags);
    }

    /**
     * Get the media resource locator (mrl) from a media descriptor object
     *
     * \return string with mrl of media descriptor object
     */
    std::string mrl()
    {
        auto str = wrapCStr( libvlc_media_get_mrl(*this) );
        if ( str == nullptr )
            return {};
        return str.get();
    }

    /**
     * Duplicate a media descriptor object.
     */
    Media duplicate()
    {
        auto obj = libvlc_media_duplicate(*this);
        // Assume failure to duplicate is due to VLC_ENOMEM.
        // libvlc_media_duplicate(nullptr) would also return nullptr, but
        // we consider the use of an empty libvlcpp instance undefined.
        if ( obj == nullptr )
            throw std::bad_alloc();
        return Media( obj, false );
    }

    /**
     * Read the meta of the media.
     *
     * If the media has not yet been parsed this will return an empty string.
     *
     * This methods automatically calls parseAsync() , so after
     * calling it you may receive a libvlc_MediaMetaChanged event. If you
     * prefer a synchronous version ensure that you call parse()
     * before get_meta().
     *
     * \see parse()
     *
     * \see parseAsync()
     *
     * \see libvlc_MediaMetaChanged
     *
     * \param e_meta  the meta to read
     *
     * \return the media's meta
     */
    std::string meta(libvlc_meta_t e_meta)
    {
        auto str = wrapCStr(libvlc_media_get_meta(*this, e_meta) );
        if ( str == nullptr )
            return {};
        return str.get();
    }

    /**
     * Set the meta of the media (this function will not save the meta, call
     * libvlc_media_save_meta in order to save the meta)
     *
     * \param e_meta  the meta to write
     *
     * \param psz_value  the media's meta
     */
    void setMeta(libvlc_meta_t e_meta, const std::string& psz_value)
    {
        libvlc_media_set_meta(*this, e_meta, psz_value.c_str());
    }

    /**
     * Save the meta previously set
     *
     * \return true if the write operation was successful
     */
    bool saveMeta(const Instance& instance)
    {
        return libvlc_media_save_meta(getInternalPtr<libvlc_instance_t>(instance), *this) != 0;
    }

    /**
     * Get the current statistics about the media
     *
     * \param p_stats  structure that contain the statistics about the media
     * (this structure must be allocated by the caller)
     *
     * \return true if the statistics are available, false otherwise
     */
    bool stats(libvlc_media_stats_t * p_stats)
    {
        return libvlc_media_get_stats(*this, p_stats) != 0;
    }

    /**
     * Get duration (in ms) of media descriptor object item.
     *
     * \return duration of media item or -1 on error
     */
    libvlc_time_t duration()
    {
        return libvlc_media_get_duration(*this);
    }

    /**
     * Whether the media has been parsed.
     *
     * \return true if the media has been parsed, false otherwise
     */
    bool isParsed()
    {
        return libvlc_media_is_parsed( *this );
    }

    /**
     * Sets media descriptor's user_data. user_data is specialized data
     * accessed by the host application, VLC.framework uses it as a pointer
     * to an native object that references a libvlc_media_t pointer
     *
     * \param p_new_user_data  pointer to user data
     */
    void setUserData(void * p_new_user_data)
    {
        libvlc_media_set_user_data(*this, p_new_user_data);
    }

    /**
     * Get media descriptor's user_data. user_data is specialized data
     * accessed by the host application, VLC.framework uses it as a pointer
     * to an native object that references a libvlc_media_t pointer
     */
    void* userData()
    {
        return libvlc_media_get_user_data(*this);
    }

    /**
     * Get media descriptor's elementary streams description
     *
     * Note, you need to call parse() or play the media at least once
     * before calling this function. Not doing this will result in an empty
     * list.
     *
     * \version LibVLC 2.1.0 and later.
     *
     * \return a vector containing all tracks
     */
    std::vector<MediaTrack> tracks( MediaTrack::Type type )
    {
        using TrackListPtr = std::unique_ptr<libvlc_media_tracklist_t,
                                decltype(&libvlc_media_tracklist_delete)>;
        TrackListPtr trackList{ libvlc_media_get_tracklist( *this,
                                    static_cast<libvlc_track_type_t>( type ) ),
                                &libvlc_media_tracklist_delete };
        if ( trackList == nullptr )
            return {};
        auto count = libvlc_media_tracklist_count( trackList.get() );
        std::vector<MediaTrack> res{};
        for ( auto i = 0u; i < count; ++i )
            res.emplace_back( libvlc_media_tracklist_at( trackList.get(), i ) );
        return res;
    }

    std::shared_ptr<MediaList> subitems()
    {
        auto p = libvlc_media_subitems( *this );
        if ( p == nullptr )
            return nullptr;
        return std::make_shared<MediaList>( p );
    }

    Type type()
    {
        return static_cast<Type>( libvlc_media_get_type( *this ) );
    }

    /**
     * Add a slave to the current media.
     *
     * A slave is an external input source that may contains an additional subtitle
     * track (like a .srt) or an additional audio track (like a .ac3).
     *
     * \note This function must be called before the media is parsed (via parseWithOptions())
     *  or before the media is played (via MediaPlayer::play())
     *
     * \version LibVLC 3.0.0 and later.
     *
     * \param uri Uri of the slave (should contain a valid scheme).
     * \param type subtitle or audio
     * \param priority from 0 (low priority) to 4 (high priority)
     *
     * \return true on success, false on error.
     */
    bool addSlave(MediaSlave::Type type, unsigned priority, std::string const &uri)
    {
        return libvlc_media_slaves_add(*this, (libvlc_media_slave_type_t)type, priority, uri.c_str()) == 0;
    }

    /**
     * Clear all slaves previously added by addSlave() or
     * internally.
     *
     * \version LibVLC 3.0.0 and later.
     */
    void slavesClear()
    {
        libvlc_media_slaves_clear(*this);
    }

    /**
     * Get a media descriptor's slaves in a vector
     *
     * The list will contain slaves parsed by VLC or previously added by
     * addSlave(). The typical use case of this function is to save
     * a list of slave in a database for a later use.
     *
     * \version LibVLC 3.0.0 and later.
     *
     * \see addSlave()
     *
     * \return a vector of MediaSlave
     */
    std::vector<MediaSlave> slaves() const
    {
        libvlc_media_slave_t **list = nullptr;

        auto length = libvlc_media_slaves_get(*this, &list);
        if (length == 0)
            return {};
        auto deletor = [length](libvlc_media_slave_t **p_list) {
            libvlc_media_slaves_release(p_list, length);
        };
        std::unique_ptr<libvlc_media_slave_t*, decltype(deletor)> scope_gard(list, deletor);
        std::vector<MediaSlave> res(list, list + length);
        return res;
    }

    enum class FileStat : uint8_t
    {
        Mtime = libvlc_media_filestat_mtime,
        Size = libvlc_media_filestat_size,
    };

    std::pair<bool, uint64_t> fileStat( FileStat s )
    {
        uint64_t value = 0;
        auto res = libvlc_media_get_filestat( *this, static_cast<uint8_t>( s ), &value ) == 1;
        return std::make_pair( res, value );
    }


private:

    /**
     * Retain a reference to a media descriptor object (libvlc_media_t). Use
     * release() to decrement the reference count of a media
     * descriptor object.
     */
    void retain()
    {
        if ( isValid() )
            libvlc_media_retain(*this);
    }
};
} // namespace VLC

#endif
