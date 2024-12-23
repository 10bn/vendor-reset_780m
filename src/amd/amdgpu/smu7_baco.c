/*
 * Copyright 2019 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */
#include "amd.h"
#include "common.h"
#include "vendor-reset-dev.h"
#include "smu7_baco.h"
#include "polaris_baco.h"

#include "bif/bif_5_0_d.h"
#include "bif/bif_5_0_sh_mask.h"

#include "smu/smu_7_1_2_d.h"
#include "smu/smu_7_1_2_sh_mask.h"

int smu7_baco_get_capability(struct amd_fake_dev *adev, bool *cap)
{
	uint32_t reg;

	*cap = false;
	/* we're going to assume true */
	/* if (!phm_cap_enabled(hwmgr->platform_descriptor.platformCaps, PHM_PlatformCaps_BACO))
		return 0; */

	reg = RREG32(mmCC_BIF_BX_FUSESTRAP0);

	if (reg & CC_BIF_BX_FUSESTRAP0__STRAP_BIF_PX_CAPABLE_MASK)
		*cap = true;

	return 0;
}

int smu7_baco_get_state(struct amd_fake_dev *adev, enum BACO_STATE *state)
{
	uint32_t reg;

	reg = RREG32(mmBACO_CNTL);

	if (reg & BACO_CNTL__BACO_MODE_MASK)
		/* gfx has already entered BACO state */
		*state = BACO_STATE_IN;
	else
		*state = BACO_STATE_OUT;
	return 0;
}

int smu7_baco_set_state(struct amd_fake_dev *adev, enum BACO_STATE state)
{
	switch (adev->vdev->info)
	{
	case AMD_POLARIS10:
	case AMD_POLARIS11:
	case AMD_POLARIS12:
		return polaris_baco_set_state(adev, state);
	default:
		return -EINVAL;
	}
}
