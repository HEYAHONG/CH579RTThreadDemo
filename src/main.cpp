#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <RC.h>
#include <string.h>
#include <dfs_fs.h>
#include <rthw.h>
#include "flasharea.h"

int main(void)
{
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

    return RT_EOK;
}
