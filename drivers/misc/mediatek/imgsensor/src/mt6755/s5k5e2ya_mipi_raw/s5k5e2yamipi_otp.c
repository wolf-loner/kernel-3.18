/*************************************************************************************************
  5e2_otp.c
  ---------------------------------------------------------
  OTP Application file From Truly for s5k5e2ya
  2015.08.19
  ---------------------------------------------------------
NOTE:
The modification is appended to initialization of image sensor. 
After sensor initialization, use the function , and get the id value.
bool otp_wb_update_5e2(BYTE zone)
and
bool otp_lenc_update_5e2(BYTE zone), 
then the calibration of AWB and LSC will be applied. 
After finishing the OTP written, we will provide you the golden_rg and golden_bg settings.
 **************************************************************************************************/

#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>  
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/atomic.h>
#include <linux/slab.h>

//#include <linux/xlog.h>


#include "kd_camera_hw.h"
#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"

#include "s5k5e2yamipiraw_Sensor.h"


#define S5K5E2YA_DEBUG
#ifdef S5K5E2YA_DEBUG
//#define LOG_TAG (__FUNCTION__)
#define PFX "s5k5e2yamipi_otp"
#define LOG_INF(format, args...)	pr_debug("s5k5e2ya_otp" "%s: " format, __FUNCTION__ , ##args) 

//#define SENSORDB(fmt,arg...) pr_debug("s5k5e2ya_otp" "%s: " fmt, __FUNCTION__ , ##arg)  	//printk(LOG_TAG "%s: " fmt "\n", __FUNCTION__ ,##arg)
#else
#define SENSORDB(fmt,arg...)  
#endif

#define s5k5e2yaMIPI_WRITE_ID   0x20
//extern int s5k5e2ya_version;
extern int iReadReg(u16 a_u2Addr , u8 * a_puBuff , u16 i2cId);//add by hhl
extern int iWriteReg(u16 a_u2Addr , u32 a_u4Data , u32 a_u4Bytes , u16 i2cId);//add by hhl
#define s5k5e2ya_write_cmos_sensor(addr, para)  iWriteReg((u16) addr , (u32) para , 1, s5k5e2yaMIPI_WRITE_ID)//add by hhl


#define USHORT             unsigned short
#define BYTE               unsigned char
#define Sleep(ms) mdelay(ms)

#define TRULY_ID_5E2           0x02
//#define LARGAN_LENS        0x01
//#define DONGWOON           0x01
//#define TDK_VCM			   0x01
#define VALID_OTP_5E2          0x01
#define WB_OFFSET_5E2          0x1C

#define GAIN_DEFAULT_5E2       0x0100

#define Golden_RG_5E2   0x303 //xu xiugai lhl
#define Golden_BG_5E2   0x285
USHORT Current_RG_5E2;
USHORT Current_BG_5E2;


static kal_uint32 r_ratio;
static kal_uint32 b_ratio;

kal_uint16 s5k5e2ya_read_cmos_sensor(kal_uint32 addr)
{
	kal_uint16 get_byte=0;
	iReadReg((u16) addr ,(u8*)&get_byte,s5k5e2yaMIPI_WRITE_ID);
	return get_byte;
}
/*************************************************************************************************
 * Function    :  start_read_otp_5e2
 * Description :  before read otp , set the reading block setting  
 * Parameters  :  void
 * Return      :  void
 **************************************************************************************************/
void start_read_otp_5e2(void)
{

	s5k5e2ya_write_cmos_sensor(0x0a00, 0x04);
	s5k5e2ya_write_cmos_sensor(0x0A02, 0x03);   //Select the page to write by writing to 0xD0000A02 0x01~0x0C
	s5k5e2ya_write_cmos_sensor(0x0A00, 0x01);   //Enter read mode by writing 01h to 0xD0000A00
	Sleep(20);
}

/*************************************************************************************************
 * Function    :  stop_read_otp_5e2
 * Description :  after read otp , stop and reset otp block setting  
 **************************************************************************************************/
