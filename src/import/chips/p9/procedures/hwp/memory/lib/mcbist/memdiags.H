/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/p9/procedures/hwp/memory/lib/mcbist/memdiags.H $ */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2017                        */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */

///
/// @file memdiags.H
/// @brief API for memory diagnostics
///
// *HWP HWP Owner: Stephen Glancy <sglancy@us.ibm.com>
// *HWP HWP Backup: Marc Gollub <gollub@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 1
// *HWP Consumed by: HB:FSP
//

#ifndef _MSS_MEMDIAGS_H_
#define _MSS_MEMDIAGS_H_

#include <fapi2.H>
#include <lib/shared/mss_const.H>
#include <lib/mcbist/mcbist.H>
#include <lib/mcbist/address.H>
#include <lib/mcbist/patterns.H>
#include <lib/mcbist/settings.H>

namespace mss
{

namespace memdiags
{

// Map some of the mcbist namespace here to make it easier for users of memdiags
// This is an intentional using statement in a header which is typically
// disallowed - I am intentionally pulling these into this namespace for all callers.
using mss::mcbist::constraints;
using mss::mcbist::speed;
using mss::mcbist::end_boundary;
using mss::mcbist::stop_conditions;
using mss::mcbist::cache_line;
using mss::mcbist::pattern;
using mss::mcbist::patterns;

// Why not mss::mcbist::address? Because the fields can't be pulled in via using,
// and it seems even more confusing to have a memdiags address but have to use
// mcbist fields. So, we all use mcbist address until such time that its promoted
// to some other general namespace.

using mss::mcbist::PATTERN_ZEROS;
using mss::mcbist::PATTERN_0;
using mss::mcbist::PATTERN_ONES;
using mss::mcbist::PATTERN_1;
using mss::mcbist::PATTERN_2;
using mss::mcbist::PATTERN_3;
using mss::mcbist::PATTERN_4;
using mss::mcbist::PATTERN_5;
using mss::mcbist::PATTERN_6;
using mss::mcbist::PATTERN_7;
using mss::mcbist::PATTERN_8;
using mss::mcbist::PATTERN_RANDOM;
using mss::mcbist::LAST_PATTERN;
using mss::mcbist::NO_PATTERN;

///
/// @brief Stop the current command
/// @tparam T the fapi2::TargetType of the target
/// @param[in] i_target the target
/// @return FAPI2_RC_SUCCESS iff ok
///
template< fapi2::TargetType T >
fapi2::ReturnCode stop( const fapi2::Target<T>& i_target );

///
/// @class Base class for memdiags operations
/// @tparam T fapi2::TargetType of the MCBIST engine
///
template< fapi2::TargetType T >
class operation
{
    public:
        ///
        /// @brief memdiags::operation constructor
        /// @param[in] i_target the target of the mcbist engine
        /// @param[in] i_subtest the proper subtest for this operation
        /// @param[in] i_const mss::constraint structure
        ///
        operation( const fapi2::Target<T>& i_target,
                   const mss::mcbist::subtest_t<T> i_subtest,
                   const constraints i_const ):
            iv_target(i_target),
            iv_subtest(i_subtest),
            iv_const(i_const)
        {
            FAPI_TRY( mss::is_simulation (iv_l_sim) );
            return;

        fapi_try_exit:
            // Seems like a safe risk to take ...
            FAPI_ERR("Unable to get the attribute ATTR_IS_SIMULATION");
            return;
        }

        operation() = delete;

        ///
        /// @brief Execute the memdiags operation
        /// @return FAPI2_RC_SUCCESS iff ok
        ///
        inline fapi2::ReturnCode execute()
        {
            return mss::mcbist::execute(iv_target, iv_program);
        }

        ///
        /// @brief memdiags::operation destructor
        ///
        virtual ~operation() = default;

        ///
        /// @brief memdiags init helper
        /// Initializes common sections. Broken out rather than the base class ctor to enable checking return codes
        /// in subclassed constructores more easily.
        /// @return FAPI2_RC_SUCCESS iff everything ok
        ///
        fapi2::ReturnCode base_init();

        ///
        /// @brief memdiags multi-port init helper
        /// Initializes common sections. Broken out rather than the base class ctor to enable checking return codes
        /// in subclassed constructores more easily.
        /// @return FAPI2_RC_SUCCESS iff everything ok
        ///
        fapi2::ReturnCode multi_port_init();

        ///
        /// @brief memdiags multi-port address config helper
        /// Initializes the address configs needed for a multi port operation
        /// @return FAPI2_RC_SUCCESS iff everything ok
        ///
        fapi2::ReturnCode multi_port_addr();

        ///
        /// @brief Single port initializer
        /// Initializes common sections. Broken out rather than the base class ctor to enable checking return codes
        /// in subclassed constructores more easily.
        /// @return FAPI2_RC_SUCCESS iff everything ok
        ///
        fapi2::ReturnCode single_port_init();

