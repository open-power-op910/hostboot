/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/p9/procedures/hwp/pm/p9_pm_recovery_ffdc_base.H $ */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2017                        */
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
#ifndef __P9_PM_RECOVERY_BASE_
#define __P9_PM_RECOVERY_BASE_


///
/// @file   p9_stop_ffdc_base.H
/// @brief  Models generic platform for the FFDC collection of PM complex
///
/// *HWP HWP Owner:      Greg Still <stillgs@us.ibm.com>
/// *HWP FW Owner:       Prem S Jha <premjha2@in.ibm.com>
/// *HWP Team:           PM
/// *HWP Level:          2
/// *HWP Consumed by:    Hostboot:Phyp
//
// *INDENT-OFF*
//--------------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------
#include <fapi2.H>
#include <collect_reg_ffdc.H>
#include <stdint.h>
#include <p9_hcd_memmap_cme_sram.H>
#include <p9_hcd_memmap_occ_sram.H>
#include <p9_pm_recovery_ffdc_defines.H>
#include <p9_pm_ocb_indir_access.H>
#include <p9_cme_sram_access.H>
#include <p9_pm_ocb_indir_setup_linear.H>
#include <p9_ppe_utils.H>

namespace p9_stop_recov_ffdc
{
    class PlatPmComplex
    {
        public:
            /// @brief  constructor
            /// @param[in]  i_procChipTgt       fapi2 target for P9 chip
            /// @param[in]  i_imageHdrBaseAddr  sram address of start of image header
            /// @param[in]  i_traceBufBaseAddr  sram address of start of trace buffer
            /// @param[in]  i_globalBaseAddr    sram address of start of global variables
            /// @param[in]  i_plat              platform id
            PlatPmComplex( const fapi2::Target< fapi2::TARGET_TYPE_PROC_CHIP > i_procChipTgt,
                           uint32_t i_imageHdrBaseAddr, uint32_t i_traceBufBaseAddr,
                           uint32_t i_globalBaseAddr,
                           PmComplexPlatId i_plat );

            /// @brief  destructor
            virtual ~PlatPmComplex() { };

            /// @brief      collects category of FFDC common for all platforms in PM complex.
            /// @param[in]  i_pHomerBuf     points to base of P9 HOMER.
            //  @return     fapi2 return code.
            virtual fapi2::ReturnCode collectFfdc( void* i_pHomerBuf );

            /// @brief  sets start address of platform's trace buffer.
            void setTraceBufAddr (uint32_t i_addr)
            { iv_traceBufBaseAddress = i_addr; }

            ///@brief returns instance id.
            PmComplexPlatId getPlatId() { return iv_plat ; }

            /// @brief returns proc chip target associated with platform
            fapi2::Target< fapi2::TARGET_TYPE_PROC_CHIP > getProcChip() const { return iv_procChip; }

        protected:
            ///@brief reads the PPE Halt State from XSR, w/o halting the PPE
            ///@param[in]  i_xirBaseAddress XCR SCOM Address of the PPE
            ///@param[out] o_haltCondition  p9_stop_recov_ffdc::PpeHaltCondition
            ///@return  fapi2 return code
            fapi2::ReturnCode readPpeHaltState (
                      const uint64_t i_xirBaseAddress,
                      uint8_t&       o_haltCondition );

            ///@brief collects PPE State (XIRs, SPRs, GPRs) to a loc in HOMER
            ///@param[in] i_xirBaseAddress XCR SCOM Address of the PPE
            ///@param[in] i_pHomerOffset   PPE section base address in HOMER
            ///@param[in] i_mode           PPE_DUMP_MODE, defaults to HALT
            ///@return  fapi2 return code
            fapi2::ReturnCode collectPpeState (
                   const uint64_t i_xirBaseAddress,
                   const uint8_t* i_pHomerOffset,
                   const PPE_DUMP_MODE i_mode = HALT );

