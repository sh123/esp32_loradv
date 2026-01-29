#ifndef DEFAULT_CONFIG_H
#define DEFAULT_CONFIG_H

#include <codec2.h>
#include <RadioLib.h>

#include "variant.h"

// saved settings version, increment to load new settings
#define CFG_VERSION                 10

// define in variant.h file if SX126X is in use
//#define USE_SX126X

// override in variant.h file, from https://github.com/jgromes/RadioLib/wiki/Modules
#ifndef MODULE_NAME
#define MODULE_NAME                 SX1268
#endif

#ifndef SERIAL_BAUD_RATE
#define SERIAL_BAUD_RATE            115200  // USB serial baud rate
#endif

// USB serial logging
// set to DebugLogLevel::LVL_TRACE for packet logging
// set to DebugLogLevel::LVL_NONE to disable logging
#ifndef CFG_LOG_LEVEL
#define CFG_LOG_LEVEL               DebugLogLevel::LVL_INFO
#endif

// modulation
#define CFG_MOD_TYPE_LORA           0   
#define CFG_MOD_TYPE_FSK            1
#ifndef CFG_MOD_TYPE
#define CFG_MOD_TYPE                CFG_MOD_TYPE_LORA         
#endif

// LoRa base pinouts
#ifndef CFG_LORA_PIN_NSS
#define CFG_LORA_PIN_NSS            5
#endif
#ifndef CFG_LORA_PIN_RST
#define CFG_LORA_PIN_RST            26
#endif
#ifndef CFG_LORA_PIN_DIO1
#define CFG_LORA_PIN_DIO1           12          // (sx127x - dio0, sx126x/sx128x - dio1)
#endif
#ifndef CFG_LORA_PIN_BUSY
#define CFG_LORA_PIN_BUSY           14          // (sx127x - dio1, sx126x/sx128x - busy)
#endif
#ifndef CFG_LORA_PIN_RXEN
#define CFG_LORA_PIN_RXEN           32          // (sx127x - unused, sx126x - RXEN pin number)
#endif
#ifndef CFG_LORA_PIN_TXEN
#define CFG_LORA_PIN_TXEN           33          // (sx127x - unused, sx126x - TXEN pin number)
#endif

// generic - frequencies, tune step, power
#ifndef CFG_LORA_FREQ_RX
#define CFG_LORA_FREQ_RX            433.775e6   // RX frequency in MHz
#endif
#ifndef CFG_LORA_FREQ_TX
#define CFG_LORA_FREQ_TX            433.775e6   // TX frequency in MHz
#endif
#ifndef CFG_LORA_FREQ_STEP
#define CFG_LORA_FREQ_STEP          25e3        // frquency step
#endif
#ifndef CFG_LORA_FREQ_MIN
#define CFG_LORA_FREQ_MIN           410e3       // minimum frequency
#endif
#ifndef CFG_LORA_FREQ_MAX
#define CFG_LORA_FREQ_MAX           493e3       // maximum frequency
#endif
#ifndef CFG_LORA_PWR
#define CFG_LORA_PWR                2           // output power in dBm (sx1268 with amplifier + 12 dbm)
#endif

// LoRa protocol default parameters (they need to match between devices!!!)
#ifndef CFG_LORA_BW
#define CFG_LORA_BW                 41.7e3      // discreate bandwidth (from 7.8 kHz up to 500 kHz)
#endif
#ifndef CFG_LORA_SF
#define CFG_LORA_SF                 7           // spreading factor (6 - 12), 6 requires implicit header mode
#endif
#ifndef CFG_LORA_CR
#define CFG_LORA_CR                 5           // coding rate (5 - 8)
#endif
#ifndef CFG_LORA_CRC
#define CFG_LORA_CRC                1           // 0 - disabled, 1 - 1 byte, 2 - 2 bytes
#endif
#ifndef CFG_LORA_SYNC
#define CFG_LORA_SYNC               0x12        // sync word (0x12 - private used by other trackers, 0x34 - public used by LoRaWAN)
#endif
#ifndef CFG_LORA_PREAMBLE_LEN
#define CFG_LORA_PREAMBLE_LEN       8           // preamble length from 6 to 65535
#endif

// fsk modem default parameters (they need to match between devices!!!)
#ifndef CFG_FSK_BIT_RATE
#define CFG_FSK_BIT_RATE            4.8         // bit rate in Kbps from 0.6 to 300.0
#endif
#ifndef CFG_FSK_FREQ_DEV
#define CFG_FSK_FREQ_DEV            1.2         // frequency deviation in kHz from 0.6 to 200.0
#endif
#ifndef CFG_FSK_RX_BW
#define CFG_FSK_RX_BW               9.7         // rx bandwidth in kHz discrete from 4.8 to 467.0
#endif
#ifndef CFG_FSK_SHAPING
#define CFG_FSK_SHAPING             RADIOLIB_SHAPING_NONE
#endif

// ptt button
#ifndef CFG_PTT_BTN_PIN
#define CFG_PTT_BTN_PIN             39          // pin for ptt button
#endif

