#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

/*
#ifdef BUILD_LK
	#include <platform/mt_gpio.h>
#elif defined(BUILD_UBOOT)
    #include <asm/arch/mt_gpio.h>
#else
    #include <mt-plat/mt_gpio.h>
    #include <linux/gpio.h>	
#endif
*/
#ifdef BUILD_LK
#include <platform/upmu_common.h>
#include <platform/mt_gpio.h>
#include <platform/mt_i2c.h>
#include <platform/mt_pmic.h>
#include <string.h>
#elif defined(BUILD_UBOOT)
#include <asm/arch/mt_gpio.h>
#else
/*#include <mach/mt_pm_ldo.h>*/
#ifdef CONFIG_MTK_LEGACY
#include <mach/mt_gpio.h>
#endif
#endif
#ifdef CONFIG_MTK_LEGACY
#include <cust_gpio_usage.h>
#endif
#ifndef CONFIG_FPGA_EARLY_PORTING
#if defined(CONFIG_MTK_LEGACY)
#include <cust_i2c.h>
#endif
#endif
// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  										(720)
#define FRAME_HEIGHT 										(1280)

#define REGFLAG_DELAY             							0XFC
#define REGFLAG_END_OF_TABLE      							0xFD   // END OF REGISTERS MARKER

#define LCM_DSI_CMD_MODE									0
#define GPIO_DISP_BL_EN     86
// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg											lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size)   			lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)    
       

struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};


