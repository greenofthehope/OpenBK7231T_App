#include "driver/drv_public.h"
#include "new_pins.h"
#include "logging/logging.h"
#include "leds/leds.h"
#include "driver/drv_adc.h"
#include "driver/drv_pixelAnim.h"

#define MICRO_PIN 23

static float micSmooth = 0;

static float gain = 2.0f;
static int noiseFloor = 40;
static int maxLevel = 900;

static int speedMin = 0;
static int speedMax = 10;

static int DRV_ADC_MICRO_IsEnabled()
{
    return 1;
}

static void DRV_ADC_MICRO_Init()
{
    ADDLOG_INFO(LOG_FEATURE_LED, "ADC Micro init");

    PIN_SetPinRoleForPinIndex(MICRO_PIN, IOR_ADC);

    ADC_Init();
}

static void DRV_ADC_MICRO_RunFrame()
{
    int raw = ADC_Read(0);

    if(raw < noiseFloor)
        raw = 0;

    float level = raw * gain;
    if(level > maxLevel)
        level = maxLevel;

    micSmooth = micSmooth * 0.8f + level * 0.2f;

    float norm = micSmooth / (float)maxLevel;

    int speed = speedMin + (speedMax - speedMin) * norm;

    if(speed < speedMin) speed = speedMin;
    if(speed > speedMax) speed = speedMax;

    PixelAnim_SetSpeed(speed);
}

static void DRV_ADC_MICRO_Shutdown() {}

DRV_DEF(DRV_ADC_MICRO)
{
    .name = "adcMicro",
    .init = DRV_ADC_MICRO_Init,
    .runFrame = DRV_ADC_MICRO_RunFrame,
    .shutdown = DRV_ADC_MICRO_Shutdown,
    .isEnabled = DRV_ADC_MICRO_IsEnabled
};
