#include "app_adv_ble.h"
#include "ble_advdata.h"
#include "ble_advertising.h"

ble_uuid_t                       m_adv_uuids[] = {{BLE_UUID_NUS_SERVICE, NUS_SERVICE_UUID_TYPE}};  /**< Universally unique service identifier. */
static uint8_t                   manuf_data[18] = {0xf6,0xcb,0x9a,0x3b,0x01,0x01,0x01,0x08,0x00,0x00,0xa5,0x02};			// Manufacturer Data

/**
 * 功能：修改、更新广播数据。
 * bit28:0、1:门的开关状态;2:有无管理员;
 */
void  ChangeAdvData(uint8_t addr, uint8_t dat)
{
    if (addr == 0) // 更新广播内容到系统
    {
//				UserStartAdv(1, 0);
    }
    else if (addr == 1) // 更新广播数据————开关状态
    {    
        if (dat) manuf_data[8] |= 0x01; // 1:开
        else manuf_data[8] &= 0xfe; // 0:关
//				advertising_init();
    }
    else if (addr == 2) // 更新广播数据————有无管理员
    {
        if (dat == 0xA5)
        {
						manuf_data[8] &= 0xfb; // 0:有
        }
        else
        {
						manuf_data[8] |= 0x04; // 1:无
        }
//				advertising_init();
    }
    else if (addr == 3) // 更新广播数据————主动请求连接
    {
        if (dat)
        {
						manuf_data[10] |= 0x01;
        }
        else
        {
						manuf_data[10] &= 0xFE;
        }
//				advertising_init();
    }
    else if (addr == 4) // 
    {
				manuf_data[9] = dat;//电池电量
//				advertising_init();
    }
    else if (addr == 5) // 未读操作日志标志
    {
        if (dat)
        {
						manuf_data[8] |= 0x08; // 有
        }
        else
        {
						manuf_data[8] &= 0xF7; // 无
        }
//				advertising_init();
    }
}

/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
void sleep_mode_enter(void)
{
    uint32_t err_code = bsp_indication_set(BSP_INDICATE_IDLE);
    APP_ERROR_CHECK(err_code);

    // Prepare wakeup buttons.
//    err_code = bsp_btn_ble_sleep_mode_prepare();
//    APP_ERROR_CHECK(err_code);

    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    uint32_t err_code;

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
            APP_ERROR_CHECK(err_code);
            break;
        case BLE_ADV_EVT_IDLE:
            sleep_mode_enter();
            break;
        default:
            break;
    }
}

static void adv_manuf_data_get(uint8_t *src)
{
    uint32_t err_code = 1;
    ble_gap_addr_t device_addr;
    uint8_t MAC_ADDR[6];
	
    // Get BLE address.
#if (NRF_SD_BLE_API_VERSION == 3)
    err_code = sd_ble_gap_addr_get(&device_addr);

#else
    err_code = sd_ble_gap_address_get(&device_addr);
#endif
#if SEGGER_RTT_DEBUG_ADV
		SEGGER_RTT_printf(0, "get mac addr err_code = 0x%02X \r\n",err_code);
#endif
		device_addr.addr[BLE_GAP_ADDR_LEN-1] += 1;
		sd_ble_gap_address_set(BLE_GAP_ADDR_CYCLE_MODE_NONE, &device_addr);
	
    for(int i = 0;i<BLE_GAP_ADDR_LEN;i++)
    {
        MAC_ADDR[i] = device_addr.addr[BLE_GAP_ADDR_LEN-1-i];
    }
    memcpy(&src[12], MAC_ADDR, sizeof(MAC_ADDR));
}

/**@brief Function for initializing the Advertising functionality.
 */
void advertising_init(void)
{
    uint32_t               err_code;
    ble_advdata_t          advdata;
    ble_advdata_t          scanrsp;
    ble_adv_modes_config_t options;
    ble_advdata_manuf_data_t  manufdata; 

    // Build advertising data struct to pass into @ref ble_advertising_init.
    memset(&advdata, 0, sizeof(advdata));
    advdata.name_type          = BLE_ADVDATA_FULL_NAME;
    advdata.include_appearance = false;
    advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

    memset(&scanrsp, 0, sizeof(scanrsp));
    scanrsp.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    scanrsp.uuids_complete.p_uuids  = m_adv_uuids;

    memset(&options, 0, sizeof(options)); 
    options.ble_adv_fast_enabled  = true;
    options.ble_adv_fast_interval = APP_ADV_INTERVAL;
    options.ble_adv_fast_timeout  = APP_ADV_TIMEOUT_IN_SECONDS;

//	  memset(manuf_data, 0, sizeof(manuf_data));
    adv_manuf_data_get(manuf_data);
    manufdata.company_identifier       = 0x0059; // Nordics company ID
    manufdata.data.p_data              = manuf_data;    
    manufdata.data.size                = sizeof(manuf_data);
    scanrsp.p_manuf_specific_data = &manufdata;	
	
    err_code = ble_advertising_init(&advdata, &scanrsp, &options, on_adv_evt, NULL);
    APP_ERROR_CHECK(err_code);
}


