#ifndef CONFIG_H
#define CONFIG_H

#include <codec2.h>
#include <RadioLib.h>

// saved settings version, increment to load new settings
#define CFG_VERSION                 10

// Comment out for SX127X module usage, passed from platform.io
//#define USE_SX126X

// Check your module name at https://github.com/jgromes/RadioLib/wiki/Modules
#ifdef USE_SX126X
#define MODULE_NAME                 SX1268
#else
#define MODULE_NAME                 SX1278
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

// modulation
#define CFG_MOD_TYPE_LORA           0   
#define CFG_MOD_TYPE_FSK            1
#define CFG_MOD_TYPE                CFG_MOD_TYPE_LORA         

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

// generic - frequencies, tune step, power
#define CFG_LORA_FREQ_RX            433.775e6   // RX frequency in MHz
#define CFG_LORA_FREQ_TX            433.775e6   // TX frequency in MHz
#define CFG_LORA_FREQ_STEP          25e3        // frquency step
#define CFG_LORA_PWR                2           // output power in dBm (sx1268 with amplifier + 12 dbm)

// LoRa protocol default parameters (they need to match between devices!!!)
#define CFG_LORA_BW                 31.25e3     // discreate bandwidth (from 7.8 kHz up to 500 kHz)
#define CFG_LORA_SF                 7           // spreading factor (6 - 12), 6 requires implicit header mode
#define CFG_LORA_CR                 5           // coding rate (5 - 8)
#define CFG_LORA_CRC                1           // 0 - disabled, 1 - 1 byte, 2 - 2 bytes
#define CFG_LORA_SYNC               0x12        // sync word (0x12 - private used by other trackers, 0x34 - public used by LoRaWAN)
#define CFG_LORA_PREAMBLE_LEN       8           // preamble length from 6 to 65535

// fsk modem default parameters (they need to match between devices!!!)
#define CFG_FSK_BIT_RATE            4.8         // bit rate in Kbps from 0.6 to 300.0
#define CFG_FSK_FREQ_DEV            1.2         // frequency deviation in kHz from 0.6 to 200.0
#define CFG_FSK_RX_BW               9.7         // rx bandwidth in kHz discrete from 4.8 to 467.0
#define CFG_FSK_SHAPING             RADIOLIB_SHAPING_NONE

// ptt button
#define CFG_PTT_BTN_PIN             39          // pin for ptt button

// rotary encoder
#define CFG_ENCODER_PIN_A           17
#define CFG_ENCODER_PIN_B           16
#define CFG_ENCODER_PIN_BTN         34
#define CFG_ENCODER_PIN_VCC         -1
#define CFG_ENCODER_STEPS           4

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
#define CFG_PM_LSLEEP_AFTER_MS      60000       // how long to wait before going to sleep
#define CFG_PM_LSLEEP_DURATION_MS   3000        // light sleep duration for polling
#define CFG_PM_LSLEEP_AWAKE_MS      100         // how long to be awake in light sleep polling

// audio
#define CFG_AUDIO_CODEC_CODEC2      0
#define CFG_AUDIO_CODEC_OPUS        1
#define CFG_AUDIO_CODEC             CFG_AUDIO_CODEC_CODEC2
#define CFG_AUDIO_SAMPLE_RATE       8000
#define CFG_AUDIO_CODEC2_MODE       CODEC2_MODE_1600
#define CFG_AUDIO_MAX_PKT_SIZE      48          // maximum super frame size
#define CFG_AUDIO_MAX_VOL           500         // maximum volume
#define CFG_AUDIO_VOL               300         // default volume

// audio, opus
#define CFG_AUDIO_OPUS_BITRATE      3200
#define CFG_AUDIO_OPUS_PCMLEN       120

// audio, experimental
#define CFG_AUDIO_ENABLE_PRIVACY    false
#define CFG_AUDIO_PRIVACY_KEY \
byte AudioPrivacyKey[32] = {0xe7,0x5c,0xf0,0x43,0x80,0xec,0x45,0x93,0xe8,0x3b,0xfb,0x72,0x22,0x40,0x19,0x57,\
                            0x6c,0xde,0x05,0x00,0xff,0x88,0x12,0x42,0x20,0xf2,0x89,0x9d,0x7f,0x57,0xee,0xd6}

#endif // CONFIG_H