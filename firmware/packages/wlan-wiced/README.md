# wlan-wiced
wlan driver with WICED.  

## Support WiFi board list  
* FH8620-AP6181-43362  
* X1000-RealBoard-AP6212-43438  

## Note  
1. Please check if the `SOC_FH8620` is defined in `rtconfig.h`, if you are using `IoT Camera` board  
2. Please check if the `BOARD_X1000_REALBOARD` is defined in `rtconfig.h`, if you are using `X1000 RealBoard v2` board    
3. Please initialize the WiFi module in application as follows before using 
```
#ifdef RT_USING_WIFI
#include "wlan_wiced.h"

int wifi_init(void)
{
    rt_kprintf("wifi init\n");
    wifi_hw_init();
}
INIT_ENV_EXPORT(wifi_init);

#endif
```
