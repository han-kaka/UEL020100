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

#define MY_READ_REG(REG)         ((REG))

#define NumberOfElements(x) (sizeof(x)/sizeof((x)[0]))

#define NB_MS_TO_S                (1000UL)
#define HEARTBEAT_TIME              (240UL)

#ifndef BV
#define BV(n)      (1 << (n))
#endif

#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)

#endif