static struct LCM_setting_table lcm_sleep_out_setting[] = 
{

{0xFF,3,{0x98,0x81,0x03}},

//GIP_1

{0x01,1,{0x00}},
{0x02,1,{0x00}},
{0x03,1,{0x72}},
{0x04,1,{0x00}},
{0x05,1,{0x00}},
{0x06,1,{0x09}},
{0x07,1,{0x00}},
{0x08,1,{0x00}},
{0x09,1,{0x01}},
{0x0a,1,{0x00}},
{0x0b,1,{0x00}},
{0x0c,1,{0x01}},
{0x0d,1,{0x00}},
{0x0e,1,{0x00}},
{0x0f,1,{0x00}},
       
{0x10,1,{0x00}},
{0x11,1,{0x00}},
{0x12,1,{0x00}},
{0x13,1,{0x00}},
{0x14,1,{0x00}},
      
      
      
{0x15,1,{0x04}},
{0x16,1,{0x01}},
{0x17,1,{0x01}},
      
      
      
{0x18,1,{0x00}},
{0x19,1,{0x00}},
{0x1a,1,{0x00}},
{0x1b,1,{0x00}},
{0x1c,1,{0x00}},
{0x1d,1,{0x00}},
{0x1e,1,{0x40}},
{0x1f,1,{0x80}},
      
{0x20,1,{0x05}},
{0x21,1,{0x02}},
{0x22,1,{0x00}},
{0x23,1,{0x00}},
{0x24,1,{0x00}},
{0x25,1,{0x00}},
{0x26,1,{0x00}},
{0x27,1,{0x00}},
{0x28,1,{0x33}},
{0x29,1,{0x02}},
{0x2a,1,{0x00}},
{0x2b,1,{0x00}},
{0x2c,1,{0x00}},
{0x2d,1,{0x00}},
{0x2e,1,{0x00}},
{0x2f,1,{0x00}},
      
{0x30,1,{0x00}},
{0x31,1,{0x00}},
{0x32,1,{0x00}},
{0x33,1,{0x00}},
{0x34,1,{0x04}},
{0x35,1,{0x00}},
{0x36,1,{0x00}},
{0x37,1,{0x00}},
{0x38,1,{0x3C}},
{0x39,1,{0x00}},
{0x3a,1,{0x40}},
{0x3b,1,{0x40}},
{0x3c,1,{0x00}},
{0x3d,1,{0x00}},
{0x3e,1,{0x00}},
{0x3f,1,{0x00}},
      
{0x40,1,{0x00}},
{0x41,1,{0x00}},
{0x42,1,{0x00}},
{0x43,1,{0x00}},
{0x44,1,{0x00}},
      
      
//GIP_,
{0x50,1,{0x01}},
{0x51,1,{0x23}},
{0x52,1,{0x45}},
{0x53,1,{0x67}},
{0x54,1,{0x89}},
{0x55,1,{0xab}},
{0x56,1,{0x01}},
{0x57,1,{0x23}},
{0x58,1,{0x45}},
{0x59,1,{0x67}},
{0x5a,1,{0x89}},
{0x5b,1,{0xab}},
{0x5c,1,{0xcd}},
{0x5d,1,{0xef}},
      
//GIP_
{0x5e,1,{0x11}},
{0x5f,1,{0x01}},
      
{0x60,1,{0x00}},
{0x61,1,{0x15}},
{0x62,1,{0x14}},
{0x63,1,{0x0E}},
{0x64,1,{0x0F}},
{0x65,1,{0x0C}},
{0x66,1,{0x0D}},
{0x67,1,{0x06}},
{0x68,1,{0x02}},
{0x69,1,{0x07}},
{0x6a,1,{0x02}},
{0x6b,1,{0x02}},
{0x6c,1,{0x02}},
{0x6d,1,{0x02}},
{0x6e,1,{0x02}},
{0x6f,1,{0x02}},
      
{0x70,1,{0x02}},
{0x71,1,{0x02}},
{0x72,1,{0x02}},
{0x73,1,{0x02}},
{0x74,1,{0x02}},
{0x75,1,{0x01}},
{0x76,1,{0x00}},
{0x77,1,{0x14}},
{0x78,1,{0x15}},
{0x79,1,{0x0E}},
{0x7a,1,{0x0F}},
{0x7b,1,{0x0C}},
{0x7c,1,{0x0D}},
{0x7d,1,{0x06}},
{0x7e,1,{0x02}},
{0x7f,1,{0x07}},
      
{0x80,1,{0x02}},
{0x81,1,{0x02}},
{0x82,1,{0x02}},
{0x83,1,{0x02}},
{0x84,1,{0x02}},
{0x85,1,{0x02}},
{0x86,1,{0x02}},
{0x87,1,{0x02}},
{0x88,1,{0x02}},
{0x89,1,{0x02}},
{0x8A,1,{0x02}},

//CMD_Page 4
{0xFF,3,{0x98,0x81,0x04}},
{0x6C,1,{0x15}},             //Set VCORE voltage =1.5V
{0x6E,1,{0x2B}},             //di_pwr_reg=0 for power mode 2A //VGH clamp 15V
{0x6F,1,{0x33}},             // reg vcl + pumping ratio VGH=3x VGL=-2.5x
      
{0x8D,1,{0x14}},            //VGL clamp -10
{0x87,1,{0xBA}},            //ESD               
{0x26,1,{0x76}},             //ESD            
{0xB2,1,{0xD1}},            //ESD
{0xB5,1,{0x06}},            //ESD 
{0x33,1,{0x00}},     //¸ÄÉÆTP  noise
{0x3A,1,{0x24}},
{0x35,1,{0x1F}},        
//CMD_Page 1
{0xFF,3,{0x98,0x81,0x01}},
{0x22,1,{0x0A}},//BGR, GS
{0x31,1,{0x00}},//column inversion
{0x40,1,{0x33}},
{0x50,1,{0xA5}},//VREG1OUT=4.692V
{0x51,1,{0xA0}},//VREG2OUT=-4.632V
{0x60,1,{0x22}},              //SDT
{0x61,1,{0x00}},               //CRT
{0x62,1,{0x19}},               //EQT     
{0x63,1,{0x00}},               //PCT    
{0xA0,1,{0x08}},//VP255	Gamma P
{0xA1,1,{0x13}},            //VP251        
{0xA2,1,{0x1D}},            //VP247        
{0xA3,1,{0x0F}},            //VP243        
{0xA4,1,{0x11}},            //VP239        
{0xA5,1,{0x22}},            //VP231        
{0xA6,1,{0x18}},            //VP219        
{0xA7,1,{0x1A}},            //VP203        
{0xA8,1,{0x62}},            //VP175        
{0xA9,1,{0x1B}},            //VP144        
{0xAA,1,{0x27}},            //VP111        
{0xAB,1,{0x59}},           //VP80         
{0xAC,1,{0x1A}},            //VP52         
{0xAD,1,{0x19}},            //VP36         
{0xAE,1,{0x4D}},            //VP24         
{0xAF,1,{0x21}},            //VP16         
{0xB0,1,{0x28}},            //VP12         
{0xB1,1,{0x4E}},            //VP8          
{0xB2,1,{0x61}},            //VP4          
{0xB3,1,{0x39}},            //VP0          
                                        
{0xC0,1,{0x08}},//VN255 GAMMA N
{0xC1,1,{0x13}},            //VN251        
{0xC2,1,{0x1D}},            //VN247        
{0xC3,1,{0x0F}},            //VN243        
{0xC4,1,{0x11}},            //VN239        
{0xC5,1,{0x22}},            //VN231        
{0xC6,1,{0x17}},           //VN219        
{0xC7,1,{0x1A}},            //VN203        
{0xC8,1,{0x62}},            //VN175        
{0xC9,1,{0x1B}},           //VN144        
{0xCA,1,{0x27}},            //VN111        
{0xCB,1,{0x59}},            //VN80         
{0xCC,1,{0x1A}},            //VN52         
{0xCD,1,{0x19}},           //VN36         
{0xCE,1,{0x4D}},            //VN24         
{0xCF,1,{0x21}},            //VN16         
{0xD0,1,{0x28}},            //VN12         
{0xD1,1,{0x4D}},            //VN8          
{0xD2,1,{0x61}},            //VN4          
{0xD3,1,{0x39}},            //VN0

