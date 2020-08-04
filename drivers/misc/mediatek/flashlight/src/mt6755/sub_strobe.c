
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/time.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include "kd_camera_typedef.h"
//#include "kd_camera_hw.h"
//#include <cust_gpio_usage.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
//#include <linux/xlog.h>
#include <linux/gpio.h>
#include <linux/version.h>
#ifdef CONFIG_COMPAT
#include <linux/fs.h>
#include <linux/compat.h>
#endif
#include "kd_flashlight.h"
/******************************************************************************
 * Debug configuration
******************************************************************************/
/* availible parameter */
/* ANDROID_LOG_ASSERT */
/* ANDROID_LOG_ERROR */
/* ANDROID_LOG_WARNING */
/* ANDROID_LOG_INFO */
/* ANDROID_LOG_DEBUG */
/* ANDROID_LOG_VERBOSE */
#define TAG_NAME "[sub_strobe.c]"
#define PK_DBG_NONE(fmt, arg...)    do {} while (0)
#define PK_DBG_FUNC(fmt, arg...)    pr_debug(TAG_NAME "%s: " fmt, __func__ , ##arg)
#define PK_WARN(fmt, arg...)        pr_warn(TAG_NAME "%s: " fmt, __func__ , ##arg)
#define PK_NOTICE(fmt, arg...)      pr_notice(TAG_NAME "%s: " fmt, __func__ , ##arg)
#define PK_INFO(fmt, arg...)        pr_info(TAG_NAME "%s: " fmt, __func__ , ##arg)
#define PK_TRC_FUNC(f)              pr_debug(TAG_NAME "<%s>\n", __func__)
#define PK_TRC_VERBOSE(fmt, arg...) pr_debug(TAG_NAME fmt, ##arg)
#define PK_ERROR(fmt, arg...)       pr_err(TAG_NAME "%s: " fmt, __func__ , ##arg)

#define  GPIO_CAMERA_FLASH_EXT2_PIN       27
#define DEBUG_LEDS_STROBE
#ifdef DEBUG_LEDS_STROBE
#define PK_DBG PK_DBG_FUNC
#define PK_VER PK_TRC_VERBOSE
#define PK_ERR PK_ERROR
#else
#define PK_DBG(a, ...)
#define PK_VER(a, ...)
#define PK_ERR(a, ...)
#endif


void sub_flashlight_GPIO_enable(void){
int ret=0;
            gpio_set_value(GPIO_CAMERA_FLASH_EXT2_PIN,1);
	  ret=gpio_get_value(GPIO_CAMERA_FLASH_EXT2_PIN);
	    printk("[CAMERA SENSOR] set gpio VCAM_front_flashlight_pin=%d\n", ret); 
}
void sub_flashlight_GPIO_disable(void){
		int ret=0;
            gpio_set_value(GPIO_CAMERA_FLASH_EXT2_PIN,0);
	  ret=gpio_get_value(GPIO_CAMERA_FLASH_EXT2_PIN);
	    printk("[CAMERA SENSOR] set gpio VCAM_front_flashlight_pin=%d\n", ret);  
}
static int sub_strobe_ioctl(unsigned int cmd, unsigned long arg)
{
	
   int RetValue;
	int ior_shift;
	int iow_shift;
	int iowr_shift;
	printk("shenchuangye===sub dummy ioctl");
	ior_shift = cmd - (_IOR(FLASHLIGHT_MAGIC,0, int));
	iow_shift = cmd - (_IOW(FLASHLIGHT_MAGIC,0, int));
	iowr_shift = cmd - (_IOWR(FLASHLIGHT_MAGIC,0, int));
    switch(cmd)
    {
		case FLASH_IOC_SET_TIME_OUT_TIME_MS:
		break;
    	case FLASH_IOC_SET_DUTY :
    		break;
    	case FLASH_IOC_SET_STEP:
    		printk("FLASH_IOC_SET_STEP: %d\n",(int)arg);
    		break;
    	case FLASH_IOC_SET_ONOFF :
    		printk("FLASHLIGHT_ONOFF: %d\n",(int)arg);
    		if(arg==1)
    		{
    		sub_flashlight_GPIO_enable();
    		}
    		else
    		{
    		sub_flashlight_GPIO_disable();
    		}
    		break;
		default :
    		printk(" No such command \n");
    		RetValue = -EPERM;
    		break;
    }
    return RetValue;
}

static int sub_strobe_open(void *pArg)
{
    printk("shenchuangye===sub dummy open");
	return 0;

}

static int sub_strobe_release(void *pArg)
{
	PK_DBG("sub dummy release");
	return 0;

}

FLASHLIGHT_FUNCTION_STRUCT subStrobeFunc = {
	sub_strobe_open,
	sub_strobe_release,
	sub_strobe_ioctl
};


MUINT32 subStrobeInit(PFLASHLIGHT_FUNCTION_STRUCT *pfFunc)
{
	if (pfFunc != NULL)
		*pfFunc = &subStrobeFunc;
	return 0;
}
