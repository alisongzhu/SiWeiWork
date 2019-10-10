/*
 * Copyright (c) 2017 Linaro Limited
 * Copyright (c) 2017 BayLibre, SAS.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "stm32f1xx_hal.h"
#include "flash_stm32.h"
#include "include/flash.h"
#include "lib/include/device.h"

#define STM32_FLASH_TIMEOUT	((uint32_t) 0x000B0000)

/*
 * This is named flash_stm32_sem_take instead of flash_stm32_lock (and
 * similarly for flash_stm32_sem_give) to avoid confusion with locking
 * actual flash pages.
 */
static inline void flash_stm32_sem_take(struct device *dev)
{
  xSemaphoreTake(FLASH_STM32_PRIV(dev)->sem, portMAX_DELAY);
}

static inline void flash_stm32_sem_give(struct device *dev)
{
  xSemaphoreGive(FLASH_STM32_PRIV(dev)->sem);
}

static int flash_stm32_check_status(struct device *dev)
{
	uint32_t const error =
#if defined(FLASH_FLAG_PGAERR)
		FLASH_FLAG_PGAERR |
#endif
#if defined(FLASH_FLAG_RDERR)
		FLASH_FLAG_RDERR  |
#endif
#if defined(FLASH_FLAG_PGPERR)
		FLASH_FLAG_PGPERR |
#endif
#if defined(FLASH_FLAG_PGSERR)
		FLASH_FLAG_PGSERR |
#endif
#if defined(FLASH_FLAG_OPERR)
		FLASH_FLAG_OPERR |
#endif
#if defined(FLASH_FLAG_PGERR)
		FLASH_FLAG_PGERR |
#endif
		FLASH_FLAG_WRPERR;

	if (FLASH_STM32_REGS(dev)->sr & error) {
		return -EIO;
	}

	return 0;
}

int flash_stm32_wait_flash_idle(struct device *dev)
{
	uint32_t timeout = STM32_FLASH_TIMEOUT;
	int rc;

	rc = flash_stm32_check_status(dev);
	if (rc < 0) {
		return -EIO;
	}

	while ((FLASH_STM32_REGS(dev)->sr & FLASH_SR_BSY) && timeout) {
		timeout--;
	}

	if (!timeout) {
		return -EIO;
	}

	return 0;
}

#if !defined(CONFIG_SOC_SERIES_STM32F1X)
static void flash_stm32_flush_caches(struct device *dev)
{
#if defined(CONFIG_SOC_SERIES_STM32F4X)
	struct stm32f4x_flash *regs = FLASH_STM32_REGS(dev);
#elif defined(CONFIG_SOC_SERIES_STM32L4X)
	struct stm32l4x_flash *regs = FLASH_STM32_REGS(dev);
#endif

	if (regs->acr.val & FLASH_ACR_ICEN) {
		regs->acr.val &= ~FLASH_ACR_ICEN;
		regs->acr.val |= FLASH_ACR_ICRST;
		regs->acr.val &= ~FLASH_ACR_ICRST;
		regs->acr.val |= FLASH_ACR_ICEN;
	}

	if (regs->acr.val & FLASH_ACR_DCEN) {
		regs->acr.val &= ~FLASH_ACR_DCEN;
		regs->acr.val |= FLASH_ACR_DCRST;
		regs->acr.val &= ~FLASH_ACR_DCRST;
		regs->acr.val |= FLASH_ACR_DCEN;
	}
}
#endif

static int flash_stm32_read(struct device *dev, off_t offset, void *data,
			    size_t len)
{
	if (!flash_stm32_valid_range(dev, offset, len, false)) {
		return -EINVAL;
	}

	if (!len) {
		return 0;
	}

	memcpy(data, (uint8_t *) CONFIG_FLASH_BASE_ADDRESS + offset, len);

	return 0;
}

static int flash_stm32_erase(struct device *dev, off_t offset, size_t len)
{
	int rc;

	if (!flash_stm32_valid_range(dev, offset, len, true)) {
		return -EINVAL;
	}

	if (!len) {
		return 0;
	}

	flash_stm32_sem_take(dev);

	rc = flash_stm32_block_erase_loop(dev, offset, len);

#if !defined(CONFIG_SOC_SERIES_STM32F1X)
	flash_stm32_flush_caches(dev);
#endif

	flash_stm32_sem_give(dev);

	return rc;
}

