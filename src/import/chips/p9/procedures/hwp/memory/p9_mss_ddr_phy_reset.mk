# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: chips/p9/procedures/hwp/memory/p9_mss_ddr_phy_reset.mk $
#
# IBM CONFIDENTIAL
#
# EKB Project
#
# COPYRIGHT 2015
# [+] International Business Machines Corp.
#
#
# The source code for this program is not published or otherwise
# divested of its trade secrets, irrespective of what has been
# deposited with the U.S. Copyright Office.
#
# IBM_PROLOG_END_TAG

# Include the macros and things for MSS procedures
-include 00common.mk

PROCEDURE=p9_mss_ddr_phy_reset
$(eval $(call ADD_MEMORY_INCDIRS,$(PROCEDURE)))
$(call BUILD_PROCEDURE)
