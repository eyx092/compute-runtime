/*
 * Copyright (C) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once
#include <level_zero/zet_api.h>

#include "third_party/level_zero/zes_api_ext.h"

#include <vector>

struct _zet_sysman_temp_handle_t {
    virtual ~_zet_sysman_temp_handle_t() = default;
};

struct _zes_temp_handle_t {
    virtual ~_zes_temp_handle_t() = default;
};

namespace L0 {

struct OsSysman;
class Temperature : _zet_sysman_temp_handle_t, _zes_temp_handle_t {
  public:
    virtual ze_result_t temperatureGetProperties(zet_temp_properties_t *pProperties) = 0;
    virtual ze_result_t temperatureGetConfig(zet_temp_config_t *pConfig) = 0;
    virtual ze_result_t temperatureSetConfig(const zet_temp_config_t *pConfig) = 0;
    virtual ze_result_t temperatureGetState(double *pTemperature) = 0;

    virtual ze_result_t temperatureGetProperties(zes_temp_properties_t *pProperties) = 0;
    virtual ze_result_t temperatureGetConfig(zes_temp_config_t *pConfig) = 0;
    virtual ze_result_t temperatureSetConfig(const zes_temp_config_t *pConfig) = 0;

    static Temperature *fromHandle(zet_sysman_temp_handle_t handle) {
        return static_cast<Temperature *>(handle);
    }
    static Temperature *fromHandle(zes_temp_handle_t handle) {
        return static_cast<Temperature *>(handle);
    }
    inline zet_sysman_temp_handle_t toZetHandle() { return this; }
    inline zes_temp_handle_t toHandle() { return this; }
    bool initSuccess = false;
};

struct TemperatureHandleContext {
    TemperatureHandleContext(OsSysman *pOsSysman) : pOsSysman(pOsSysman){};
    ~TemperatureHandleContext();

    void init();

    ze_result_t temperatureGet(uint32_t *pCount, zet_sysman_temp_handle_t *phTemperature);
    ze_result_t temperatureGet(uint32_t *pCount, zes_temp_handle_t *phTemperature);

    OsSysman *pOsSysman = nullptr;
    std::vector<Temperature *> handleList = {};

  private:
    void createHandle(zet_temp_sensors_t type);
    void createHandle(zes_temp_sensors_t type);
};

} // namespace L0