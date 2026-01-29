#ifndef VARIANT_H
#define VARIANT_H

// e22 module type
#define USE_SX126X
#define MODULE_NAME                 SX1268      // 433 MHz module

// LoRa pinougs
#define CFG_LORA_PIN_NSS            5
#define CFG_LORA_PIN_RST            26
#define CFG_LORA_PIN_DIO1           12          // (sx127x - dio0, sx126x/sx128x - dio1)
#define CFG_LORA_PIN_BUSY           14          // (sx127x - dio1, sx126x/sx128x - busy)
#define CFG_LORA_PIN_RXEN           32          // (sx127x - unused, sx126x - RXEN pin number)
#define CFG_LORA_PIN_TXEN           33          // (sx127x - unused, sx126x - TXEN pin number)

#endif // VARIANT_H