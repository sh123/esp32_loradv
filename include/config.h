#ifndef CONFIG_H
#define CONFIG_H

#include <codec2.h>
#include <RadioLib.h>

// saved settings version, increment to load new settings
#define CFG_VERSION  2

// Comment out for SX127X module usage
//#define USE_SX126X

// Check your module name at https://github.com/jgromes/RadioLib/wiki/Modules
#ifdef USE_SX126X
#define MODULE_NAME   SX1268
#else
#define MODULE_NAME   SX1278
#endif

#define SERIAL_BAUD_RATE            115200  // USB serial baud rate

// USB serial logging
// set to DebugLogLevel::LVL_TRACE for packet logging
// set to DebugLogLevel::LVL_NONE to disable logging
#define CFG_LOG_LEVEL               DebugLogLevel::LVL_INFO

// change pinouts if not defined through native board LORA_* definitions
#ifndef LORA_RST
#pragma message("LoRa pin definitions are not found, redefining...")
#define LORA_RST                    26
#define LORA_IRQ                    12
#endif

// LoRa pinouts
#define CFG_LORA_PIN_SS             SS
#define CFG_LORA_PIN_RST            LORA_RST
#define CFG_LORA_PIN_A              LORA_IRQ    // (sx127x - dio0, sx126x/sx128x - dio1)
#ifdef USE_SX126X
#define CFG_LORA_PIN_B              14          // (sx127x - dio1, sx126x/sx128x - busy)
#define CFG_LORA_PIN_RXEN           32          // (sx127x - unused, sx126x - RXEN pin number)
#define CFG_LORA_PIN_TXEN           33          // (sx127x - unused, sx126x - TXEN pin number)
#else
#define CFG_LORA_PIN_B              RADIOLIB_NC // (sx127x - dio1, sx126x/sx128x - busy)
#define CFG_LORA_PIN_RXEN           RADIOLIB_NC // (sx127x - unused, sx126x - RXEN pin number)
#define CFG_LORA_PIN_TXEN           RADIOLIB_NC // (sx127x - unused, sx126x - TXEN pin number)
#endif

// LoRa protocol default parameters (they need to match between devices!!!)
#define CFG_LORA_FREQ_RX            433.775e6   // RX frequency in MHz
#define CFG_LORA_FREQ_TX            433.775e6   // TX frequency in MHz
#define CFG_LORA_FREQ_STEP          5e3         // frquency step
#define CFG_LORA_BW                 125e3       // bandwidth (from 7.8 kHz up to 500 kHz)
#define CFG_LORA_SF                 8           // spreading factor (6 - 12), 6 requires implicit header mode
#define CFG_LORA_CR                 7           // coding rate (5 - 8)
#define CFG_LORA_CRC                1           // 0 - disabled, 1 - 1 byte, 2 - 2 bytes
#define CFG_LORA_SYNC               0x12        // sync word (0x12 - private used by other trackers, 0x34 - public used by LoRaWAN)
#define CFG_LORA_PWR                20          // output power in dBm (sx1268 with amplifier + 12 dbm)
#define CFG_LORA_PREAMBLE_LEN       8           // preamble length from 6 to 65535

// ptt button
#define CFG_PTT_BTN_PIN             39          // pin for ptt button

// rotary encoder
#define CFG_ENCODER_PIN_A           17
#define CFG_ENCODER_PIN_B           16
#define CFG_ENCODER_PIN_BTN         34
#define CFG_ENCODER_PIN_VCC         -1
#define CFG_ENCODER_STEPS           4

// audio
#define CFG_AUDIO_CODEC2_MODE       CODEC2_MODE_700C
#define CFG_AUDIO_MAX_PKT_SIZE      48
#define CFG_AUDIO_MAX_VOL           500
#define CFG_AUDIO_VOL               50

// i2s speaker
#define CFG_AUDIO_SPK_PIN_BCLK      26
#define CFG_AUDIO_SPK_PIN_LRC       13
#define CFG_AUDIO_SPK_PIN_DIN       25

// i2s mic
#define CFG_AUDIO_MIC_PIN_SD        2
#define CFG_AUDIO_MIC_PIN_WS        15
#define CFG_AUDIO_MIC_PIN_SCK       4

// battery monitor
#define CFG_AUDIO_BATTERY_MON_PIN   36
#define CFG_AUDIO_BATTERY_MON_CAL   0.25f

// power management
#define CFG_PM_LSLEEP_AFTER_MS      60000
#define CFG_PM_LSLEEP_DURATION_MS   3000
#define CFG_PM_LSLEEP_AWAKE_MS      100

#endif // CONFIG_H