void stop_read_otp_5e2(void)
{
	s5k5e2ya_write_cmos_sensor(0x0A00, 0x00);   //Reset the NVM interface by writing 00h to 0xD0000A00
}


/*************************************************************************************************
 * Function    :  get_otp_flag_5e2
 * Description :  get otp WRITTEN_FLAG  
 * Parameters  :  [BYTE] zone : OTP PAGE index , 0x00~0x0B
 * Return      :  [BYTE], if 1 , this type has valid otp data, otherwise, invalid otp data
 **************************************************************************************************/
BYTE get_otp_flag_5e2(BYTE zone)
{
	BYTE flag = 0;
	start_read_otp_5e2();
	if(zone==1)
	{
		flag = s5k5e2ya_read_cmos_sensor(0x0A04);

	}
	if(zone==2)
	{
		flag = s5k5e2ya_read_cmos_sensor(0x0A1F);

	}
	stop_read_otp_5e2();
	if((flag&0xFF)==0x00)//empty
	{
		return 0;
	}
	else if((flag&0xFF)==0x01)//valid
	{
		return 1;
	}
	else if((flag&0xFF)==0x03)//invalid
	{
		return 2;
	}
	else 
	{
		return 0;
	}
}

/*************************************************************************************************
 * Function    :  get_otp_date_5e2
 * Description :  get otp date value    
 * Parameters  :  [BYTE] zone : OTP PAGE index , 0x00~0x0B    
 **************************************************************************************************/
#if 0
bool get_otp_date_5e2(BYTE zone) 
{

	BYTE year  = 0;
	BYTE month = 0;
	BYTE day   = 0;
	start_read_otp_5e2();
	year  = s5k5e2ya_read_cmos_sensor(0x0A06+(zone*WB_OFFSET_5E2));
	month = s5k5e2ya_read_cmos_sensor(0x0A07+(zone*WB_OFFSET_5E2));
	day   = s5k5e2ya_read_cmos_sensor(0x0A08+(zone*WB_OFFSET_5E2));
	stop_read_otp_5e2();

	LOG_INF("OTP date=%02d.%02d.%02d", year,month,day);

	return 1;

}
#endif


/*************************************************************************************************
 * Function    :  get_otp_module_id_5e2
 * Description :  get otp MID value 
 * Parameters  :  [BYTE] zone : OTP PAGE index , 0x00~0x0B
 * Return      :  [BYTE] 0 : OTP data fail 
 other value : module ID data , TRULY ID is 0x0002            
 **************************************************************************************************/
#if 0
BYTE get_otp_module_id_5e2(BYTE zone)
{

	BYTE module_id = 0;
	start_read_otp_5e2();

	module_id  = s5k5e2ya_read_cmos_sensor(0x0A05+(zone*WB_OFFSET_5E2));

	stop_read_otp_5e2();

	LOG_INF("OTP_Module ID: 0x%02x.\n",module_id);

	return module_id;

}
#endif


/*************************************************************************************************
 * Function    :  get_otp_lens_id_5e2
 * Description :  get otp LENS_ID value 
 * Parameters  :  [BYTE] zone : OTP PAGE index , 0x00~0x0B
 * Return      :  [BYTE] 0 : OTP data fail 
 other value : LENS ID data             
 **************************************************************************************************/
#if 0
BYTE get_otp_lens_id_5e2(BYTE zone)
{

	BYTE lens_id = 0;

	start_read_otp_5e2();

	lens_id  = s5k5e2ya_read_cmos_sensor(0x0A09+(zone*WB_OFFSET_5E2));

	stop_read_otp_5e2();

	LOG_INF("OTP_Lens ID: 0x%02x.\n",lens_id);

	return lens_id;

}
#endif


/*************************************************************************************************
 * Function    :  get_otp_vcm_id_5e2
 * Description :  get otp VCM_ID value 
 * Parameters  :  [BYTE] zone : OTP PAGE index , 0x00~0x0B
 * Return      :  [BYTE] 0 : OTP data fail 
 other value : VCM ID data             
 **************************************************************************************************/
