#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

int main(void)
{
    int counter = 0;

    while (1)
    {
        printk("Counter = %d\r\n", counter++);
        k_sleep(K_SECONDS(1));
    }

    return 0;
}