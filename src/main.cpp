#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <RC.h>
#include <string.h>
#include <dfs_fs.h>
#include <rthw.h>
#include "flasharea.h"

#ifndef  OVERRIDE
#define OVERRIDE(x) $Sub$$##x
#endif

/*
硬件错误中断钩子
*/
#if defined(RT_USING_FINSH) && defined(MSH_USING_BUILT_IN_COMMANDS)
extern "C"
{
    extern long list_thread(void);
}
#endif

extern "C"
{
    rt_err_t OVERRIDE(rt_hw_hard_fault_exception)(void *context)
    {
#if defined(RT_USING_FINSH) && defined(MSH_USING_BUILT_IN_COMMANDS)
        list_thread();
#endif

#ifdef APP_AUTORESET_ON_ERROR
        printf("hardfault occurred,system will reboot ... \r\n\r\n");
        rt_hw_cpu_reset();
#endif

        return 0;
    }

}

#ifdef RT_DEBUG
/*
RT Thread断言
*/
static __attribute__((unused)) void assert_hook(const char *ex, const char *func, rt_size_t line)
{
#if defined(RT_USING_FINSH) && defined(MSH_USING_BUILT_IN_COMMANDS)
    list_thread();
#endif

#ifdef APP_AUTORESET_ON_ERROR
    printf("%s -> %s:%d\r\nassert occurred,system will reboot ... \r\n\r\n", ex, func, (int)line);
    rt_hw_cpu_reset();
#endif

}
#endif

#ifdef APP_USING_PAHOMQTT
#include "MQTT.h"
#endif

extern "C"
{
    extern uint8_t eth_mac_addr[];
}


int main(void)
{

    {
#ifdef RT_DEBUG
        rt_assert_set_hook(assert_hook);
#endif
    }

    {
        //打印banner
        char *banner = (char *)RCGetHandle("banner");
        if (banner != NULL)
            printf("%s", banner);
    }

    {
        //打印剩余内存
        rt_size_t total = 0, used = 0;
        rt_memory_info(&total, &used, NULL);
        printf("\r\nTotal Memory:%d Bytes,Used Memory:%d Bytes\r\n", (int)total, (int)used);
    }

    {
        //挂载定义的文件系统,在flash上划分一块区域作为根文件系统(由编译器分配，采用littlefs作为驱动)。
        printf("User Flash Start=%08X length=%u\r\n", (unsigned int)app_get_flasharea_addr(), app_get_flasharea_size());
        {
            if (app_flashdev_init() == RT_EOK)
            {
                printf("mount user flash ...\r\n");
                if (dfs_mount(APP_FLASHAREA_DEVNAME, "/", "lfs", 0, NULL) != 0)
                {
                    app_erase_flashdev();
                    dfs_mkfs("lfs", APP_FLASHAREA_DEVNAME);
                    printf("format flash finished,it will reboot...\r\n");
                    rt_thread_mdelay(1000);
                    rt_hw_cpu_reset();
                }
            }
        }
    }

    {
        //设置mac地址
        eth_mac_addr[5] = ((R32_ETH_MAADRL >> 0) & 0xFF);
        eth_mac_addr[4] = ((R32_ETH_MAADRL >> 8) & 0xFF);
        eth_mac_addr[3] = ((R32_ETH_MAADRL >> 16) & 0xFF);
        eth_mac_addr[2] = ((R32_ETH_MAADRL >> 24) & 0xFF);
        eth_mac_addr[1] = ((R16_ETH_MAADRH >> 0) & 0xFF);
        eth_mac_addr[0] = ((R16_ETH_MAADRH >> 8) & 0xFF);;


    }


#ifdef APP_USING_PAHOMQTT
    MQTT_Init();
#endif


    return RT_EOK;
}
