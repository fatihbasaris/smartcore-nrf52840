#include "battery.h"

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/sys/printk.h>

#define ADC_NODE        DT_NODELABEL(adc)
#define ADC_CHANNEL_ID  3
#define ADC_RESOLUTION  12
#define ADC_GAIN        ADC_GAIN_1_6
#define ADC_REFERENCE   ADC_REF_INTERNAL
#define ADC_ACQ_TIME    ADC_ACQ_TIME_DEFAULT

#define BATTERY_DIVIDER_MULTIPLIER 11

static const struct device *adc_dev = DEVICE_DT_GET(ADC_NODE);
static int16_t sample_buffer;

static struct adc_channel_cfg channel_cfg = {
    .gain = ADC_GAIN,
    .reference = ADC_REFERENCE,
    .acquisition_time = ADC_ACQ_TIME,
    .channel_id = ADC_CHANNEL_ID,
#if defined(CONFIG_ADC_CONFIGURABLE_INPUTS)
    .input_positive = SAADC_CH_PSELP_PSELP_AnalogInput3, // P0.05 = AIN3
#endif
};

int battery_init(void)
{
    if (!device_is_ready(adc_dev)) {
        printk("ADC device not ready\n");
        return -ENODEV;
    }

    int err = adc_channel_setup(adc_dev, &channel_cfg);
    if (err) {
        printk("ADC channel setup failed: %d\n", err);
        return err;
    }

    return 0;
}

int battery_read_mv(int32_t *battery_mv)
{
    int err;
    int32_t total_mv = 0;

    const struct adc_sequence sequence = {
        .channels = BIT(ADC_CHANNEL_ID),
        .buffer = &sample_buffer,
        .buffer_size = sizeof(sample_buffer),
        .resolution = ADC_RESOLUTION,
    };

    for (int i = 0; i < 32; i++) {
        err = adc_read(adc_dev, &sequence);
        if (err) {
            return err;
        }

        int32_t adc_mv = sample_buffer;

        err = adc_raw_to_millivolts(
            adc_ref_internal(adc_dev),
            ADC_GAIN,
            ADC_RESOLUTION,
            &adc_mv
        );

        if (err) {
            return err;
        }

        total_mv += adc_mv;
        k_sleep(K_MSEC(5));
    }

    int32_t avg_adc_mv = total_mv / 32;

    *battery_mv =
        (((avg_adc_mv * BATTERY_DIVIDER_MULTIPLIER) + 5) / 10) * 10;

    return 0;
}

uint8_t battery_percent_from_mv(int32_t mv)
{
    if (mv >= 4200) return 100;
    if (mv >= 4100) return 90;
    if (mv >= 4000) return 80;
    if (mv >= 3900) return 70;
    if (mv >= 3800) return 60;
    if (mv >= 3700) return 50;
    if (mv >= 3600) return 30;
    if (mv >= 3500) return 20;
    if (mv >= 3400) return 10;
    if (mv >= 3300) return 5;

    return 0;
}