#if 0
BYTE get_otp_vcm_id_5e2(BYTE zone)
{

	BYTE vcm_id = 0;

	start_read_otp_5e2();

	vcm_id = s5k5e2ya_read_cmos_sensor(0x0A0A+(zone*WB_OFFSET_5E2));

	stop_read_otp_5e2();

	LOG_INF("OTP_VCM ID: 0x%02x.\n",vcm_id);

	return vcm_id;	

}
#endif


/*************************************************************************************************
 * Function    :  get_otp_driver_id_5e2
 * Description :  get otp driver id value 
 * Parameters  :  [BYTE] zone : OTP PAGE index , 0x00~0x0B
 * Return      :  [BYTE] 0 : OTP data fail 
 other value : driver ID data             
 **************************************************************************************************/
#if 0
BYTE get_otp_driver_id_5e2(BYTE zone)
{

	BYTE driver_id = 0;

	start_read_otp_5e2();

	driver_id = s5k5e2ya_read_cmos_sensor(0x0A0B+(zone*WB_OFFSET_5E2));

	stop_read_otp_5e2();

	LOG_INF("OTP_Driver ID: 0x%02x.\n",driver_id);

	return driver_id;

}
#endif

/*************************************************************************************************
 * Function    :  get_light_id_5e2
 * Description :  get otp environment light temperature value 
 * Parameters  :  [BYTE] zone : OTP PAGE index , 0x00~0x0B
 * Return      :  [BYTE] 0 : OTP data fail 
 other value : driver ID data     
BIT0:D65(6500K) EN
BIT1:D50(5100K) EN
BIT2:CWF(4000K) EN
BIT3:A Light(2800K) EN
 **************************************************************************************************/
 #if 0
BYTE get_light_id_5e2(BYTE zone)
{

	BYTE light_id = 0;

	start_read_otp_5e2();
	light_id = s5k5e2ya_read_cmos_sensor(0x0A0D);

	stop_read_otp_5e2();

	LOG_INF("OTP_Light ID: 0x%02x.\n",light_id);

	return light_id;

}
 #endif

#if 1

/*************************************************************************************************
 * Function    :  otp_lenc_update_5e2
 * Description :  Update lens correction 
 * Parameters  :  [BYTE] zone : OTP PAGE index , 0x00~0x0B
 * Return      :  [bool] 0 : OTP data fail 
1 : otp_lenc update success            
 **************************************************************************************************/
void otp_lenc_update_5e2(void)
{

	int flag = 0;
	int sum=0;

	flag = s5k5e2ya_read_cmos_sensor(0x0A42);//flag 0 empty; 1 valid; 3 invalid 

       sum=s5k5e2ya_read_cmos_sensor(0x0A43);

	  printk("otp_lenc_update_5e2---sum=%d\n",sum);
	


   if(flag==1)
 	{
	s5k5e2ya_write_cmos_sensor(0x3400, 0x00);
	s5k5e2ya_write_cmos_sensor(0x3B4C, 0x00);
	s5k5e2ya_write_cmos_sensor(0x3B4C, 0x01);
	s5k5e2ya_write_cmos_sensor(0x0100, 0x01);

 	}

}
#endif

#if 0
static void s5k5e2ya_MIPI_otp_lsc(void)
{
	write_cmos_sensor(0x0B00,0x01); //调用OTP里LSC的设定
    write_cmos_sensor(0x3400,0x00);
}
#endif
/*************************************************************************************************
 * Function    :  wb_gain_set_5e2
 * Description :  Set WB ratio to register gain setting  512x
 * Parameters  :  [int] r_ratio : R ratio data compared with golden module R
b_ratio : B ratio data compared with golden module B
 * Return      :  [bool] 0 : set wb fail 
1 : WB set success            
 **************************************************************************************************/

