#ifndef _user_config_h_
#define _user_config_h_

#ifndef LWIP_OPEN_SRC
	#define LWIP_OPEN_SRC
#endif
#ifndef PBUF_RSV_FOR_WLAN
	#define PBUF_RSV_FOR_WLAN
#endif
#ifndef EBUF_LWIP
	#define EBUF_LWIP
#endif
#ifndef ICACHE_FLASH
	#define ICACHE_FLASH
//	#define ICACHE_FLASH_ATTR __attribute__((section(".irom0.text")))
//	#define ICACHE_RODATA_ATTR __attribute__((section(".irom.text")))
#endif

#ifndef USE_OPEN_LWIP
 #define USE_OPEN_LWIP 140 // использовать OpenLwIP 1.4.0 (при изменении править app/MakeFile #USE_OPEN_LWIP = 140)
#endif

#ifndef USE_MAX_IRAM
	#define USE_MAX_IRAM  48k // использовать часть cache под IRAM, IRAM size = 49152 байт
#endif

//#define USE_OVERLAP_MODE // используются две и более flash

#ifndef USE_OPTIMIZE_PRINTF
	#define USE_OPTIMIZE_PRINTF
#endif

#ifndef USE_US_TIMER
	#define USE_US_TIMER
#endif

#define SDK_VERSION 1129   // 1129 = 1.1.1 release + path N1  // 1.1.2 b1 = 1121
#define SYS_VERSION "0.3.5"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define DEBUGSOO	2  // 0 - откл вывода, 1 - минимум, 2 - норма, >3 - текушая отладка, >4 - удалить что найдется :)

#if DEBUGSOO > 0
	#define UDP_TEST_PORT		1025
#endif

#define USE_CPU_SPEED  160 // 80


#define USE_NETBIOS		1

#define USE_SNTP	1

#define USE_SRV_WEB_PORT    	80

#define USE_TMP2NET_PORT 		0 // 65506 // =0 not use

#endif // _user_config_h_