		//CMD_Page 0        
	{0xFF,3,{0x98,0x81,0x00}},
	{0x35,1,{0x00}},
	{0x11,1,{0x00}},
	{REGFLAG_DELAY, 120, {}},		//120 
	{0x29,1,{0x00}},
	{REGFLAG_DELAY, 20, {}},	   //20
 {REGFLAG_END_OF_TABLE, 0x00, {}}

};

static struct LCM_setting_table lcm_sleep_in_setting[] = {
{0xFF, 3, {0x98, 0x81, 0x00}},
 // Display off sequence
 {0x28, 0, {0x00}},
 {REGFLAG_DELAY, 40, {}},
 // Sleep Mode On
 {0x10, 0, {0x00}},
 {REGFLAG_DELAY, 150, {}},
 {REGFLAG_END_OF_TABLE, 0x00, {}}
};
static struct LCM_setting_table lcm_backlight_level_setting[] = {
{0xFF, 3, {0x98, 0x81, 0x00}},	
{0x51, 2, {0x0F,0xFF}},
{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static void lcm_init_power(void)
{
#ifndef CONFIG_FPGA_EARLY_PORTING
#ifdef BUILD_LK
	//mt6325_upmu_set_rg_vgp1_en(1);  //liuyi
#else
	printk("%s, begin\n", __func__);
	//hwPowerOn(MT6325_POWER_LDO_VGP1, VOL_DEFAULT, "LCM_DRV");	//liuyi
	printk("%s, end\n", __func__);
#endif
#endif
}

static void lcm_suspend_power(void)
{
#ifndef CONFIG_FPGA_EARLY_PORTING
#ifdef BUILD_LK
	//mt6325_upmu_set_rg_vgp1_en(0);//liuyi
#else
	printk("%s, begin\n", __func__);
	//hwPowerDown(MT6325_POWER_LDO_VGP1, "LCM_DRV");	//liuyi
	printk("%s, end\n", __func__);
#endif
#endif
}

static void lcm_resume_power(void)
{
#ifndef CONFIG_FPGA_EARLY_PORTING
#ifdef BUILD_LK
	//mt6325_upmu_set_rg_vgp1_en(1);  //liuyi
#else
	printk("%s, begin\n", __func__);
	//hwPowerOn(MT6325_POWER_LDO_VGP1, VOL_DEFAULT, "LCM_DRV");	 //liuyi
	printk("%s, end\n", __func__);
#endif
#endif
}

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

    for(i = 0; i < count; i++) {
		
        unsigned cmd;
        cmd = table[i].cmd;
		
        switch (cmd) {
			
            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;
				
            case REGFLAG_END_OF_TABLE :
                break;
				
            default:
				dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
       	}
    }
	
}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

#if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
	
#else
		params->dsi.mode   = SYNC_EVENT_VDO_MODE; //BURST_VDO_MODE;
	
#endif
	
		// DSI
		/* Command mode setting */
		params->dsi.LANE_NUM				= LCM_FOUR_LANE;
            //The following defined the fomat for data coming from LCD engine.
            params->dsi.data_format.color_order     = LCM_COLOR_ORDER_RGB;
            params->dsi.data_format.trans_seq       = LCM_DSI_TRANS_SEQ_MSB_FIRST;
            params->dsi.data_format.padding         = LCM_DSI_PADDING_ON_LSB;
            params->dsi.data_format.format              = LCM_DSI_FORMAT_RGB888;

            // Highly depends on LCD driver capability.
                 params->dsi.packet_size=256;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