bool wb_gain_set_5e2(void)
{
	USHORT R_GAIN;
	USHORT B_GAIN;
	USHORT Gr_GAIN;
	USHORT Gb_GAIN;
	USHORT G_GAIN;

    kal_uint16 R_GainH, B_GainH, G_GainH;
	kal_uint16 R_GainL, B_GainL, G_GainL;

	if(!r_ratio || !b_ratio)
	{
		return 0;
	}
	
	if(r_ratio >= 512 )
	{
		if(b_ratio>=512) 
		{
			R_GAIN = (USHORT)(GAIN_DEFAULT_5E2 * r_ratio / 512);						
			G_GAIN = GAIN_DEFAULT_5E2;
			B_GAIN = (USHORT)(GAIN_DEFAULT_5E2 * b_ratio / 512);
		}

		else
		{
			R_GAIN =  (USHORT)(GAIN_DEFAULT_5E2*r_ratio / b_ratio );
			G_GAIN = (USHORT)(GAIN_DEFAULT_5E2*512 / b_ratio );
			B_GAIN = GAIN_DEFAULT_5E2;    
		}
	}
	else
	{
		if(b_ratio >= 512)
		{
			R_GAIN = GAIN_DEFAULT_5E2;
			G_GAIN = (USHORT)(GAIN_DEFAULT_5E2*512 /r_ratio);		
			B_GAIN =  (USHORT)(GAIN_DEFAULT_5E2*b_ratio / r_ratio );
		} 
		else
		{

			Gr_GAIN = (USHORT)(GAIN_DEFAULT_5E2*512/ r_ratio );						
			Gb_GAIN = (USHORT)(GAIN_DEFAULT_5E2*512/b_ratio );						
			if(Gr_GAIN >= Gb_GAIN)						
			{						
				R_GAIN = GAIN_DEFAULT_5E2;						
				G_GAIN = (USHORT)(GAIN_DEFAULT_5E2 *512/ r_ratio );						
				B_GAIN =  (USHORT)(GAIN_DEFAULT_5E2*b_ratio / r_ratio );						
			} 

			else
			{						
				R_GAIN =  (USHORT)(GAIN_DEFAULT_5E2*r_ratio  / b_ratio);						
				G_GAIN = (USHORT)(GAIN_DEFAULT_5E2*512 / b_ratio );						
				B_GAIN = GAIN_DEFAULT_5E2;	
			}
		}        
	}

	R_GainH = (R_GAIN & 0xff00)>>8;
	R_GainL = (R_GAIN & 0x00ff);

	B_GainH = (B_GAIN & 0xff00)>>8;
	B_GainL = (B_GAIN & 0x00ff);

	G_GainH = (G_GAIN & 0xff00)>>8;
	G_GainL = (G_GAIN & 0x00ff);


	LOG_INF("QYC_OTP_golden_rg=%d,golden_bg=%d\n",Golden_RG_5E2,Golden_BG_5E2);
	LOG_INF("QYC_OTP_current_rg=%d,current_bg=%d\n",Current_RG_5E2,Current_BG_5E2);
	LOG_INF("QYC_OTP_r_ratio=%d,b_ratio=%d \n",r_ratio,b_ratio);
#if 1
	s5k5e2ya_write_cmos_sensor(0x020e, G_GainH);
	s5k5e2ya_write_cmos_sensor(0x020f, G_GainL);
	s5k5e2ya_write_cmos_sensor(0x0210, R_GainH);
	s5k5e2ya_write_cmos_sensor(0x0211, R_GainL);
	s5k5e2ya_write_cmos_sensor(0x0212, B_GainH);
	s5k5e2ya_write_cmos_sensor(0x0213, B_GainL);
	s5k5e2ya_write_cmos_sensor(0x0214, G_GainH);
	s5k5e2ya_write_cmos_sensor(0x0215, G_GainL);
#endif
	LOG_INF("QYC_OTP WB Update Finished! \n");
	return 1;

}

