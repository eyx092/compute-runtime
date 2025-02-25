/*
 * Copyright (C) 2021 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "shared/source/gmm_helper/windows/gmm_memory_base.h"
#include "shared/source/helpers/debug_helpers.h"
#include "shared/source/os_interface/windows/windows_defs.h"

#include "gmm_client_context.h"

namespace NEO {

bool GmmMemoryBase::configureDeviceAddressSpace(GMM_ESCAPE_HANDLE hAdapter,
                                                GMM_ESCAPE_HANDLE hDevice,
                                                GMM_ESCAPE_FUNC_TYPE pfnEscape,
                                                GMM_GFX_SIZE_T SvmSize,
                                                BOOLEAN BDWL3Coherency) {
    return true;
}

}; // namespace NEO
