#ifndef __APP_ADV_BLE_H
#define __APP_ADV_BLE_H

#include "global.h"

#define APP_ADV_INTERVAL                1600                                        /**< The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS      0                                           /**< The advertising timeout (in units of seconds). */

#define NUS_SERVICE_UUID_TYPE           BLE_UUID_TYPE_BLE                           /**< UUID type for the Nordic UART Service (vendor specific). */

extern ble_uuid_t                       m_adv_uuids[];

void advertising_init(void);
void sleep_mode_enter(void);
void  ChangeAdvData(uint8_t addr, uint8_t dat);
#endif

