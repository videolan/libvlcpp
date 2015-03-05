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

    template <typename Func, typename Ret, typename... Args>
    struct signature_match_or_nullptr : std::integral_constant<bool,
            signature_match<Func, Ret, Args...>::value ||
            std::is_same<Func, std::nullptr_t>::value
        >
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
    struct EventOwner
    {
        std::array<std::shared_ptr<CallbackHandlerBase>, NbEvent> callbacks;

    protected:
        EventOwner() = default;
    };

    template <int Idx, typename Func, typename... Args>
    struct CallbackWrapper;

    // We assume that any callback will take a void* opaque as its first parameter.
    // We intercept this parameter, and use it to fetch the list of user provided
    // functions. Once we know what function to call, we forward the rest of the
    // parameters.
    // Using partial specialization also allows us to get the list of the expected
    // callback parameters automatically, rather than having to specify them.
    template <int Idx, typename Func, typename Ret, typename... Args>
    struct CallbackWrapper<Idx, Func, Ret(*)(void*, Args...)>
    {
        using Wrapped = Ret(void*, Args...);
        template <int NbEvents>
        static Wrapped* wrap(EventOwner<NbEvents>* owner, Func&& func)
        {
            owner->callbacks[Idx] = std::shared_ptr<CallbackHandlerBase>( new CallbackHandler<Func>( std::forward<Func>( func ) ) );
            return [](void* opaque, Args... args) -> Ret {
                auto self = reinterpret_cast<EventOwner<NbEvents>*>(opaque);
                assert(self->callbacks[Idx].get());
                auto cbHandler = static_cast<CallbackHandler<Func>*>( self->callbacks[Idx].get() );
                return cbHandler->func( std::forward<Args>(args)... );
            };
        }
    };

    // Special handling for events with a void** opaque first parameter.
    // We fetch it and do our business with it, then forward the parameters
    // to the user callback, without this opaque param.
    template <int Idx, typename Func, typename Ret, typename... Args>
    struct CallbackWrapper<Idx, Func, Ret(*)(void**, Args...)>
    {
        using Wrapped = Ret(void**, Args...);
        template <int NbEvents>
        static Wrapped* wrap(EventOwner<NbEvents>* owner, Func&& func)
        {
            owner->callbacks[Idx] = std::shared_ptr<CallbackHandlerBase>( new CallbackHandler<Func>( std::forward<Func>( func ) ) );
            return [](void** opaque, Args... args) -> Ret {
                auto self = reinterpret_cast<EventOwner<NbEvents>*>(*opaque);
                assert(self->callbacks[Idx].get());
                auto cbHandler = static_cast<CallbackHandler<Func>*>( self->callbacks[Idx].get() );
                return cbHandler->func( std::forward<Args>(args)... );
            };
        }
    };

    // Specialization to handle null callbacks at build time.
    // We could try to compare any "Func" against nullptr at runtime, though
    // since Func is a template type, which roughly has to satisfy the "Callable" concept,
    // it could be an instance of a function object, which doesn't compare nicely against nullptr.
    // Using the specialization at build time is easier and performs better.
    template <int Idx, typename... Args>
    struct CallbackWrapper<Idx, std::nullptr_t, void(*)(void*, Args...)>
    {
        template <int NbEvents>
        static std::nullptr_t wrap(EventOwner<NbEvents>*, std::nullptr_t)
        {
            return nullptr;
        }
    };
}

#endif
