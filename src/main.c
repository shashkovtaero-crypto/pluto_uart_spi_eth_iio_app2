#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "xuartps.h"
#include "xil_printf.h"
#include "ad9361_ctrl.h"
#include "xgpiops.h"
#include "xil_cache.h"

static XGpioPs gpio;
static volatile int led_state = 0;
/* Mutex дл€ UART Ч xil_printf не thread-safe */
static SemaphoreHandle_t uart_mutex;

#define SAFE_PRINTF(...)  do { \
    if (xSemaphoreTake(uart_mutex, pdMS_TO_TICKS(100)) == pdTRUE) { \
        xil_printf(__VA_ARGS__); \
        xSemaphoreGive(uart_mutex); \
    } \
} while(0)

static void task_led_blink(void *param)
{
    (void)param;
    TickType_t last_wake = xTaskGetTickCount();

    for (;;) {
        led_state = 1;
        XGpioPs_WritePin(&gpio, GPIO_LED_PIN, 1);
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(500));

        led_state = 0;
        XGpioPs_WritePin(&gpio, GPIO_LED_PIN, 0);
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(500));
    }
}
static void task_led_status(void *param)
{
    (void)param;
    TickType_t last_wake = xTaskGetTickCount();
    uint32_t count = 0;

    for (;;) {
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(500));
        count++;
        SAFE_PRINTF("[LED] t=%4us  LED=%s\r\n",
                    (unsigned)(count / 2),
                    led_state ? "ON " : "OFF");
    }
}
static void ad9361_task(void *arg)
{
    int ret;
    (void)arg;

    ret = ad9361_device_setup();
    if (ret)
        xil_printf("AD9361 setup error: %d\r\n", ret);
    else
    	xil_printf("AD9361 setup sucess\r\n");

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int main(void)
{
	Xil_ICacheEnable();
	Xil_DCacheEnable();

	XUartPs uart;
	XUartPs_Config *ucfg = XUartPs_LookupConfig(XPAR_XUARTPS_0_DEVICE_ID);
	XUartPs_CfgInitialize(&uart, ucfg, ucfg->BaseAddress);
	XUartPs_SetBaudRate(&uart, 115200);

	xil_printf("\r\n========================================\r\n");
	xil_printf("  Pluto+ FreeRTOS v6.0\r\n");
	xil_printf("  3 tasks: LED blink, LED status, ADI poll\r\n");
	xil_printf("========================================\r\n");

	/* GPIO init for main context */
	XGpioPs_Config *gcfg = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
	XGpioPs_CfgInitialize(&gpio, gcfg, gcfg->BaseAddr);

    BaseType_t rc;

    xTaskCreate(task_led_blink,  "LED_Blink",  256, NULL, 2, NULL);
    xTaskCreate(task_led_status, "LED_Status", 512, NULL, 1, NULL);

    rc = xTaskCreate(ad9361_task,
                     "ad9361",
                     4096,
                     NULL,
                     tskIDLE_PRIORITY + 2,
                     NULL);

    if (rc != pdPASS) {
        xil_printf("xTaskCreate failed\r\n");
        for (;;)
            ;
    }

    vTaskStartScheduler();

    for (;;)
        ;
}
