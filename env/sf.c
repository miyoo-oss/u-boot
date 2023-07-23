// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2000-2010
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2001 Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Andreas Heppel <aheppel@sysgo.de>
 *
 * (C) Copyright 2008 Atmel Corporation
 */
#include <common.h>
#include <dm.h>
#include <env.h>
#include <env_internal.h>
#include <malloc.h>
#include <spi.h>
#include <spi_flash.h>
#include <search.h>
#include <errno.h>
#include <dm/device-internal.h>
#include <u-boot/crc.h>

#ifdef CONFIG_MSTAR_ENV_OFFSET
#include <asm/arch/mach/platform.h>
#include <image.h>
#endif

#ifndef CONFIG_SPL_BUILD
#define CMD_SAVEENV
#define INITENV
#endif

#ifdef CONFIG_MS_SAVE_ENV_IN_ISP_FLASH
int ms_nor_env_offset = 0;
#endif

#ifdef CONFIG_ENV_OFFSET_REDUND
#ifdef CMD_SAVEENV
static ulong env_offset		= CONFIG_ENV_OFFSET;
static ulong env_new_offset	= CONFIG_ENV_OFFSET_REDUND;
#endif

#endif /* CONFIG_ENV_OFFSET_REDUND */

DECLARE_GLOBAL_DATA_PTR;

static struct spi_flash *env_flash;


#ifdef CONFIG_MS_PARTITION
#include "../drivers/mstar/partition/part_mxp.h"
extern int mxp_init_nor_flash(void);

int mxp_get_env(ulong* offset, int* size)
{
    char strENVName[] = "UBOOT_ENV";
	int idx;
    *offset = CONFIG_ENV_OFFSET;
    *size = CONFIG_ENV_SIZE;
    int ret=0;

    if((ret=mxp_init_nor_flash())<0)
    {
        return -1;
    }

    mxp_load_table();
    idx=mxp_get_record_index(strENVName);
    if(idx>=0)
    {
        mxp_record rec;
        if(0==mxp_get_record_by_index(idx,&rec))
        {
            *offset = rec.start;
            *size = rec.size;
        }
        else
        {
            printf("failed to get MXP record with name: %s\n", strENVName);
            return -1;
        }


    }
    else
    {
        printf("can not found mxp record: %s\n", strENVName);
        return -1;
    }


	printf("env_offset=0x%X env_size=0x%X\n", *offset, *size);

	return 0;

}
#elif defined(CONFIG_MSTAR_ENV_OFFSET)
static image_header_t *_get_image_header(void)
{
    return (image_header_t *)(MS_SPI_ADDR+MS_SPI_BOOT_ROM_SIZE);
}

int ms_get_spi_env_offset(void)
{
    int spi_env_offset= MS_SPI_UBOOT_SIZE+MS_SPI_BOOT_ROM_SIZE;

    image_header_t *hdr=_get_image_header();
    if(image_check_magic(hdr))
    {
        spi_env_offset=(((image_get_image_size(hdr) -1 ) / CONFIG_ENV_SECT_SIZE )+1)*CONFIG_ENV_SECT_SIZE + MS_SPI_BOOT_ROM_SIZE;
    }
    else
    {
        printf("Not a img type UBOOT!! Using default spi_env_offset !!\n");
    }

    printf("spi_env_offset=0x%08X\n",spi_env_offset);

    return spi_env_offset;
}
#endif


static int setup_flash_device(void)
{
#ifdef CONFIG_DM_SPI_FLASH
	struct udevice *new;
	int	ret;

	/* speed and mode will be read from DT */
	ret = spi_flash_probe_bus_cs(CONFIG_ENV_SPI_BUS, CONFIG_ENV_SPI_CS,
				     CONFIG_ENV_SPI_MAX_HZ, CONFIG_ENV_SPI_MODE,
				     &new);
	if (ret) {
		env_set_default("spi_flash_probe_bus_cs() failed", 0);
		return ret;
	}

	env_flash = dev_get_uclass_priv(new);
#else

#ifdef CONFIG_MS_PARTITION
    int mxp_env_size = 0;
    mxp_get_env(&env_offset, &mxp_env_size);
    if(mxp_env_size != CONFIG_ENV_SIZE)
    {
        puts("mxp env size error\n");
        return 1;
    }
#elif define(CONFIG_MSTAR_ENV_OFFSET)
    env_offset = ms_get_spi_env_offset();
#endif

	if (!env_flash) {
		env_flash = spi_flash_probe(CONFIG_ENV_SPI_BUS,
			CONFIG_ENV_SPI_CS,
			CONFIG_ENV_SPI_MAX_HZ, CONFIG_ENV_SPI_MODE);
		if (!env_flash) {
			env_set_default("spi_flash_probe() failed", 0);
			return -EIO;
		}
	}
#endif
	return 0;
}