        ///
        /// @brief get the protected mcbist program - useful for testing
        /// @return a reference to the iv_program member
        /// @note Intentionally not const ref; allows getter to set.
        ///
        mss::mcbist::program<T>& get_program()
        {
            return iv_program;
        }

        ///
        /// @brief get the protected mcbist subtest_t - useful for testing
        /// @return a reference to the iv_subtest member
        ///
        const mss::mcbist::subtest_t<T>& get_subtest() const
        {
            return iv_subtest;
        }

    protected:
        fapi2::Target<T>          iv_target;
        mss::mcbist::subtest_t<T> iv_subtest;
        constraints               iv_const;
        mss::mcbist::program<T>   iv_program;
        uint8_t                   iv_l_sim;
};

///
/// @class Class for memdiags' super-fast init
/// @tparam T fapi2::TargetType of the MCBIST engine
///
template< fapi2::TargetType T >
struct sf_init_operation : public operation<T>
{

    ///
    /// @brief memdiags::sf_init_operation constructor
    /// @param[in] i_target the target of the mcbist engine
    /// @param[in] i_const mss::constraint structure
    /// @param[out] o_rc the fapi2::ReturnCode of the intialization process
    ///
    sf_init_operation( const fapi2::Target<T>& i_target,
                       const constraints i_const,
                       fapi2::ReturnCode& o_rc ):
        operation<T>(i_target, mss::mcbist::init_subtest<T>(), i_const)
    {
        // If sf_init was passed the random data pattern, then modify the subtest to use the true random data
        if(i_const.iv_pattern == PATTERN_RANDOM)
        {
            this->iv_subtest.change_data_mode(mss::mcbist::data_mode::RAND_FWD_MAINT);
        }

        // We're a multi-port operation
        o_rc = this->multi_port_init();
    }

    sf_init_operation() = delete;
};

///
/// @class Class for memdiags' super-fast read
/// @tparam T fapi2::TargetType of the MCBIST engine
///
template< fapi2::TargetType T >
struct sf_read_operation : public operation<T>
{

    ///
    /// @brief memdiags::sf_read_operation constructor
    /// @param[in] i_target the target of the mcbist engine
    /// @param[in] i_const mss::constraint structure
    /// @param[out] o_rc the fapi2::ReturnCode of the intialization process
    ///
    sf_read_operation( const fapi2::Target<T>& i_target,
                       const constraints i_const,
                       fapi2::ReturnCode& o_rc ):
        operation<T>(i_target, mss::mcbist::read_subtest<T>(), i_const)
    {
        // We're a multi-port operation
        o_rc = this->multi_port_init();
    }

    sf_read_operation() = delete;
};

///
/// @class Class for memdiags' super-fast read to end of port
/// @tparam T fapi2::TargetType of the MCBIST engine
///
template< fapi2::TargetType T >
struct sf_read_eop_operation : public operation<T>
{
    ///
    /// @brief memdiags::sf_read_operation constructor
    /// @param[in] i_target the target of the mcbist engine
    /// @param[in] i_const mss::constraint structure
    /// @param[out] o_rc the fapi2::ReturnCode of the intialization process
    ///
    sf_read_eop_operation( const fapi2::Target<T>& i_target,
                           const constraints i_const,
                           fapi2::ReturnCode& o_rc ):
        operation<T>(i_target, mss::mcbist::read_subtest<T>(), i_const)
    {
        // We're a single-port operation
        o_rc = this->single_port_init();
    }

    sf_read_eop_operation() = delete;
};

///
/// @class Class for memdiags' continuous scrub
/// @tparam T fapi2::TargetType of the MCBIST engine
///
template< fapi2::TargetType T >
struct continuous_scrub_operation : public operation<T>
{

    ///
    /// @brief memdiags::continuous_scrub_operation constructor
    /// @param[in] i_target the target of the mcbist engine
    /// @param[in] i_const the contraints of the operation
    /// @param[out] o_rc the fapi2::ReturnCode of the intialization process
    ///
    continuous_scrub_operation( const fapi2::Target<T>& i_target,
                                const constraints i_const,
                                fapi2::ReturnCode& o_rc );

    continuous_scrub_operation() = delete;
};

///
/// @class Class for memdiags' targeted scrub
/// @tparam T fapi2::TargetType of the MCBIST engine
///
template< fapi2::TargetType T >
struct targeted_scrub_operation : public operation<T>
{

