/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/usr/isteps/istep08/call_host_p9_fbc_eff_config_links.C $  */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016                             */
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
#include <stdint.h>
#include <trace/interface.H>
#include <errl/errlentry.H>
#include <errl/errlmanager.H>
#include <initservice/taskargs.H>
#include <initservice/isteps_trace.H>
#include <initservice/initserviceif.H>
#include <isteps/hwpisteperror.H>
#include <fapi2/plat_hwp_invoker.H>
#include <fapi2/target.H>
#include <p9_fbc_eff_config_links.H>

namespace ISTEP_08
{

void* call_host_p9_fbc_eff_config_links( void *io_pArgs )
{
    errlHndl_t l_errl = NULL;
    ISTEP_ERROR::IStepError l_stepError;

    TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
               "call_host_p9_fbc_eff_config_links entry" );

    TARGETING::TargetHandleList l_procChips;
    getAllChips( l_procChips, TARGETING::TYPE_PROC);
    for (const auto & l_procChip: l_procChips)
    {
        fapi2::Target<fapi2::TARGET_TYPE_PROC_CHIP>l_fapi2CpuTarget(l_procChip);
        FAPI_INVOKE_HWP(l_errl,p9_fbc_eff_config_links,l_fapi2CpuTarget,
                        SMP_ACTIVATE_PHASE1,
                        true,false);
        if(l_errl)
        {
            l_stepError.addErrorDetails(l_errl);
            TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
                       "ERROR : call call_host_p9_fbc_eff_config_links, "
                       "PLID=0x%x",
                       l_errl->plid() );
            errlCommit(l_errl, HWPF_COMP_ID);
        }
    }


    TRACFCOMP( ISTEPS_TRACE::g_trac_isteps_trace,
               "call_host_p9_fbc_eff_config_links exit" );

    return l_stepError.getErrorHandle();
}

};
