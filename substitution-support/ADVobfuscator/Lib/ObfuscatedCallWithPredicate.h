//
//  ObfuscatedCallWithPredicate.h
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

#ifndef ObfuscatedCallWithPredicate_h
#define ObfuscatedCallWithPredicate_h

#include "MetaFSM.h"
#include "MetaRandom.h"

// Obfuscate function call with a finite state machine (FSM).
// In this example, the execution of the FSM is combined with the checking of a predicate (is a debugger present or not).

namespace andrivet { namespace ADVobfuscator { namespace Machine2 {

    // Finite State Machine
    // E: Event associated with target
    // P: Predicate
    // R: Type of return value
    template<typename E, typename P, typename R = Void>
    struct Machine : public msm::front::state_machine_def<Machine<E, R>>
    {
        // -- Events
        struct event1 {};
        struct event2 {};

        // --- States
        struct State1 : public msm::front::state<>{};
        struct State2 : public msm::front::state<>{};
        struct State3 : public msm::front::state<>{};
        struct State4 : public msm::front::state<>{};
        struct State5 : public msm::front::state<>{};
        struct State6 : public msm::front::state<>{};
        struct Final  : public msm::front::state<>{};

        // --- Transitions
        struct CallTarget
        {
            template<typename EVT, typename FSM, typename SRC, typename TGT>
            void operator()(EVT const& evt, FSM& fsm, SRC&, TGT&)
            {
                fsm.result_ = evt.call();
            }
        };

        struct CallPredicate
        {
            template<typename EVT, typename FSM, typename SRC, typename TGT>
            void operator()(EVT const&, FSM& fsm, SRC&, TGT&)
            {
                fsm.predicateCounter_ += P{}();
            }
        };

        struct Increment
        {
            template<typename EVT, typename FSM, typename SRC, typename TGT>
            void operator()(EVT const&, FSM& fsm, SRC&, TGT&)
            {
                ++fsm.predicateCounter_;
            }
        };

        // --- Guards
        struct Predicate
        {
            template<typename EVT, typename FSM, typename SRC, typename TGT>
            bool operator()(EVT const&, FSM& fsm, SRC&, TGT&)
            {
                return (fsm.predicateCounter_ - fsm.predicateCounterInit_) % 2 == 0;
            }
        };

        struct NotPredicate
        {
            template<typename EVT, typename FSM, typename SRC, typename TGT>
            bool operator()(EVT const& evt, FSM& fsm, SRC& src, TGT& tgt)
            {
                return !Predicate{}(evt, fsm, src, tgt);
            }
        };

        // --- Initial state of the FSM. Must be defined
        using initial_state = State1;

        // --- Transition table
        struct transition_table : mpl::vector<
        //    Start     Event         Next      Action               Guard
        //  +---------+-------------+---------+---------------------+----------------------+
        Row < State1  , event1      , State2                                               >,
        Row < State1  , E           , State5                                               >,
        //  +---------+-------------+---------+---------------------+----------------------+
        Row < State2  , event1      , State3  , CallPredicate                              >,
        Row < State2  , event2      , State1  , none                , Predicate            >,
        Row < State2  , event2      , State4  , none                , NotPredicate         >,
        //  +---------+-------------+---------+---------------------+----------------------+
        Row < State3  , event1      , State2  , Increment                                  >,
        //  +---------+-------------+---------+---------------------+----------------------+
        Row < State4  , E           , State5  , CallTarget                                 >,
        //  +---------+-------------+---------+---------------------+----------------------+
        Row < State5  , event2      , State6                                               >,
        //  +---------+-------------+---------+---------------------+----------------------+
        Row < State6  , event1      , Final                                                >
        //  +---------+-------------+---------+---------------------+----------------------+
        > {};

        using StateMachine = msm::back::state_machine<Machine<E, P, R>>;

        template<typename F, typename... Args>
        struct Run
        {
            static inline void run(StateMachine& machine, F f, Args&&... args)
            {
                machine.start();

                machine.process_event(event1{});

                // Generate a lot of transitions (at least 19, at most 19 + 2 * 40)
                // Important: This has to be an odd number to detect if the predicate is true or not
                // This is computed at Compile-Time
                Unroller<19 + 2 * MetaRandom<__COUNTER__, 40>::value>{}([&]()
                {
                    machine.process_event(event1{});
                    machine.process_event(event1{});
                });

                machine.process_event(event2{});

                // Call our target. Will be actually called only if predicate P is true
                machine.process_event(E{f, args...});

                machine.process_event(event2{});
                machine.process_event(event1{});
            }
        };


        // Result of the target
        R result_;

        // Counter to obfuscate predicate result
        static const int predicateCounterInit_ = 100 + MetaRandom<__COUNTER__, 999>::value;
        int predicateCounter_ = predicateCounterInit_;
    };

}}}

// Warning: ##__VA_ARGS__ is not portable (only __VA_ARGS__ is). However, ##__VA_ARGS__ is far better (handles cases when it is empty) and supported by most compilers

#pragma warning(push)
#pragma warning(disable : 4068)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

#define OBFUSCATED_CALL_P0(P, f) andrivet::ADVobfuscator::ObfuscatedCallP<andrivet::ADVobfuscator::Machine2::Machine, P>(MakeObfuscatedAddress(f, andrivet::ADVobfuscator::MetaRandom<__COUNTER__, 400>::value + 278))
#define OBFUSCATED_CALL_RET_P0(R, P, f) andrivet::ADVobfuscator::ObfuscatedCallRetP<andrivet::ADVobfuscator::Machine2::Machine, P, R>(MakeObfuscatedAddress(f, andrivet::ADVobfuscator::MetaRandom<__COUNTER__, 400>::value + 278))

#define OBFUSCATED_CALL_P(P, f, ...) andrivet::ADVobfuscator::ObfuscatedCallP<andrivet::ADVobfuscator::Machine2::Machine, P>(MakeObfuscatedAddress(f, andrivet::ADVobfuscator::MetaRandom<__COUNTER__, 400>::value + 278), ##__VA_ARGS__)
#define OBFUSCATED_CALL_RET_P(R, P, f, ...) andrivet::ADVobfuscator::ObfuscatedCallRetP<andrivet::ADVobfuscator::Machine2::Machine, P, R>(MakeObfuscatedAddress(f, andrivet::ADVobfuscator::MetaRandom<__COUNTER__, 400>::value + 278), ##__VA_ARGS__)

#pragma clang diagnostic pop
#pragma warning(pop)


#endif