    ///
    /// @brief memdiags::targeted_scrub_operation constructor
    /// @param[in] i_target the target of the mcbist engine
    /// @param[in] i_const the contraints of the operation
    /// @param[out] o_rc the fapi2::ReturnCode of the intialization process
    ///
    targeted_scrub_operation( const fapi2::Target<T>& i_target,
                              const constraints i_const,
                              fapi2::ReturnCode& o_rc ):
        operation<T>(i_target, mss::mcbist::scrub_subtest<T>(), i_const)
    {
        // Scrub operations run 128B
        this->iv_program.change_len64(mss::OFF);

        // We're a single-port operation
        o_rc = this->single_port_init();

        // Targeted scrub needs to force a pause and the end boundary. So we make sure that happens here.
        this->iv_program.change_forced_pause( i_const.iv_end_boundary );
    }

    targeted_scrub_operation() = delete;
};

///
/// @brief Super Fast Init - used to init all memory behind a target with a given pattern
/// @note Uses broadcast mode if possible
/// @tparam T the fapi2::TargetType of the target
/// @param[in] i_target the target behind which all memory should be initialized
/// @param[in] i_pattern an index representing a pattern to use to init memory (defaults to 0)
/// @return FAPI2_RC_SUCCESS iff everything ok
/// @note The function is asynchronous, and the caller should be looking for a done attention
///
template< fapi2::TargetType T >
fapi2::ReturnCode sf_init( const fapi2::Target<T>& i_target,
                           const uint64_t i_pattern = PATTERN_0 );

///
/// @brief Super Fast Read - used to run superfast read on all memory behind the target
/// Determines ability to braodcast to all ports behind a target, does so if possible.
/// @tparam T the fapi2::TargetType of the target
/// @param[in] i_target the target behind which all memory should be read
/// @param[in] i_stop stop conditions
/// @param[in] i_address mcbist::address representing the address from which to start.
//    Defaults to the first address behind the target
/// @param[in] i_end whether to end, and where
///   Defaults to stop after slave rank
/// @return FAPI2_RC_SUCCESS iff everything ok
/// @note The function is asynchronous, and the caller should be looking for a done attention
/// @note The address is often the port, dimm, rank but this is not enforced in the API.
///
template< fapi2::TargetType T >
fapi2::ReturnCode sf_read( const fapi2::Target<T>& i_target,
                           const stop_conditions& i_stop,
                           const mss::mcbist::address& i_address = mss::mcbist::address(),
                           const end_boundary i_end = end_boundary::STOP_AFTER_SLAVE_RANK );

///
/// @brief Scrub - continuous scrub all memory behind the target
/// @param[in] i_target the target behind which all memory should be scrubbed
/// @param[in] i_stop stop conditions
/// @param[in] i_speed the speed to scrub
/// @param[in] i_address mcbist::address representing the address from which to start.
/// @return FAPI2_RC_SUCCESS iff everything ok
/// @note The function is asynchronous, and the caller should be looking for a done attention
/// @note The address is often the port, dimm, rank but this is not enforced in the API.
///
template< fapi2::TargetType T >
fapi2::ReturnCode background_scrub( const fapi2::Target<T>& i_target,
                                    const stop_conditions& i_stop,
                                    const speed i_speed,
                                    const mss::mcbist::address& i_address );

///
/// @brief Scrub - targeted scrub all memory described by the input address (rank, slave, etc.)
/// @param[in] i_target the target behind which all memory should be scrubbed
/// @param[in] i_stop stop conditions
/// @param[in] i_speed the speed to scrub
/// @param[in] i_start_address mcbist::address representing the address from which to start.
/// @param[in] i_end_address mcbist::address representing the address at which to end.
/// @param[in] i_end whether to end, and where
/// @return FAPI2_RC_SUCCESS iff everything ok
/// @note The function is asynchronous, and the caller should be looking for a done attention
/// @note The address is often the port, dimm, rank but this is not enforced in the API.
///
template< fapi2::TargetType T >
fapi2::ReturnCode targeted_scrub( const fapi2::Target<T>& i_target,
                                  const stop_conditions& i_stop,
                                  const mss::mcbist::address& i_start_address,
                                  const mss::mcbist::address& i_end_address,
                                  const end_boundary i_end );

///
/// @brief Continue current command on next address
/// The current commaand has paused on an error, so we can record the address of the error
/// and finish the current master or slave rank.
/// @tparam T the fapi2::TargetType of the target
/// @param[in] i_target the target
/// @param[in] i_end whether to end, and where (default - don't stop at end of rank)
/// @param[in] i_stop stop conditions (default - 0 meaning 'don't change conditions')
/// @param[in] i_speed the speed to scrub (default - SAME_SPEED meaning leave speed untouched)
/// @return FAPI2_RC_SUCCESS iff ok
///
template< fapi2::TargetType T >
fapi2::ReturnCode continue_cmd( const fapi2::Target<T>& i_target,
                                const end_boundary i_end = end_boundary::DONT_CHANGE,
                                const stop_conditions& i_stop = stop_conditions(stop_conditions::DONT_CHANGE),
                                const speed i_speed = speed::SAME_SPEED );

} // namespace

} // namespace
#endif