#if defined(CONFIG_ENV_OFFSET_REDUND)
#ifdef CMD_SAVEENV
static int env_sf_save(void)
{
	env_t	env_new;
	char	*saved_buffer = NULL, flag = ENV_REDUND_OBSOLETE;
	u32	saved_size, saved_offset, sector;
	int	ret;

	ret = setup_flash_device();
	if (ret)
		return ret;

	ret = env_export(&env_new);
	if (ret)
		return -EIO;
	//env_new.flags	= ENV_REDUND_ACTIVE;

#ifdef CONFIG_MS_PARTITION
    int mxp_env_size = 0;
    mxp_get_env(&env_offset, &mxp_env_size);
    if(mxp_env_size != CONFIG_ENV_SIZE * 2)
    {
        puts("mxp env size error\n");
        return 1;
    }
#elif define(CONFIG_MSTAR_ENV_OFFSET)
    env_offset = ms_get_spi_env_offset();
#endif

	if (gd->env_valid == ENV_VALID) {
		env_new_offset = CONFIG_ENV_OFFSET_REDUND;
		//env_offset = CONFIG_ENV_OFFSET;
	} else {
		env_new_offset = CONFIG_ENV_OFFSET;
		//env_offset = CONFIG_ENV_OFFSET_REDUND;
	}

	/* Is the sector larger than the env (i.e. embedded) */
	if (CONFIG_ENV_SECT_SIZE > CONFIG_ENV_SIZE) {
		saved_size = CONFIG_ENV_SECT_SIZE - CONFIG_ENV_SIZE;
		saved_offset = env_new_offset + CONFIG_ENV_SIZE;
		saved_buffer = memalign(ARCH_DMA_MINALIGN, saved_size);
		if (!saved_buffer) {
			ret = -ENOMEM;
			goto done;
		}
		ret = spi_flash_read(env_flash, saved_offset,
					saved_size, saved_buffer);
		if (ret)
			goto done;
	}

	sector = DIV_ROUND_UP(CONFIG_ENV_SIZE, CONFIG_ENV_SECT_SIZE);

	puts("Erasing SPI flash...");
	ret = spi_flash_erase(env_flash, env_new_offset,
				sector * CONFIG_ENV_SECT_SIZE);
	if (ret)
		goto done;

	puts("Writing to SPI flash...");

	ret = spi_flash_write(env_flash, env_new_offset,
		CONFIG_ENV_SIZE, &env_new);
	if (ret)
		goto done;

	if (CONFIG_ENV_SECT_SIZE > CONFIG_ENV_SIZE) {
		ret = spi_flash_write(env_flash, saved_offset,
					saved_size, saved_buffer);
		if (ret)
			goto done;
	}
/*
	ret = spi_flash_write(env_flash, env_offset + offsetof(env_t, flags),
				sizeof(env_new.flags), &flag);
	if (ret)
		goto done;
*/
	puts("done\n");

	gd->env_valid = gd->env_valid == ENV_REDUND ? ENV_VALID : ENV_REDUND;

	printf("Valid environment: %d\n", (int)gd->env_valid);

 done:
	if (saved_buffer)
		free(saved_buffer);

	return ret;
}
#endif /* CMD_SAVEENV */

static int env_sf_load(void)
{
	int ret;
	int read1_fail, read2_fail;
	env_t *tmp_env1, *tmp_env2;

#ifdef CONFIG_MS_PARTITION
    int mxp_env_size = 0;
    mxp_get_env(&env_offset, &mxp_env_size);
    if(mxp_env_size != CONFIG_ENV_SIZE * 2)
    {
        env_set_default("mxp env size error", 0);
        return -EIO;
    }
#elif define(CONFIG_MSTAR_ENV_OFFSET)
    env_offset = ms_get_spi_env_offset();
#endif

	tmp_env1 = (env_t *)memalign(ARCH_DMA_MINALIGN,
			CONFIG_ENV_SIZE);
	tmp_env2 = (env_t *)memalign(ARCH_DMA_MINALIGN,
			CONFIG_ENV_SIZE);

	if (!tmp_env1 || !tmp_env2) {
		env_set_default("malloc() failed", 0);
		ret = -EIO;
		goto out;
	}

	ret = setup_flash_device();
	if (ret)
		goto out;

	read1_fail = spi_flash_read(env_flash, env_offset,
				    CONFIG_ENV_SIZE, tmp_env1);
	read2_fail = spi_flash_read(env_flash, CONFIG_ENV_OFFSET_REDUND,
				    CONFIG_ENV_SIZE, tmp_env2);

	ret = env_import_redund((char *)tmp_env1, read1_fail, (char *)tmp_env2,
				read2_fail);

	spi_flash_free(env_flash);
	env_flash = NULL;
out:
	free(tmp_env1);
	free(tmp_env2);

	return ret;
}

