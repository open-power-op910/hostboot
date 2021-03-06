/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/p9/procedures/hwp/pm/p9_pm_recovery_ffdc_qppm.C $ */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2017                             */
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
// *INDENT-OFF*


///
/// @file   p9_pm_recovery_ffdc_qppm.C
/// @brief  QPPM FFDC collection of PM complex
///
/// *HWP HWP Owner:      Greg Still <stillgs@us.ibm.com>
/// *HWP FW Owner:       Prasad Bg Ranganath <prasadbgr@in.ibm.com>
/// *HWP Team:           PM
/// *HWP Level:          2
/// *HWP Consumed by:    Hostboot
//
// *INDENT-OFF*
//--------------------------------------------------------------------------
// Includes
//--------------------------------------------------------------------------

#include <p9_pm_recovery_ffdc_qppm.H>
#include <collect_reg_ffdc.H>
#include <stddef.h>
#include <endian.h>

 namespace p9_stop_recov_ffdc
 {
    QppmRegs::QppmRegs( const fapi2::Target< fapi2::TARGET_TYPE_PROC_CHIP > i_procChipTgt )
      : PlatPmComplex( i_procChipTgt,0,0,0,PLAT_PPM)
    { }

    //----------------------------------------------------------------------

    fapi2::ReturnCode QppmRegs::collectRegFfdc( void * i_pHomerBuf )
    {
        FAPI_DBG(">> QppmRegs::collectRegFfdc");
        fapi2::current_err = fapi2::FAPI2_RC_SUCCESS;
        fapi2::ReturnCode l_rc = fapi2::current_err;
        auto l_quadList =
            getProcChip().getChildren< fapi2::TARGET_TYPE_EQ > ( fapi2::TARGET_STATE_PRESENT );
        uint8_t l_quadPos = 0;
        uint8_t *l_pFfdcLoc = NULL;
        uint8_t l_ffdcValid = 0;
        HomerFfdcRegion * l_pHomerFfdc =
                ( HomerFfdcRegion *)( (uint8_t *)i_pHomerBuf + FFDC_REGION_HOMER_BASE_OFFSET );

        for( auto quad : l_quadList )
        {
            FAPI_TRY( FAPI_ATTR_GET( fapi2::ATTR_CHIP_UNIT_POS, quad, l_quadPos ),
                      "FAPI_ATTR_GET Failed To Read QUAD Position" );

            l_pFfdcLoc = &l_pHomerFfdc->iv_quadFfdc[l_quadPos].iv_quadQppmRegion[0];
            if( quad.isFunctional() )
            {
                l_ffdcValid = 1;
                FAPI_INF("QPPM FFDC Pos %d ", l_quadPos);

                l_rc = collectRegisterData<fapi2::TARGET_TYPE_EQ> (quad,
                                            l_pFfdcLoc + sizeof(PpmFfdcHeader),
                                            static_cast<fapi2::HwpFfdcId>(fapi2::QPPM_FFDC_REGISTERS));
                if (l_rc )
                {
                    l_ffdcValid = 0;
                }
            }


            FAPI_TRY( updateQppmFfdcHeader( l_pFfdcLoc, l_quadPos, l_ffdcValid),
                      "Failed To Update QPPM FFDC Header for quad 0x%0d", l_quadPos);
        }

        fapi_try_exit:
        FAPI_DBG("<< QppmRegs::collectRegFfdc");
        return fapi2::current_err;
    }

    fapi2::ReturnCode QppmRegs::updateQppmFfdcHeader( uint8_t * i_pHomerBuf, 
                                                      const uint8_t i_quadPos,
                                                      const uint8_t i_ffdcValid)
    {
        FAPI_DBG(">> updateQppmFfdcHeader" );

        PpmFfdcHeader * l_QppmFfdcHdr       =   (PpmFfdcHeader *) i_pHomerBuf ;
        l_QppmFfdcHdr->iv_ppmMagicWord      =  htobe32(FFDC_QPPM_MAGIC_NUM);
        l_QppmFfdcHdr->iv_Instance          =  i_quadPos;
        l_QppmFfdcHdr->iv_ppmHeaderSize     =  sizeof(PpmFfdcHeader);
        l_QppmFfdcHdr->iv_sectionSize       =  FFDC_QPPM_REGISTERS_SIZE;
        l_QppmFfdcHdr->iv_ffdcValid         =  i_ffdcValid;

        FAPI_DBG("<< updateQppmFfdcHeader" );
        return fapi2::FAPI2_RC_SUCCESS;
    }
    //-----------------------------------------------------------------------



extern "C"
{
    fapi2::ReturnCode p9_pm_recovery_ffdc_qppm( const fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP >& i_procChip, void * i_pFfdcBuf )
    {
        FAPI_IMP(">> p9_pm_recovery_ffdc_qppm" );
        QppmRegs l_qppmFfdc( i_procChip );
        FAPI_TRY( l_qppmFfdc.collectRegFfdc( i_pFfdcBuf ),
                  "Failed To Collect QPPM FFDC" );

        fapi_try_exit:
        FAPI_IMP("<< p9_pm_recovery_ffdc_qppm" );
        return fapi2::current_err;
    }

}

 }//namespace p9_stop_recov_ffdc ends
