#ifndef VARIANT_H
#define VARIANT_H

// RA01 module name from RadiioLib
#define MODULE_NAME                 SX1278      // 433 MHz module

// LoRa module specific pinouts
#define CFG_LORA_PIN_NSS            5
#define CFG_LORA_PIN_RST            26
#define CFG_LORA_PIN_DIO1           12          // (sx127x - dio0, sx126x/sx128x - dio1)
#define CFG_LORA_PIN_BUSY           RADIOLIB_NC // (sx127x - dio1, sx126x/sx128x - busy)
#define CFG_LORA_PIN_RXEN           RADIOLIB_NC // (sx127x - unused, sx126x - RXEN pin number)
#define CFG_LORA_PIN_TXEN           RADIOLIB_NC // (sx127x - unused, sx126x - TXEN pin number)

#endif // VARIANT_H