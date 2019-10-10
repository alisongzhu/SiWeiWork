
/*
 * Copyright (c) 2015 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_DEVICE_H_
#define ZEPHYR_INCLUDE_DEVICE_H_

/**
 * @brief Device Driver APIs
 * @defgroup io_interfaces Device Driver APIs
 * @{
 * @}
 */
/**
 * @brief Device Model APIs
 * @defgroup device_model Device Model APIs
 * @{
 */

#include <zephyr/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define Z_DEVICE_MAX_NAME_LEN	48


struct device;


/**
 * @brief Static device information (In ROM) Per driver instance
 *
 * @param name name of the device
 * @param init init function for the driver
 * @param config_info address of driver instance config information
 */
struct device_config {
	const char *name;
	int (*init)(struct device *device);
	const void *config_info;
};

/**
 * @brief Runtime device structure (In memory) Per driver instance
 * @param device_config Build time config information
 * @param driver_api pointer to structure containing the API functions for
 * the device type. This pointer is filled in by the driver at init time.
 * @param driver_data driver instance data. For driver use only
 */
struct device {
	struct device_config *config;
	const void *driver_api;
	void *driver_data;
};

static inline int devive_init(struct device *dev)
{
  struct device_config *config = dev->config;
  return config->init(dev);
}


#ifdef __cplusplus
}
#endif
/**
 * @}
 */
#endif /* ZEPHYR_INCLUDE_DEVICE_H_ */
