#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <RC.h>
#include <string.h>
#include "flasharea.h"


//define flash area
#define flash_block_size   512//erase block size
#define flash_block_number 128//totalsize=flash_block_size*flash_block_number
__attribute__((aligned(flash_block_size < 4096 ? 4096 : flash_block_size))) const uint8_t flash_area[flash_block_size * flash_block_number];

uint32_t app_get_flasharea_addr(void)
{
    return (uint32_t)flash_area;
}

uint32_t app_get_flasharea_size(void)
{
    return (uint32_t)sizeof(flash_area);
}


/*
register flash device
*/
#include <rtdevice.h>
#include <string.h>
#include <dfs_fs.h>
#include <CH57x_flash.h>

static rt_err_t _read_id(struct rt_mtd_nor_device *device)
{
    return RT_EOK;
}
static rt_size_t _read(struct rt_mtd_nor_device *device, rt_off_t offset, rt_uint8_t *data, rt_uint32_t length)
{
    rt_off_t dev_offset = offset + device->block_start * device->block_size;
    if (dev_offset + length > device->block_end * device->block_size)
    {
        return 0;
    }
    memcpy(data, (const void *)(unsigned int)dev_offset, length);
    return length;
}
static rt_size_t _write(struct rt_mtd_nor_device *device, rt_off_t offset, const rt_uint8_t *data, rt_uint32_t length)
{
    rt_off_t dev_offset = offset + device->block_start * device->block_size;
    if (dev_offset + length > device->block_end * device->block_size)
    {
        return 0;
    }
    if (dev_offset % 4 != 0 || ((unsigned int)data) % 4 != 0)
    {
        return 0;
    }
    FlashWriteBuf((uint32_t)dev_offset, (PUINT32)data, (uint16_t)length);
    return length;
}
static rt_err_t _erase_block(struct rt_mtd_nor_device *device, rt_off_t offset, rt_uint32_t length)
{
    rt_off_t dev_offset = offset + device->block_start * device->block_size;
    if (dev_offset + length > device->block_end * device->block_size)
    {
        return RT_ERROR;
    }
    {
        const size_t erase_size = 512;
        for (unsigned int addr = dev_offset; addr < dev_offset + length; addr += erase_size)
        {
            FlashBlockErase(addr);
        }
    }
    return RT_EOK;
}
static struct rt_mtd_nor_driver_ops flashops =
{
    _read_id,
    _read,
    _write,
    _erase_block
};
int app_flashdev_init(void)
{
    struct rt_mtd_nor_device *flash = (struct rt_mtd_nor_device *)rt_malloc(sizeof(struct rt_mtd_nor_device));
    memset(flash, 0, sizeof(struct rt_mtd_nor_device));

    flash->block_size = flash_block_size;
    flash->block_start = ((size_t)flash_area) / flash->block_size;
    flash->block_end = flash->block_start + flash_block_number;
    flash->ops = &flashops;
    if (RT_EOK != rt_mtd_nor_register_device(APP_FLASHAREA_DEVNAME, flash))
    {
        rt_free(flash);
        return RT_ERROR;
    }
    return RT_EOK;
}

void app_erase_flashdev(void)
{
    {
        const size_t erase_size = 512;
        for (unsigned int addr = (unsigned int)app_get_flasharea_addr(); addr < app_get_flasharea_addr() + app_get_flasharea_size(); addr += erase_size)
        {
            FlashBlockErase(addr);
        }
    }
}
