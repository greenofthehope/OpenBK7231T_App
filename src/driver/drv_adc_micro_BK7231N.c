#include "driver.h"
#include "drv_adc.h"
#include "drv_sm16703p.h"
#include "logging/logging.h"

static float g_microSmooth = 0.0f;
static float g_gain = 2.0f;
static int g_noise = 40;
static int g_max = 900;

static int g_animBrightness = 0;
static int g_adcChannel = 0;

static float EMA_ALPHA = 0.2f;


// =====================================================
// PROCESS AUDIO LEVEL
// =====================================================
static void Micro_Process()
{
    int raw = ADC_Read(g_adcChannel);

    if(raw < g_noise)
        raw = 0;

    float level = raw * g_gain;
    if(level > g_max)
        level = g_max;

    g_microSmooth =
        g_microSmooth * (1.0f - EMA_ALPHA) +
        level * EMA_ALPHA;

    g_animBrightness = (int)(g_microSmooth * 255.0f / g_max);
}



// =====================================================
// APPLY TO LED STRIP
// =====================================================
static void Micro_UpdateLED()
{
    int starPos = (int)(g_microSmooth * 30.0f / g_max);

    for(int i = 0; i < 30; i++)
    {
        int brightness = 0;

        if(i == starPos)
            brightness = g_animBrightness;

        SM16703P_SetPixel(i, brightness, brightness, brightness);
    }

    SM16703P_Start();
}



// =====================================================
// DRIVER TICK
// =====================================================
static void Micro_Tick()
{
    Micro_Process();
    Micro_UpdateLED();
}



// =====================================================
// INIT DRIVER
// =====================================================
static void DRV_ADC_MICRO_Init()
{
    ADDLOG_INFO(LOG_FEATURE_DRIVER,
        "ADC Micro driver started");

    ADC_Init();

    addRepeatingEvent(0.02f, 0, Micro_Tick);
}



// =====================================================
// REGISTER DRIVER
// =====================================================
void DRV_ADC_MICRO_InitDriver()
{
    Driver_Register("ADC_MICRO", DRV_ADC_MICRO_Init);
}
