#include <rtm.h>
#include <rtthread.h>

#include <gpio.h>
RTM_EXPORT(gpio_to_irq);
RTM_EXPORT(gpio_release);
RTM_EXPORT(gpio_direction_input);
RTM_EXPORT(gpio_irq_enable);
RTM_EXPORT(gpio_set_irq_type);
RTM_EXPORT(gpio_request);

extern void fh_select_gpio(int gpio_no);
RTM_EXPORT(fh_select_gpio);

#include <lwip/pbuf.h>
RTM_EXPORT(pbuf_free)
RTM_EXPORT(pbuf_ref);
RTM_EXPORT(pbuf_copy);
RTM_EXPORT(pbuf_alloc);
RTM_EXPORT(pbuf_header);

RTM_EXPORT(sys_arch_assert);

#include <inc/fh_sdio.h>
RTM_EXPORT(sdio_high_speed_mode);
RTM_EXPORT(fh_sdio1_init);
RTM_EXPORT(sdio_drv_creg_write);
RTM_EXPORT(sdio_drv_write);
RTM_EXPORT(sdio_drv_creg_read);
RTM_EXPORT(sdio_drv_read);
RTM_EXPORT(sdio_init);

#include <netif/ethernetif.h>
#include <lwip/etharp.h>
RTM_EXPORT(eth_device_linkchange);
RTM_EXPORT(netif_set_link_up);
RTM_EXPORT(etharp_output);
RTM_EXPORT(eth_device_init);
RTM_EXPORT(eth_device_init_with_flag);

#include <rthw.h>
RTM_EXPORT(rt_hw_interrupt_install);

extern int sdio_set_bcm43362(int bcm43362);
RTM_EXPORT(sdio_set_bcm43362);

