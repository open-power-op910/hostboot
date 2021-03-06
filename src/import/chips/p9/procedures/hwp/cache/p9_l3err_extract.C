/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/p9/procedures/hwp/cache/p9_l3err_extract.C $ */
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

///----------------------------------------------------------------------------
///
/// @file p9_l3err_extract.C
///
/// @brief Parse and extract error information from L3 Scom Registers (FAPI2)
///
/// *HWP HWP Owner   : Chen Qian <qianqc@cn.ibm.com>
/// *HWP FW Owner    : Thi Tran <thi@us.ibm.com>
/// *HWP Team        : Quad
/// *HWP Consumed by : HB
/// *HWP Level       : 3
///----------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <p9_l3err_extract.H>
#include <p9_quad_scom_addresses.H>
#include <p9_quad_scom_addresses_fld.H>

//------------------------------------------------------------------------------
// Constant definitions
//------------------------------------------------------------------------------
const uint8_t P9_L3_HALF_MACRO_INDEX = 73;
const uint8_t P9_L3ERR_EXTRACT_ECC_PAT[P9_L3_HALF_MACRO_INDEX] =
{
    0xc4, 0x8c, 0x94, 0xd0, 0xf4, 0xb0, 0xa8, 0xe0,
    0x62, 0x46, 0x4a, 0x68, 0x7a, 0x58, 0x54, 0x70,
    0x31, 0x23, 0x25, 0x34, 0x3d, 0x2c, 0x2a, 0x38,
    0x98, 0x91, 0x92, 0x1a, 0x9e, 0x16, 0x15, 0x1c,
    0x4c, 0xc8, 0x49, 0x0d, 0x4f, 0x0b, 0x8a, 0x0e,
    0x26, 0x64, 0xa4, 0x86, 0xa7, 0x85, 0x45, 0x07,
    0x13, 0x32, 0x52, 0x43, 0xd3, 0xc2, 0xa2, 0x83,
    0x89, 0x19, 0x29, 0xa1, 0xe9, 0x61, 0x51, 0xc1,
    0xc7, 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
};
const uint8_t L3ERR_MAX_CYCLES_BACK   = 13;
const uint8_t L3ERR_NUM_DWS = 8;

//------------------------------------------------------------------------------
// Function definitions
//------------------------------------------------------------------------------

