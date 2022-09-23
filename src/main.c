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

    return RT_EOK;
}