            ///@brief collects FFDC from CME/OCC SRAM
            ///@param[in]   i_chipletTarget     fapi2 target for EX or Proc
            ///@param[in]   i_pSramData         points to HOMER location containing SRAM contents
            ///@param[in]   i_dataType          type of FFDC data
            ///@param[in]   i_sramLength        length of SRAM FFDC in bytes
            ///@return  fapi2 return code
            fapi2::ReturnCode collectSramInfo( const fapi2::Target< fapi2::TARGET_TYPE_EX >& i_exTgt,
                                               uint8_t * i_pSramData,
                                               FfdcDataType i_dataType,
                                               uint32_t i_sramLength );

            fapi2::ReturnCode collectSramInfo( const fapi2::Target< fapi2::TARGET_TYPE_PROC_CHIP > & i_procChip,
                                               uint8_t * i_pSramData,
                                               FfdcDataType i_dataType,
                                               uint32_t i_sramLength );

            ///@brief  Collects register data
            ///param[in]    i_chipletTarget Chip/chilpet target
            ///param[out]   o_pHomerBuf     Homer base address to fill register
            //                              data
            // param[in]    i_ffdcId        Hwp ffdc id to know register
            //                              collection type
            ///@return  fapi2 return code
            template<fapi2::TargetType T>
            fapi2::ReturnCode collectRegisterData( const fapi2::Target<T>& i_chipletTarget,
                                                   uint8_t* o_pHomerBuf,
                                                   fapi2::HwpFfdcId i_ffdcId);


            ///@brief  updates parts of PPE FFDC Header common for all platforms.
            ///param[in]    i_pFfdcHdr      points to the PPE FFDC header
            ///param[in]    i_ffdcValid     bit vector summarizing FFDC validity
            ///param[in]    i_haltState     FFDC halt state
            ///@note  refer to PPE Spec for halt state description.
            fapi2::ReturnCode updatePpeFfdcHeader( PpeFfdcHeader * i_pFfdcHdr,
                                                   uint8_t i_ffdcValid, uint8_t i_haltState );
            #ifndef __HOSTBOOT_MODULE
            ///@brief to debug FFDC contents collected from SRAM.
            ///param[in]    i_pSram     points to location of SRAM info in HOMER.
            ///param[in]    i_len       length of info.
            ///@return  fapi2 return code
            fapi2::ReturnCode debugSramInfo( uint8_t * i_pSram, uint32_t i_len );
            #endif

        private:
            fapi2::Target< fapi2::TARGET_TYPE_PROC_CHIP > iv_procChip;  // processor chip target
            uint32_t iv_imageHeaderBaseAddress;     // base address of platform's image header
            uint32_t iv_traceBufBaseAddress;        // base address of platforms's trace buffer
            uint32_t iv_globalBaseAddress;          // base address of platform's global variables
            PmComplexPlatId iv_plat;
    };

    template<fapi2::TargetType T>
    fapi2::ReturnCode PlatPmComplex::collectRegisterData(const fapi2::Target<T>& i_chipletTarget,
                                           uint8_t *o_pHomerBuf,
                                           fapi2::HwpFfdcId i_ffdcId)
    {
        FAPI_DBG(">> collectRegisterData");
        std::vector<uint32_t> l_cfamAddresses;
        std::vector<uint64_t> l_scomAddresses;
        uint32_t l_ffdcRegReadSize = 0;
        uint32_t l_offset = 0;
        fapi2::ScomReader<T> l_scomReader(i_chipletTarget);

        fapi2::getAddressData(i_ffdcId, l_scomAddresses, l_cfamAddresses, l_ffdcRegReadSize);

        FAPI_TRY((fapi2::collectRegisterAndAddressData<uint64_t,
                    fapi2::ScomReader<T> >(l_scomAddresses, l_scomReader,
                                        l_offset, o_pHomerBuf)), 
                   "Failed in collectRegisterAndAddressData");

        fapi_try_exit:
        FAPI_DBG("<< collectRegisterData");
        return fapi2::current_err;
    }

} //namespace p9_stop_recov_ffdc ends

#endif //__P9_PM_RECOVERY_BASE_
