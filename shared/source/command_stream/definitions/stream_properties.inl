/*
 * Copyright (C) 2021 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "shared/source/command_stream/stream_property.h"

namespace NEO {

struct StateComputeModeProperties {
    StreamProperty isCoherencyRequired{};

    void setProperties(bool requiresCoherency, uint32_t numGrfRequired, bool isMultiOsContextCapable,
                       bool useGlobalAtomics, bool areMultipleSubDevicesInContext);
    void setProperties(const StateComputeModeProperties &properties);
    bool isDirty();
    void clearIsDirty();
};

struct FrontEndProperties {
    void setProperties(bool isCooperativeKernel, const HardwareInfo &hwInfo);
    void setProperties(const FrontEndProperties &properties);
    bool isDirty();
    void clearIsDirty();
};

} // namespace NEO