// See doxygen in header file
fapi2::ReturnCode p9_l3err_extract(
    const fapi2::Target<fapi2::TARGET_TYPE_EX>& i_target,
    p9_l3err_extract_err_data& o_err_data,
    bool& o_error_found)
{

    fapi2::buffer<uint64_t> l_l3eDRAM_rd_err_stat_data0;
    fapi2::buffer<uint64_t> l_l3eDRAM_rd_err_stat_data1;

    uint8_t             err_1st_beat_syndrome = 0;
    uint8_t             err_2nd_beat_syndrome = 0;
    uint8_t             syndrome = 0;
    bool                err_in_first_beat = 0;
    uint8_t             even_odd = 0;

    bool                err_stat_val = 0;
    bool                err_stat_1st_beat_UE = 0;
    bool                err_stat_2nd_beat_UE = 0;
    uint8_t             err_stat_dw = 0;
    uint8_t             err_stat_dw_odd = 0;
    uint8_t             dw = 0;
    uint16_t            err_stat_ra = 0;
    uint8_t             err_stat_bank = 0;

    uint8_t             dataout = 0;
    uint8_t             syndrome_col = 0;
    uint8_t             member = 0;
    uint8_t             member_select = 0;
    uint8_t             read_select = 0;
    bool                ce_ue = true;
    bool                sue = false;

    uint16_t            real_address45_52 = 0;
    uint16_t            real_address53_54 = 0;
    uint16_t            real_address55 = 0;
    uint16_t            real_address56 = 0;
    uint16_t            real_address45_56 = 0;


    // mark function entry
    FAPI_DBG("Entering ...");


    // Get the values of l3eDRAM_rd_err_stat_register_0 & l3eDRAM_rd_err_stat_register_1
    FAPI_TRY(fapi2::getScom(i_target, EX_ED_RD_ERR_STAT_REG0, l_l3eDRAM_rd_err_stat_data0),
             "Error from getScom (L3_eDRAM_RD_ERR_STAT_Register_0)");

    FAPI_TRY(fapi2::getScom(i_target, EX_L3_ED_RD_ERR_STAT_REG1, l_l3eDRAM_rd_err_stat_data1),
             "Error from getScom (L3_eDRAM_RD_ERR_STAT_Register_1)");

    FAPI_DBG("l_l3eDRAM_rd_err_stat_data0: %#lx", l_l3eDRAM_rd_err_stat_data0);
    FAPI_DBG("l_l3eDRAM_rd_err_stat_data1: %#lx", l_l3eDRAM_rd_err_stat_data1);

    // err_stat_val, err_stat_1st_beat_UE, err_stat_2nd_beat_UE
    l_l3eDRAM_rd_err_stat_data0.extractToRight<EX_ED_RD_ERR_STAT_REG0_L3_VAL, 1>
    (err_stat_val);

    l_l3eDRAM_rd_err_stat_data0.extractToRight<EX_ED_RD_ERR_STAT_REG0_L3_1ST_BEAT_UE, 1>
    (err_stat_1st_beat_UE);

    l_l3eDRAM_rd_err_stat_data0.extractToRight<EX_ED_RD_ERR_STAT_REG0_L3_2ND_BEAT_UE, 1>
    (err_stat_2nd_beat_UE);


    FAPI_DBG("err_stat_val = %x", err_stat_val);
    o_error_found = err_stat_val;

    // Return if no CE or UE error found by Scom registers
    if (!err_stat_val)
    {
        FAPI_DBG ("No error is found!");
        return fapi2::current_err;
    }


    // syndrome value
    l_l3eDRAM_rd_err_stat_data0.extractToRight
    <EX_ED_RD_ERR_STAT_REG0_L3_1ST_BEAT_SYNDROME,
    EX_ED_RD_ERR_STAT_REG0_L3_1ST_BEAT_SYNDROME_LEN>
    (err_1st_beat_syndrome);

    l_l3eDRAM_rd_err_stat_data0.extractToRight
    <EX_ED_RD_ERR_STAT_REG0_L3_2ND_BEAT_SYNDROME,
    EX_ED_RD_ERR_STAT_REG0_L3_2ND_BEAT_SYNDROME_LEN>
    (err_2nd_beat_syndrome);

    //check the first beat of data for bad syndrome
    FAPI_DBG("Checking first beat for syndrome data.");

    if( err_1st_beat_syndrome == 0)
    {
        FAPI_DBG("Checking second beat for syndrome data.");
        syndrome = err_2nd_beat_syndrome;
        ce_ue = !(err_stat_2nd_beat_UE);

        if (ce_ue == false)
        {
            if (err_2nd_beat_syndrome == 0xFE)
            {
                sue = true;
            }
            else
            {
                sue = false;
            }
        }

    }
    else
    {
        syndrome = err_1st_beat_syndrome;
        err_in_first_beat = 1;
        ce_ue = !(err_stat_1st_beat_UE);

        if (ce_ue == false)
        {
            if (err_1st_beat_syndrome == 0xFE)
            {
                sue = true;
            }
            else
            {
                sue = false;
            }
        }
    }

    // if could not find syndrome
    FAPI_ASSERT(syndrome,
                fapi2::P9_L3ERR_EXTRACT_SYNDROME_NOT_FOUND()
                .set_TARGET(i_target)
                .set_FIRST_BEAT_SYNDROME(err_1st_beat_syndrome)
                .set_SECOND_BEAT_SYNDROME(err_2nd_beat_syndrome),
                "Error: could not find syndrome. "
                "1st beat syndrome 0x%.8X, 2nd beat syndrome 0x%.8X",
                err_1st_beat_syndrome, err_2nd_beat_syndrome);

    FAPI_DBG("Found syndrome: %2X", syndrome);

    // read_select value
    l_l3eDRAM_rd_err_stat_data1.extractToRight<13, 1>(read_select);

    //calculate dataout (column)
    if( err_in_first_beat )
    {
        even_odd = read_select;
    }
    else
    {
        even_odd = !read_select;
    }

    FAPI_DBG("readselect=%u,err_in_first_beat=%s,even_odd=%u",
             read_select, err_in_first_beat ? "true" : "false", even_odd);

    //if a CE occurred, process the syndrome
    if( ce_ue )
    {
        //decodes the specified syndrome into a column offset
        bool found = false;

        //use the ECC lookup to find what column the error occured
        for( uint8_t i = 0;
             i < (uint8_t)(sizeof(P9_L3ERR_EXTRACT_ECC_PAT) / sizeof(uint8_t));
             i++)
        {
            if( syndrome == P9_L3ERR_EXTRACT_ECC_PAT[i] )
            {
                syndrome_col = i;
                found = true;
                break;
            }
        }

        // if the syndrome_col value can not found in the P9_L3ERR_EXTRACT_ECC_PAT
        FAPI_ASSERT(found,
                    fapi2::P9_L3ERR_EXTRACT_UNKNOWN_SYNDROME_ECC().
                    set_TARGET(i_target).
                    set_SYNDROME(syndrome),
                    "Syndrome ECC is unknown. %2X", syndrome);

        FAPI_DBG("syndrome_col = %u", syndrome_col);
        // caculate the dataout
        dataout = ( syndrome_col * 2 ) + even_odd;
    }
    else
    {
        dataout = 0;
    }

    // dw, ra and member_select
    // l_l3eDRAM_rd_err_stat_register0[20] says whether dw is even/odd
    // l_l3eDRAM_rd_err_stat_register0[21:22] encode which of the 4 DW's on an even/odd side
    l_l3eDRAM_rd_err_stat_data0.extractToRight < EX_ED_RD_ERR_STAT_REG0_L3_DW + 1, 2 >
    (err_stat_dw);
    l_l3eDRAM_rd_err_stat_data0.extractToRight<EX_ED_RD_ERR_STAT_REG0_L3_DW, 1>
    (err_stat_dw_odd);

    if( err_stat_dw_odd == 1)
    {
        dw = err_stat_dw * 2 + 1;       // odd side
    }
    else
    {
        dw = err_stat_dw * 2;           // even side
    }

    l_l3eDRAM_rd_err_stat_data1.extractToRight
    <EX_L3_ED_RD_ERR_STAT_REG1_L3_RA,
    EX_L3_ED_RD_ERR_STAT_REG1_L3_RA_LEN>(err_stat_ra);

    l_l3eDRAM_rd_err_stat_data1.extractToRight
    <EX_L3_ED_RD_ERR_STAT_REG1_L3_BANK,
    EX_L3_ED_RD_ERR_STAT_REG1_L3_BANK_LEN>(err_stat_bank);

    l_l3eDRAM_rd_err_stat_data1.extractToRight<12, 1>(member_select);

    if(member_select)
    {
        member = err_stat_bank + 10;
    }
    else
    {
        member = err_stat_bank;
    }

    // Translate cache read address to real address first
    // The EDRAM read-address is made up of edram_ra(0:9) & edram_ca(0:2) & edram_rs.
    // Edram_ra(0:9) are taken from hashed address(56, 55, 45:52)
    // Edram_ca(0:2) are taken from hashed address(53:54)& memberGroup (member 0-9 vs.10-19)
    // Edram_rs is taken from  address(57)
    real_address45_52   = err_stat_ra & (0x0FF0);
    real_address53_54   = err_stat_ra & (0x000C);
    real_address55      = (err_stat_ra & (0x1000)) >> 11;
    real_address56      = (err_stat_ra & (0x2000)) >> 13;
    real_address45_56   = real_address45_52 | real_address53_54 | real_address55 | real_address56;

    //print out error location information
    if( ce_ue )
    {
        FAPI_DBG("CE Location Information");
    }
    else
    {
        if(!sue)
        {

            FAPI_DBG("UE Location Information");
        }
        else
        {
            FAPI_DBG("SUE Location Information");
        }

    }

    FAPI_DBG("DW     = %u", dw);
    FAPI_DBG("Bank   = %u", err_stat_bank);
    FAPI_DBG("Member = %u", member);
    FAPI_DBG("Cache Read Address = 0x%X", err_stat_ra);
    FAPI_DBG("Hashed Real Address (45:56) = 0x%X", real_address45_56);
    FAPI_DBG("Dataout = 0x%X", dataout);

    // output error data
    //o_err_data.ce_ue = ce_ue ? L3ERR_CE : L3ERR_UE;
    if (ce_ue)
    {
        o_err_data.ce_ue = L3ERR_CE;
    }
    else
    {
        if(!sue)
        {
            o_err_data.ce_ue = L3ERR_UE;
        }
        else
        {
            o_err_data.ce_ue = L3ERR_SUE;
        }
    }

    o_err_data.member = member;
    o_err_data.dw = dw;
    o_err_data.bank = err_stat_bank;
    o_err_data.dataout = dataout;
    o_err_data.hashed_real_address_45_56 = real_address45_56;
    o_err_data.cache_read_address = err_stat_ra;

fapi_try_exit:
    // mark HWP exit
    FAPI_DBG("Exiting ...");
    return fapi2::current_err;
} // p9_l3err_extract
