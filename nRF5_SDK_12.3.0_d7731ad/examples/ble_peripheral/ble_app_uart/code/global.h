#ifndef __GLOBAL_H
#define __GLOBAL_H

#pragma anon_unions

#include "main.h"

//#pragma anon_unions

//#define ESWAP(word) ((word)>>8 & 0xff | (word)<<8 & 0xff00)

typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;											 				

#define Var_Fill(var,data)      for(uint16_t ii = 0; ii < sizeof(var); ++ii)\
                                *(((uint8_t*)(&var))+ii) = data              //向变量var中填充data

#define TmrSecondsToMilliseconds( n )   ( (uint32_t) ((n) * 1000) )

#define MY_SET_BIT(REG, BIT)     ((REG) |= (BIT))

#define MY_CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))

#define MY_READ_BIT(REG, BIT)    ((REG) & (BIT))

//#define MY_CLEAR_REG(REG)        ((REG) = (0x0))

//#define MY_WRITE_REG(REG, VAL)   ((REG) = (VAL))

#define MY_READ_REG(REG)          ((REG))

#define NumberOfElements(x) (sizeof(x)/sizeof((x)[0]))

#define NB_MS_TO_S                (1000UL)
#define HEARTBEAT_TIME              (2400UL)

#ifndef BV
#define BV(n)      (1 << (n))
#endif

#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)

#define PROTOCOL_APP_HEAD									0xF0A5		/* 协议头 */
#define PROTOCOL_APP_VERSION							0x0100		/* 协议版本 */
#define PROTOCOL_APP_SCENARIOS						0x01			/* 应用场景 */
#if defined ANGUMEN
	#define PROTOCOL_APP_VENDORCODE					0x08			/* 厂商代码 */
#elif defined NUNENG
	#define PROTOCOL_APP_VENDORCODE					0x09			/* 厂商代码 */
#elif defined SFG
	#define PROTOCOL_APP_VENDORCODE					0x0A			/* 厂商代码 */
#else
	#define PROTOCOL_APP_VENDORCODE					0x01			/* 厂商代码 */
#endif
#define PROTOCOL_APP_RESERVE							0x00			/* 保留 */
#define PROTOCOL_APP_SRC_MPHONE						0xAA			/* 数据来源手机 */
#define PROTOCOL_APP_SRC_GATEWAY					0x55			/* 数据来源网关 */
#define PROTOCOL_APP_TAIL									0x160D		/* 协议尾 */

#define PROTOCOL_APP_ERR_NONE							0x00			/* 协议成功 */
#define PROTOCOL_APP_ERR_GEN							0x01			/* 通用错误 */
#define PROTOCOL_APP_ERR_STORAGE					0x02			/* 存储错误 */
#define PROTOCOL_APP_ERR_MODE							0x03			/* 模式错误 */
#define PROTOCOL_APP_ERR_PARAM						0x04			/* 参数错误 */
#define PROTOCOL_APP_ERR_NOT_PERMIT				0x05			/* 操作不允许 */
#define PROTOCOL_APP_ERR_FINISHED					0x06			/* 操作完成 */
#define PROTOCOL_APP_ERR_VERSION					0x07			/* 版本不支持 */
#define PROTOCOL_APP_ERR_PASSWORD_SAME		0x08			/* 内有相同密码 */
#define PROTOCOL_APP_ERR_NOT_FOUND_NB			0x09			/* 未发现联网模块 */
#define PROTOCOL_APP_ERR_NOT_FOUND_SIM		0x0A			/* 未发现SIM卡 */

#define CMD_GET_HARD_INFO_VERSION					0x01			/* 查询硬件版本信息 */
#define CMD_GET_SOFT_INFO_VERSION					0x02			/* 查询软件版本信息 */
#define CMD_GET_AESKEY						    		0xA0			/* 初始化 */
#define CMD_ADD_ADMIN						      		0xA1			/* 添加管理员 */
#define CMD_ADD_USER											0xA2			/* 添加用户 */
#define CMD_READ_USER											0xA3			/* 读取用户 */
#define CMD_DEL_USER											0xA4			/* 删除用户 */
#define CMD_USER_LOGIN										0xA5			/* 用户登录 */
#define CMD_SET_KEYBOARD									0xA6			/* 设置键盘密码 */
#define CMD_ADD_PERIPHERAL								0xA7			/* 添加外设 */
#define CMD_SWITCH												0xA8			/* 开关指令 */
#define CMD_ADJUST_TIME										0xA9			/* 校时指令 */
#define CMD_READ_LOG											0xAA			/* 读取日志 */
#define CMD_READ_USER_CONFIG							0xAB			/* 读取用户信息 */
#define CMD_DEL_LOG												0xAC			/* 删除日志 */
#define CMD_FACTORYRESET									0xAD			/* 恢复出厂设置 */
#define CMD_TERFILEDATA										0xAE			/* 写设备文件数据 */
#define CMD_PARAM_CONFIG									0xAF			/* 参数配置指令 */
#define CMD_PARAM_READ										0xB0			/* 参数读取指令 */

#endif