// rotary encoder
#ifndef CFG_ENCODER_PIN_A
#define CFG_ENCODER_PIN_A           17
#endif
#ifndef CFG_ENCODER_PIN_B
#define CFG_ENCODER_PIN_B           16
#endif
#ifndef CFG_ENCODER_PIN_BTN
#define CFG_ENCODER_PIN_BTN         34
#endif
#ifndef CFG_ENCODER_PIN_VCC
#define CFG_ENCODER_PIN_VCC         -1
#endif
#ifndef CFG_ENCODER_STEPS
#define CFG_ENCODER_STEPS           4
#endif

// i2s speaker
#ifndef CFG_AUDIO_SPK_PIN_BCLK
#define CFG_AUDIO_SPK_PIN_BCLK      26
#endif
#ifndef CFG_AUDIO_SPK_PIN_LRC
#define CFG_AUDIO_SPK_PIN_LRC       13
#endif
#ifndef CFG_AUDIO_SPK_PIN_DIN
#define CFG_AUDIO_SPK_PIN_DIN       25
#endif

// i2s mic
#ifndef CFG_AUDIO_MIC_PIN_SD
#define CFG_AUDIO_MIC_PIN_SD        2
#endif
#ifndef CFG_AUDIO_MIC_PIN_WS
#define CFG_AUDIO_MIC_PIN_WS        15
#endif
#ifndef CFG_AUDIO_MIC_PIN_SCK
#define CFG_AUDIO_MIC_PIN_SCK       4
#endif

// battery monitor
#ifndef CFG_AUDIO_BATTERY_MON_PIN
#define CFG_AUDIO_BATTERY_MON_PIN   36
#endif
#ifndef CFG_AUDIO_BATTERY_MON_CAL
#define CFG_AUDIO_BATTERY_MON_CAL   0.25f
#endif

// power management
#ifndef CFG_PM_LSLEEP_AFTER_MS
#define CFG_PM_LSLEEP_AFTER_MS      60000       // how long to wait before going to sleep
#endif
#ifndef CFG_PM_LSLEEP_DURATION_MS
#define CFG_PM_LSLEEP_DURATION_MS   3000        // light sleep duration for polling
#endif
#ifndef CFG_PM_LSLEEP_AWAKE_MS
#define CFG_PM_LSLEEP_AWAKE_MS      100         // how long to be awake in light sleep polling
#endif
#ifndef CFG_PM_OPTIMIZE_SLEEP
#define CFG_PM_OPTIMIZE_SLEEP       false       // set to true if not using boost controller with auto shutdown
#endif

// audio
#define CFG_AUDIO_CODEC_CODEC2      0
#define CFG_AUDIO_CODEC_OPUS        1
#ifndef CFG_AUDIO_CODEC
#define CFG_AUDIO_CODEC             CFG_AUDIO_CODEC_CODEC2
#endif
#ifndef CFG_AUDIO_SAMPLE_RATE
#define CFG_AUDIO_SAMPLE_RATE       16000       // 8000 or 16000, i2s mic/spk have poor quality at 8000
#endif
#ifndef CFG_AUDIO_CODEC_SAMPLE_RATE
#define CFG_AUDIO_CODEC_SAMPLE_RATE 8000        // 8000
#endif
#ifndef CFG_AUDIO_RESAMPLE_COEFF
#define CFG_AUDIO_RESAMPLE_COEFF    2           // 1 or 2 only
#endif
#ifndef CFG_AUDIO_HPF_CUTOFF_HZ
#define CFG_AUDIO_HPF_CUTOFF_HZ     300         // high pass filter cutoff frequency
#endif
#ifndef CFG_AUDIO_CODEC2_MODE
#define CFG_AUDIO_CODEC2_MODE       CODEC2_MODE_1200
#endif
#ifndef CFG_AUDIO_MAX_PKT_SIZE
#define CFG_AUDIO_MAX_PKT_SIZE      48          // maximum super frame size
#endif
#ifndef CFG_AUDIO_MAX_VOL
#define CFG_AUDIO_MAX_VOL           30          // maximum volume
#endif
#ifndef CFG_AUDIO_VOL
#define CFG_AUDIO_VOL               10          // default volume
#endif

// audio, opus
#ifndef CFG_AUDIO_OPUS_BITRATE
#define CFG_AUDIO_OPUS_BITRATE      2400        // 2400 is the minimum
#endif
#ifndef CFG_AUDIO_OPUS_PCMLEN
#define CFG_AUDIO_OPUS_PCMLEN       20          // discrete one of 2.5, 5, 10, 20, 40, 60, 80, 100, 120
#endif

// audio, experimental 
#ifndef CFG_AUDIO_ENABLE_PRIVACY
#define CFG_AUDIO_ENABLE_PRIVACY    false
#endif

// keys must be randomly generated using true random generator and re-generated as often as possible
#ifndef CFG_AUDIO_PRIVACY_KEY 
#define CFG_AUDIO_PRIVACY_KEY \
byte AudioPrivacyKey[32] = {0xe7,0x5c,0xf0,0x43,0x80,0xec,0x45,0x93,0xe8,0x3b,0xfb,0x72,0x22,0x40,0x19,0x57,\
                            0x6c,0xde,0x05,0x00,0xff,0x88,0x12,0x42,0x20,0xf2,0x89,0x9d,0x7f,0x57,0xee,0xd6}
#endif

#endif // DEFAULT_CONFIG_H