		params->dsi.vertical_sync_active				=15; // 6; //4   
		params->dsi.vertical_backporch				       = 20;  //14  
		params->dsi.vertical_frontporch				       = 30; //14;  //16  
		params->dsi.vertical_active_line				       = FRAME_HEIGHT;     
		params->dsi.horizontal_sync_active				= 40; // 60;   //4
		params->dsi.horizontal_backporch				= 140; //100;  //60  
		params->dsi.horizontal_frontporch				= 120; //100;    //60
		params->dsi.horizontal_blanking_pixel				= 60;   
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;  
		params->dsi.HS_TRAIL = 12;
		
		params->dsi.PLL_CLOCK = 255 ; //212;   //245
///////////////////////////////////////////////////////ESD			
		params->dsi.esd_check_enable = 1;
		params->dsi.customization_esd_check_enable = 1;
		params->dsi.lcm_esd_check_table[0].cmd 			= 0x0a;
		params->dsi.lcm_esd_check_table[0].count 		= 1;
		params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9C;
		params->dsi.noncont_clock = 1;
		params->dsi.noncont_clock_period = 1;
///////////////////////////////////////////////////////			
		params->dsi.vertical_vfp_lp = 100;

}

//#define GPIO_PCD_ID0  14 //62|0x80000000
static unsigned int lcm_compare_id(void)
{
		return 1;


/*
int pin_lcd_id0=0;	
        pin_lcd_id0= gpio_get_value(GPIO_PCD_ID0);	
#ifdef BUILD_LK
	printf("%s, dijing ili9881c , pin_lcd_id0= %d \n", __func__, pin_lcd_id0);
#else
	printk("%s, dijing ili9881c , pin_lcd_id0= %d \n", __func__, pin_lcd_id0);
#endif
	return  (pin_lcd_id0 == 1)?1:0; 
*/
}
static void lcm_init(void)
{
    #ifdef BUILD_LK
    printf("lcm_init:skyworth ili9881c \n");
    #else
    printk("lcm_init:skyworth ili9881c \n");
    #endif
    SET_RESET_PIN(1);
    MDELAY(10);//5
    SET_RESET_PIN(0);
    MDELAY(10);//50
    SET_RESET_PIN(1);
    MDELAY(120);//100
    
    //init_lcm_registers();
    push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
    MDELAY(5);//100
    


}
static void lcm_suspend(void)
{	
    push_table(lcm_sleep_in_setting, sizeof(lcm_sleep_in_setting) / sizeof(struct LCM_setting_table), 1);
    MDELAY(5);
    SET_RESET_PIN(0);
    MDELAY(1);
    // gpio_set_value(GPIO_DISP_BL_EN, 0);
	
}



static void lcm_resume(void)
{

	lcm_init();
//	push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
}

#if (LCM_DSI_CMD_MODE)
static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	dsi_set_cmdq(data_array, 3, 1);
	
	data_array[0]= 0x00053902;
	data_array[1]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[2]= (y1_LSB);
	dsi_set_cmdq(data_array, 3, 1);
	
	data_array[0]= 0x002c3909;
	dsi_set_cmdq(data_array, 1, 0);	
}
#endif
static void lcm_setbacklight(unsigned int level)
{
        unsigned char level_high,level_low;
#ifdef BUILD_LK
	dprintf(0,"%s,zhangjian lk ili9881c_dsi_vdo_skyworth_hd720: level = %d\n", __func__, level);
#else
	printk("%s, zhangjian kernel ili9881c_dsi_vdo_skyworth_hd720 backlight: level = %d\n", __func__, level);
#endif
        // Refresh value of backlight level.
        
        level = 4095*level /255;	
        level_high = (level >> 8) & 0x0F;
        level_low = level & 0x0FF;
        printk("%s,  ili9881c_dsi_vdo_skyworth_hd720 backlight: new level = 0x%x evel_high, level_low 0x%x,0x%x\n", __func__, level,level_high, level_low);
        
        lcm_backlight_level_setting[1].para_list[0] = level_high;
        lcm_backlight_level_setting[1].para_list[1] = level_low;
        
        push_table(lcm_backlight_level_setting, sizeof(lcm_backlight_level_setting) / sizeof(struct LCM_setting_table), 1);

}

LCM_DRIVER ili9881c_dsi_vdo_skyworth_hd720_lcm_drv = 
{
	.name			= "ili9881c_dsi_vdo_skyworth_hd720",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id,
	.init_power		= lcm_init_power,
    .resume_power = lcm_resume_power,
    .suspend_power = lcm_suspend_power,
	.set_backlight	= lcm_setbacklight,
#if (LCM_DSI_CMD_MODE)
    .update         = lcm_update,
#endif
};