#else
#ifdef CMD_SAVEENV
static int env_sf_save(void)
{
	u32	saved_size, saved_offset, sector;
	char	*saved_buffer = NULL;
	int	ret = 1;
	env_t	env_new;

	ret = setup_flash_device();
	if (ret)
		return ret;

	/* Is the sector larger than the env (i.e. embedded) */
	if (CONFIG_ENV_SECT_SIZE > CONFIG_ENV_SIZE) {
		saved_size = CONFIG_ENV_SECT_SIZE - CONFIG_ENV_SIZE;
		saved_offset = CONFIG_ENV_OFFSET + CONFIG_ENV_SIZE;
		saved_buffer = malloc(saved_size);
		if (!saved_buffer)
			goto done;

		ret = spi_flash_read(env_flash, saved_offset,
			saved_size, saved_buffer);
		if (ret)
			goto done;
	}

	ret = env_export(&env_new);
	if (ret)
		goto done;

	sector = DIV_ROUND_UP(CONFIG_ENV_SIZE, CONFIG_ENV_SECT_SIZE);

	puts("Erasing SPI flash...");
	ret = spi_flash_erase(env_flash, CONFIG_ENV_OFFSET,
		sector * CONFIG_ENV_SECT_SIZE);
	if (ret)
		goto done;

	puts("Writing to SPI flash...");
	ret = spi_flash_write(env_flash, CONFIG_ENV_OFFSET,
		CONFIG_ENV_SIZE, &env_new);
	if (ret)
		goto done;

	if (CONFIG_ENV_SECT_SIZE > CONFIG_ENV_SIZE) {
		ret = spi_flash_write(env_flash, saved_offset,
			saved_size, saved_buffer);
		if (ret)
			goto done;
	}

	ret = 0;
	puts("done\n");

 done:
	if (saved_buffer)
		free(saved_buffer);

	return ret;
}
#endif /* CMD_SAVEENV */

static int env_sf_load(void)
{
	int ret;
	char *buf = NULL;

#ifdef CONFIG_MS_PARTITION
    int mxp_env_size = 0;
    mxp_get_env(&CONFIG_ENV_OFFSET, &mxp_env_size);
    if(mxp_env_size != CONFIG_ENV_SIZE)
    {
        env_set_default("mxp env size error", 0);
        return;
    }
#elif define(CONFIG_MSTAR_ENV_OFFSET)
    CONFIG_ENV_OFFSET= ms_get_spi_env_offset();
#endif

	buf = (char *)memalign(ARCH_DMA_MINALIGN, CONFIG_ENV_SIZE);
	if (!buf) {
		env_set_default("malloc() failed", 0);
		return -EIO;
	}

	ret = setup_flash_device();
	if (ret)
		goto out;

	ret = spi_flash_read(env_flash,
		CONFIG_ENV_OFFSET, CONFIG_ENV_SIZE, buf);
	if (ret) {
		env_set_default("spi_flash_read() failed", 0);
		goto err_read;
	}

	ret = env_import(buf, 1);
	if (!ret)
		gd->env_valid = ENV_VALID;

err_read:
	spi_flash_free(env_flash);
	env_flash = NULL;
out:
	free(buf);

	return ret;
}
#endif

#if CONFIG_ENV_ADDR != 0x0
__weak void *env_sf_get_env_addr(void)
{
	return (void *)CONFIG_ENV_ADDR;
}
#endif

#if defined(INITENV) && (CONFIG_ENV_ADDR != 0x0)
static int env_sf_init(void)
{
	env_t *env_ptr = (env_t *)env_sf_get_env_addr();

	if (crc32(0, env_ptr->data, ENV_SIZE) == env_ptr->crc) {
		gd->env_addr	= (ulong)&(env_ptr->data);
		gd->env_valid	= 1;
	} else {
		gd->env_addr = (ulong)&default_environment[0];
		gd->env_valid = 1;
	}

	return 0;
}
#endif

U_BOOT_ENV_LOCATION(sf) = {
	.location	= ENVL_SPI_FLASH,
	ENV_NAME("SPI Flash")
	.load		= env_sf_load,
#ifdef CMD_SAVEENV
	.save		= env_save_ptr(env_sf_save),
#endif
#if defined(INITENV) && (CONFIG_ENV_ADDR != 0x0)
	.init		= env_sf_init,
#endif
};
