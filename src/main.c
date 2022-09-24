#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <RC.h>

int main(void)
{
    {
        //¥Ú”°banner
        char *banner = (char *)RCGetHandle("banner");
        if (banner != NULL)
            printf("%s", banner);
    }

    {
        //¥Ú”° £”‡ƒ⁄¥Ê
        rt_size_t total = 0, used = 0;
        rt_memory_info(&total, &used, NULL);
        printf("\r\nTotal Memory:%d Bytes,Used Memory:%d Bytes\r\n", (int)total, (int)used);
    }

    return RT_EOK;
}