/*************************************************************************************************
 * Function    :  get_otp_wb_5e2
 * Description :  Get WB data    
 * Parameters  :  [BYTE] zone : OTP PAGE index , 0x00~0x0f      
 **************************************************************************************************/
bool get_otp_wb_5e2(int zone)
{

	BYTE temph = 0;
	BYTE templ = 0;

	start_read_otp_5e2();
	if(zone == 1){
		templ = s5k5e2ya_read_cmos_sensor(0x0A0C);// + (zone*WB_OFFSET_5E2));
		temph = s5k5e2ya_read_cmos_sensor(0x0A0D);// + (zone*WB_OFFSET_5E2));

		Current_RG_5E2 = (USHORT)((temph<<8)| templ);

		templ = s5k5e2ya_read_cmos_sensor(0x0A0E);//+ (zone*WB_OFFSET_5E2));
		temph = s5k5e2ya_read_cmos_sensor(0x0A0F);//+ (zone*WB_OFFSET_5E2));

		Current_BG_5E2 = (USHORT)((temph<<8)| templ);
	}
	if(zone == 2){
		templ = s5k5e2ya_read_cmos_sensor(0x0A27);// + (zone*WB_OFFSET_5E2));
		temph = s5k5e2ya_read_cmos_sensor(0x0A28);// + (zone*WB_OFFSET_5E2));
	
		Current_RG_5E2 = (USHORT)((temph<<8)| templ);
	
		templ = s5k5e2ya_read_cmos_sensor(0x0A29);//+ (zone*WB_OFFSET_5E2));
		temph = s5k5e2ya_read_cmos_sensor(0x0A2A);//+ (zone*WB_OFFSET_5E2));
	
		Current_BG_5E2 = (USHORT)((temph<<8)| templ);
	}


	LOG_INF("Current_RG_5E2=0x%x, Current_BG_5E2=0x%x\n", Current_RG_5E2, Current_BG_5E2);

	stop_read_otp_5e2();

	return 1;
}


/*************************************************************************************************
 * Function    :  otp_wb_update_5e2
 * Description :  Update WB correction 
 * Return      :  [bool] 0 : OTP data fail 
1 : otp_WB update success            
 **************************************************************************************************/
bool otp_wb_update_5e2(BYTE zone)
{
	//USHORT golden_g, current_g;


	if(!get_otp_wb_5e2(zone))  // get wb data from otp
		return 0;

	r_ratio = 512 * Golden_RG_5E2 / Current_RG_5E2;
	b_ratio = 512 * Golden_BG_5E2 / Current_BG_5E2;

	wb_gain_set_5e2();

	LOG_INF("QYC_WB update finished! \n");

	return 1;
}

/*************************************************************************************************
 * Function    :  otp_update_s5k5e2ya()
 * Description :  update otp data from otp , it otp data is valid, 
 it include get ID and WB update function  
 * Return      :  [bool] 0 : update fail
1 : update success
 **************************************************************************************************/
bool otp_update_s5k5e2ya(void)
{
	BYTE zone = 0x00;
	BYTE FLG = 0x00;
	int i;

	for(i=0;i<3;i++)
	{
		FLG = get_otp_flag_5e2(zone);
		if(FLG == VALID_OTP_5E2)
			break;
		else
			zone++;
	}
	if(i==3)
	{
		LOG_INF("wgs_Warning: No OTP Data or OTP data is invalid!!");
		return 0;
	}

	//MID = get_otp_module_id_5e2(zone);
	//LENS_ID =	get_otp_lens_id_5e2(zone);
	//VCM_ID =	get_otp_vcm_id_5e2(zone);
	//get_otp_date_5e2(zone);
	//get_otp_driver_id_5e2(zone);
	//	get_light_id_5e2(zone);
	//if(MID != TRULY_ID_5E2)
	//{
		LOG_INF("wgs_Warning: No Truly Module !!!!");
	//	return 0;
	//}
	otp_wb_update_5e2(zone);	
	otp_lenc_update_5e2();

	return 1;

}