static int flash_stm32_write(struct device *dev, off_t offset,
			     const void *data, size_t len)
{
	int rc;

	if (!flash_stm32_valid_range(dev, offset, len, true)) {
		return -EINVAL;
	}

	if (!len) {
		return 0;
	}

	flash_stm32_sem_take(dev);

	rc = flash_stm32_write_range(dev, offset, data, len);

	flash_stm32_sem_give(dev);

	return rc;
}

static int flash_stm32_write_protection(struct device *dev, bool enable)
{
#if defined(CONFIG_SOC_SERIES_STM32F4X)
	struct stm32f4x_flash *regs = FLASH_STM32_REGS(dev);
#elif defined(CONFIG_SOC_SERIES_STM32F0X)
	struct stm32f0x_flash *regs = FLASH_STM32_REGS(dev);
#elif defined(CONFIG_SOC_SERIES_STM32F1X)
  struct stm32f10x_flash *regs = FLASH_STM32_REGS(dev);
#elif defined(CONFIG_SOC_SERIES_STM32L4X)
	struct stm32l4x_flash *regs = FLASH_STM32_REGS(dev);
#endif
	int rc = 0;

	flash_stm32_sem_take(dev);

	if (enable) {
		rc = flash_stm32_wait_flash_idle(dev);
		if (rc) {
			flash_stm32_sem_give(dev);
			return rc;
		}
//		regs->cr |= FLASH_CR_LOCK;
		HAL_FLASH_Lock();
	} else {
//		if (regs->cr & FLASH_CR_LOCK) {
//			regs->keyr = FLASH_KEY1;
//			regs->keyr = FLASH_KEY2;
//		}
	  HAL_FLASH_Unlock();
	}

	flash_stm32_sem_give(dev);

	return rc;
}

static struct flash_stm32_priv flash_data = {
#if defined(CONFIG_SOC_SERIES_STM32F0X)
	.regs = (struct stm32f0x_flash *) DT_FLASH_DEV_BASE_ADDRESS,
	.pclken = { .bus = STM32_CLOCK_BUS_AHB1,
		    .enr = LL_AHB1_GRP1_PERIPH_FLASH },
#elif defined(CONFIG_SOC_SERIES_STM32F1X)
  .regs = (struct stm32f10x_flash *)DT_FLASH_DEV_BASE_ADDRESS,
#elif defined(CONFIG_SOC_SERIES_STM32F4X)
	.regs = (struct stm32f4x_flash *) DT_FLASH_DEV_BASE_ADDRESS,
#elif defined(CONFIG_SOC_SERIES_STM32L4X)
	.regs = (struct stm32l4x_flash *) DT_FLASH_DEV_BASE_ADDRESS,
	.pclken = { .bus = STM32_CLOCK_BUS_AHB1,
		    .enr = LL_AHB1_GRP1_PERIPH_FLASH },
#endif
};

static const struct flash_driver_api flash_stm32_api = {
	.write_protection = flash_stm32_write_protection,
	.erase = flash_stm32_erase,
	.write = flash_stm32_write,
	.read = flash_stm32_read,
#ifdef CONFIG_FLASH_PAGE_LAYOUT
	.page_layout = flash_stm32_page_layout,
#endif
#ifdef FLASH_WRITE_BLOCK_SIZE
	.write_block_size = FLASH_WRITE_BLOCK_SIZE,
#else
#error Flash write block size not available
	/* Flash Write block size is extracted from device tree */
	/* as flash node property 'write-block-size' */
#endif
};

static int stm32_flash_init(struct device *dev)
{
	struct flash_stm32_priv *p = FLASH_STM32_PRIV(dev);
#if defined(CONFIG_SOC_SERIES_STM32L4X) || \
	defined(CONFIG_SOC_SERIES_STM32F0X)
	struct device *clk = device_get_binding(STM32_CLOCK_CONTROL_NAME);

	/*
	 * On STM32F0, Flash interface clock source is always HSI,
	 * so statically enable HSI here.
	 */
#if defined(CONFIG_SOC_SERIES_STM32F0X)
	LL_RCC_HSI_Enable();

	while (!LL_RCC_HSI_IsReady()) {
	}
#endif

	/* enable clock */
	clock_control_on(clk, (clock_control_subsys_t *)&p->pclken);
#endif

	p->sem = xSemaphoreCreateMutex();

	return flash_stm32_write_protection(dev, false);
}

static struct device_config flash_dev_config = {
    .init = stm32_flash_init,
};

struct device flash_dev = {
    .config = &flash_dev_config,
    .driver_api = &flash_stm32_api,
    .driver_data = &flash_data
};
