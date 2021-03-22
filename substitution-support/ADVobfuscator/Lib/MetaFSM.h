//
//  MetaFSM.h
//  ADVobfuscator
//
// Copyright (c) 2010-2017, Sebastien Andrivet
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Get latest version on https://github.com/andrivet/ADVobfuscator

#ifndef MetaFSM_h
#define MetaFSM_h

#include <iostream>
#include <tuple>
#include <type_traits>

#pragma warning(push)
#pragma warning(disable: 4127 4100)
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
// functors
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/euml/common.hpp>
#pragma warning(pop)

#include "Indexes.h"
#include "Unroller.h"

// Code common to our FSM (finite state machines)

namespace msm = boost::msm;
namespace mpl = boost::mpl;
using namespace boost::msm::front;

namespace andrivet { namespace ADVobfuscator {

    // Same as void but can be instantiated
    struct Void {};

    // Event template to call a function F with a list of parameters.
    // Note: F is passed as value.
    template<typename R, typename F, typename... Args>
    struct event
    {
        // Constructor
        constexpr event(F f, Args&... args): f_{f}, data_{args...} {}

        // Call target function
        R call() const
        {
            // Generate a list of indexes to extract arguments from tuple
            using I = typename Make_Indexes<sizeof...(Args)>::type;
            return call_(I{});
        }

    private:
        // When F is returning a value
        template<typename U = R, int... I>
        typename std::enable_if<!std::is_same<U, Void>::value, U>::type
        call_(Indexes<I...>) const { return f_.original()(std::get<I>(data_)...); }

        // When F does not return a value (void)
        template<typename U = R, int... I>
        typename std::enable_if<std::is_same<U, Void>::value, Void>::type
        call_(Indexes<I...>) const { f_.original()(std::get<I>(data_)...); return Void{}; }

    private:
        F f_;
        std::tuple<Args&...> data_;
    };

    // When function F is returning a value
    // FSM: Finite State Machine
    // R: Type of the returned value
    // F: Function (target)
    // Args: Arguments of target
    template<template<typename, typename> class FSM, typename R, typename F, typename... Args>
    inline R ObfuscatedCallRet(F f, Args&&... args)
    {
        using E = event<R, F, Args&...>;
        using M = msm::back::state_machine<FSM<E, R>>;
        using Run = typename FSM<E, R>::template Run<F, Args...>;

        M machine;
        Run::run(machine, f, std::forward<Args>(args)...);
        return machine.result_;
    }

    // When function F is not returning a value
    // FSM: Finite State Machine
    // F: Function (target)
    // Args: Arguments of target
    template<template<typename, typename = Void> class FSM, typename F, typename... Args>
    inline void ObfuscatedCall(F f, Args&&... args)
    {
        using E = event<Void, F, Args&...>;
        using M = msm::back::state_machine<FSM<E, Void>>;
        using Run = typename FSM<E, Void>::template Run<F, Args...>;

        M machine;
        Run::run(machine, f, std::forward<Args>(args)...);
    }

    // Note: It is possible to merge these two members with ObfuscatedCall and ObfuscatedCallRet (by introducing a TruePredicate) but it will make the 1st FSM example more complicated.

    // When function F is returning a value
    // FSM: Finite State Machine
    // R: Type of the returned value
    // P: Predicate (functor)
    // F: Function (target)
    // Args: Arguments of target
    template<template<typename, typename, typename> class FSM, typename R, typename P, typename F, typename... Args>
    inline R ObfuscatedCallRetP(F f, Args&&... args)
    {
        using E = event<R, F, Args&...>;
        using M = msm::back::state_machine<FSM<E, P, R>>;
        using Run = typename FSM<E, P, R>::template Run<F, Args...>;

        M machine;
        Run::run(machine, f, std::forward<Args>(args)...);
        return machine.result_;
    }

    // When function F is not returning a value
    // FSM: Finite State Machine
    // P: Predicate
    // F: Function (target)
    // Args: Arguments of target
    template<template<typename, typename, typename = Void> class FSM, typename P, typename F, typename... Args>
    inline void ObfuscatedCallP(F f, Args&&... args)
    {
        using E = event<Void, F, Args&...>;
        using M = msm::back::state_machine<FSM<E, P, Void>>;
        using Run = typename FSM<E, P, Void>::template Run<F, Args...>;

        M machine;
        Run::run(machine, f, std::forward<Args>(args)...);
    }

    // Obfuscate the address of the target. Very simple implementation but enough to annoy IDA and Co.
    template<typename F>
    struct ObfuscatedAddress
    {
        // Pointer to a function
        using func_ptr_t = void(*)();
        // Integral type big enough (and not too big) to store a function pointer
        using func_ptr_integral = std::conditional<sizeof(func_ptr_t) <= sizeof(long), long, long long>::type;

        func_ptr_integral f_;
        int offset_;

        constexpr ObfuscatedAddress(F f, int offset): f_{reinterpret_cast<func_ptr_integral>(f) + offset}, offset_{offset} {}
        constexpr F original() const { return reinterpret_cast<F>(f_ - offset_); }
    };

    // Create a instance of ObfuscatedFunc and deduce types
    template<typename F>
    constexpr ObfuscatedAddress<F> MakeObfuscatedAddress(F f, int offset) { return ObfuscatedAddress<F>(f, offset); }

}}

#endif
