/*****************************************************************************
 * common.hpp: Basic shared types & helpers
 *****************************************************************************
 * Copyright © 2015 libvlcpp authors & VideoLAN
 *
 * Authors: Hugo Beauzée-Luyssen <hugo@beauzee.fr>
 *          Jonathan Calmels <exxo@videolabs.io>
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

#ifndef LIBVLC_CXX_COMMON_H
#define LIBVLC_CXX_COMMON_H

#include <vlc/vlc.h>
#include <array>
#include <cassert>
#include <memory>

namespace VLC
{
    class Media;
    using MediaPtr = std::shared_ptr<Media>;

    // Work around cross class dependencies
    // Class A needs to access B's internal pointer
    // Class B needs to access A's internal pointer
    // By using a template function to do this, we're delegating
    // the access to both classes' guts to a later point, when the compiler
    // already knows everything it needs.
    // The only drawback is that we can't return decltype(ptr->get()), since when
    // the compiler checks for the prototype, it hasn't parsed all the declarations yet.
    template <typename TYPE, typename REF>
    TYPE* getInternalPtr(const REF& ref)
    {
        return ref.get();
    }

    inline std::unique_ptr<char, void (*)(void*)> wrapCStr(char* str)
    {
        return std::unique_ptr<char, void(*)(void*)>( str, [](void* ptr) { libvlc_free(ptr); } );
    }

#if !defined(_MSC_VER)
    // Kudos to 3xxO for the signature_match helper
    template <typename, typename, typename = void>
    struct signature_match : std::false_type {};

    template <typename Func, typename Ret, typename... Args>
    struct signature_match<Func, Ret(Args...),
        typename std::enable_if<
            std::is_convertible<
                decltype(std::declval<Func>()(std::declval<Args>()...)),
                Ret
            >::value // true or false
        >::type // void or SFINAE
    > : std::true_type {};
#else
    template <typename... Args>
    struct signature_match : std::false_type {};

    template <typename Func, typename Ret, class... Args>
    struct signature_match<Func, Ret(Args...)>
        : std::integral_constant < bool,
            std::is_convertible < decltype(std::declval<Func>()(std::declval<Args>()...)), Ret
        > ::value
    > {};
#endif

    template <typename Func, typename Ret, typename... Args>
    struct signature_match_or_nullptr : std::integral_constant<bool,
        signature_match<Func, Ret, Args...>::value
        >
    {
    };

    template <typename Func>
    struct signature_match_or_nullptr<std::nullptr_t, Func> : std::true_type
    {
    };

    struct CallbackHandlerBase
    {
        virtual ~CallbackHandlerBase() = default;
    };

    template <typename Func>
    struct CallbackHandler : public CallbackHandlerBase
    {
        CallbackHandler(Func&& f) : func( std::forward<Func>( f ) ) {}
        Func func;
    };

    template <int NbEvent>
    struct CallbackOwner
    {
        std::array<std::shared_ptr<CallbackHandlerBase>, NbEvent> callbacks;

    protected:
        CallbackOwner() = default;
    };

    template <int, typename>
    struct FromOpaque;

    template <int NbEvents>
    struct FromOpaque<NbEvents, void*>
    {
        static CallbackOwner<NbEvents>* get(void* opaque)
        {
            return reinterpret_cast<CallbackOwner<NbEvents>*>( opaque );
        }
    };

    template <int NbEvents>
    struct FromOpaque<NbEvents, void**>
    {
        static CallbackOwner<NbEvents>* get(void** opaque)
        {
            return reinterpret_cast<CallbackOwner<NbEvents>*>( *opaque );
        }
    };

    template <int Idx, typename... Args>
    struct CallbackWrapper;

    // We assume that any callback will take a void*/void** opaque as its first parameter.
    // We intercept this parameter, and use it to fetch the list of user provided
    // functions. Once we know what function to call, we forward the rest of the
    // parameters.
    // Using partial specialization also allows us to get the list of the expected
    // callback parameters automatically, rather than having to specify them.
    template <int Idx, typename Ret, typename Opaque, typename... Args>
    struct CallbackWrapper<Idx, Ret(*)(Opaque, Args...)>
    {
        using Wrapped = Ret(*)(Opaque, Args...);

        template <int NbEvents, typename Func>
        static Wrapped wrap(CallbackOwner<NbEvents>* owner, Func&& func)
        {
            owner->callbacks[Idx] = std::make_shared<CallbackHandler<Func>>( std::forward<Func>( func ) );
            return [](Opaque opaque, Args... args) -> Ret {
                auto self = FromOpaque<NbEvents, Opaque>::get( opaque );
                assert(self->callbacks[Idx].get());
                auto cbHandler = static_cast<CallbackHandler<Func>*>( self->callbacks[Idx].get() );
                return cbHandler->func( std::forward<Args>(args)... );
            };
        }

        // Overload to handle null callbacks at build time.
        // We could try to compare any "Func" against nullptr at runtime, though
        // since Func is a template type, which roughly has to satisfy the "Callable" concept,
        // it could be an instance of a function object, which doesn't compare nicely against nullptr.
        // Using the specialization at build time is easier and performs better.
        template <int NbEvents>
        static std::nullptr_t wrap(CallbackOwner<NbEvents>*, std::nullptr_t)
        {
            return nullptr;
        }
    };

    struct VaCopy
    {
        VaCopy(va_list va_)
        {
            va_copy( va, va_ );
        }
        ~VaCopy()
        {
            va_end( va );
        }

        VaCopy( const VaCopy& ) = delete;
        VaCopy& operator=(const VaCopy& ) = delete;
        VaCopy( VaCopy&& ) = delete;
        VaCopy& operator=( VaCopy&& ) = delete;

        va_list va;
    };
}

#endif
