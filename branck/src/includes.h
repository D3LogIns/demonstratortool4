
#ifndef __includes_h__
#define __includes_h__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>
/**************************************************************************
//common part
**************************************************************************/
#include "common\def.h"
#include "common\option.h"
#include "common\addr.h"

#include "common\isr.h"

#include "common\lib.h"
//#include "slib.h"
/**************************************************************************
//driver part
**************************************************************************/
//#include "driver/uart/uart0.h"
#include "driver\lcd\lcd.h"
#include "driver\lcd\glib.h"
#include "driver\mmu\mmu.h"

//#include "pll.h"
#include "vitest\vitest.h"
#include "touchpanel\tptest.h"

#include "fat16_sd\sdi.h"
#include "fat16_sd\fat16_1.h"

//**************************************************
#include "key\key.h"

#ifdef __cplusplus
}
#endif

#endif   //__includes_h__