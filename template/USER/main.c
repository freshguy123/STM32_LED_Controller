#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	
#include "usmart.h"	 
#include "rtc.h" 
#include "stmflash.h"
#include "touch.h"
#include "timer.h"
#include "adc.h"
#include "lsens.h"

/************************************************
 ALIENTEK 战舰STM32F103开发板实验15
 RTC实时时钟实验  
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

#define DARKBLUE        0X01CF 
#define LIGHTBLUE        0X7D7C 
#define GRAYBLUE         0X5458 



#define LASH_MODE			0X08041000

#define FIRST			0X08070000

#define SUM_ON_ADDR 0x08071000
#define WIN_ON_ADDR		0x08072000
#define SUM_OFF_ADDR	0x08073000
#define WIN_OFF_ADDR	0x08074000
#define SEASON_SUM_ADDR	0X08075000
#define SEASON_WIN_ADDR	0X08076000
#define LIGHT_ADDR		0x08077000


#define SUM_HOUR_ON_ADDR 0x08062000
#define SUM_HOUR_OFF_ADDR		0x08063000
#define SUM_MINUTE_ON_ADDR 0x08064000
#define SUM_MINUTE_OFF_ADDR		0x08065000
#define SUM_SEC_ON_ADDR 0x08066000
#define SUM_SEC_OFF_ADDR		0x08067000

#define WIN_HOUR_ON_ADDR 0x08052000
#define WIN_HOUR_OFF_ADDR		0x08053000
#define WIN_MINUTE_ON_ADDR 0x08054000
#define WIN_MINUTE_OFF_ADDR		0x08055000
#define WIN_SEC_ON_ADDR 0x08056000
#define WIN_SEC_OFF_ADDR		0x08057000

#define LIGHT_SHRESHOLD 0x08040000


u16	sum_on;
u16 sum_off;
u16 sum_hour_on;
u16 sum_minute_on;
u16 sum_hour_off;
u16 sum_minute_off;

u16 win_on;
u16	win_off;
u16 win_hour_on;
u16 win_minute_on;
u16 win_hour_off;
u16 win_minute_off;

u16 timenow;  //当前时间

u16 mode_flag;  //当前模式的标记
u16 key_flag;  //当前按键值得标记

u16 adcx;      //当前光敏值的标记

u16 light_shreshold = 50;  //LED亮灯的阈值
u16 flag_light;

/*
此处用以标记控制面板上的显示
*/
u16 setting_flag=0;
u16 Data_flag=0;
u16 Key_flag=0;
u16 Auto_flag=0;
u16	Lsen_flag=0;




#define LightPeriod	64
#define KeyPeriod 64
#define TouchPeriod 64
#define LsensPeriod 32768
#define LsenSubPeriod LsensPeriod/LSENS_READ_TIMES
#define WifiPeriod 65536
#define CountPeriod 131072

int key_scan=-1;

void initial_show(){
	u8 t=0;	
	POINT_COLOR=BLUE;
	LCD_ShowString(10,5,120,16,16,"LED_Controller");	
	LCD_ShowString(220,20,100,16,16,"@Pangbo");	
	
	POINT_COLOR=BLACK;
	LCD_ShowString(140,45,80,16,24,"Season:");
	LCD_ShowString(140,75,80,16,24,"LED0:");
	LCD_ShowString(140,105,80,16,24,"MODE:");
	POINT_COLOR=RED;
	LCD_DrawLine(0,44,360,44);	
	LCD_DrawLine(0,130,360,130);
	POINT_COLOR=BLACK;	
	if(t!=calendar.sec)
		{
			t=calendar.sec;
			LCD_ShowNum(0,45,calendar.w_year,4,24);		
			LCD_ShowString(50,45,15,16,24,"-");			
			LCD_ShowNum(55,45,calendar.w_month,2,24);	
			LCD_ShowString(80,45,15,16,24,"-");				
			LCD_ShowNum(95,45,calendar.w_date,2,24);	 
			
			switch(calendar.week)
			{
				case 0:
					LCD_ShowString(0,75,200,24,24,"Sunday   ");
					break;
				case 1:
					LCD_ShowString(0,75,200,24,24,"Monday   ");
					break;
				case 2:
					LCD_ShowString(0,75,200,24,24,"Tuesday  ");
					break;
				case 3:
					LCD_ShowString(0,75,200,24,24,"Wednesday");
					break;
				case 4:
					LCD_ShowString(0,75,200,24,24,"Thursday ");
					break;
				case 5:
					LCD_ShowString(0,75,200,24,24,"Friday   ");
					break;
				case 6:
					LCD_ShowString(0,75,200,24,24,"Saturday ");
					break;  
			}
			LCD_ShowNum(0,105,calendar.hour,2,24);		
			LCD_ShowString(25,105,10,16,24,":");				
			LCD_ShowNum(36,105,calendar.min,2,24);		
			LCD_ShowString(61,105,10,16,24,":");				
			LCD_ShowNum(72,105,calendar.sec,2,24);
			if(calendar.w_month>=6&&calendar.w_month<=9)
			{
				LCD_ShowString(230,45,80,16,24,"Summer");	
			}
			else 
			{
				LCD_ShowString(230,45,80,16,24,"Winter");	
			}
		}	
		delay_ms(100);
	  

}





void Key_show()
{
	POINT_COLOR=BLACK;
	LCD_DrawLine(0,480,360,480);	
	LCD_DrawLine(0,430,360,430);
	LCD_DrawLine(0,380,360,380);
	POINT_COLOR=RED;
	LCD_DrawLine(0,330,360,330);
	POINT_COLOR=BLACK;
	LCD_DrawLine(64,330,64,480);	
	LCD_DrawLine(128,330,128,480);
	LCD_DrawLine(192,330,192,480);
	LCD_DrawLine(256,330,256,480);
	
	LCD_ShowNum(25,340,1,1,24);
	LCD_ShowNum(89,340,2,1,24);
	LCD_ShowNum(153,340,3,1,24);
	
	LCD_ShowNum(25,390,4,1,24);
	LCD_ShowNum(89,390,5,1,24);
	LCD_ShowNum(153,390,6,1,24);
	
	LCD_ShowNum(25,440,7,1,24);
  LCD_ShowNum(89,440,8,1,24);
	LCD_ShowNum(153,440,9,1,24);
	
	LCD_ShowNum(217,340,0,1,24);
	LCD_ShowString(205,390,50,16,24,"SET");
	LCD_ShowString(205,440,50,16,24,"Back");
	
	LCD_ShowString(261,340,50,16,24,"Mode1");
	LCD_ShowString(261,390,50,16,24,"Mode2");
	LCD_ShowString(261,440,50,16,24,"Mode3");

	POINT_COLOR=RED;
	LCD_DrawLine(0,190,229,190);
	LCD_DrawLine(229,190,229,330);
}







/*
通过矩阵扫描的方法得到按下的按钮
*/
u16 KbdxSize=64;
u16 KbdySize=50;
u16 x_start=0;
u16 y_start=330;

int Key_Scan()
{
	u16 i,j;
	int key = -1;

	if(tp_dev.sta & TP_PRES_DOWN)
	{
		if((tp_dev.x[0]<lcddev.width)&&(tp_dev.y[0]<lcddev.height)
			&&(tp_dev.y[0]>330))
		{
			for(i=0;i<3;i++)
		{
			for(j=0;j<5;j++)
			{
				if(((tp_dev.x[0]<(x_start+j*64+64))&&(tp_dev.x[0]>(x_start+j*64)))
					&&((tp_dev.y[0]<(y_start+i*50+50))&&(tp_dev.y[0]>(y_start+i*50))))
					{
						LCD_Fill(x_start+j*64,y_start+i*50,
						x_start+j*64+64,y_start+i*50+50,GREEN);
						key = i*5+j;
						delay_ms(10);
						LCD_Fill(x_start+j*64,y_start+i*50,
						x_start+j*64+64,y_start+i*50+50,WHITE);
						break;			
					}
			}
			if(key>=0) 
			{						
				LCD_ShowNum(50,200,key,2,24);
				break;}
	}
		}
		return key;
}
}


/*
按键控制灯
*/
void Handle_Control()
{
	
	LCD_ShowString(220,105,60,24,24,"Key");
	POINT_COLOR=BLUE;
		LCD_ShowString(10,132,200,16,16,"Control the light by key");
		key_flag=KEY_Scan(0);
	  if(key_flag==KEY0_PRES)
		{
			LED0=!LED0;
		}
}



/*
自动模式下flash中存放的灯的设定时间
*/
void auto_time()
{
			if(*(u16*)FIRST!=4759){
		
		FLASH_Unlock();
		FLASH_ErasePage(FIRST);
		FLASH_ProgramWord(FIRST,4759);
			
		FLASH_ErasePage(SUM_ON_ADDR);
		FLASH_ErasePage(SUM_OFF_ADDR);
		FLASH_ErasePage(SUM_HOUR_ON_ADDR);
		FLASH_ErasePage(SUM_HOUR_OFF_ADDR);
		FLASH_ErasePage(SUM_MINUTE_ON_ADDR);
		FLASH_ErasePage(SUM_MINUTE_OFF_ADDR);
	  FLASH_ErasePage(SUM_SEC_ON_ADDR);
		FLASH_ErasePage(SUM_SEC_OFF_ADDR);
		
		FLASH_ProgramWord(SUM_ON_ADDR,6);
		FLASH_ProgramWord(SUM_OFF_ADDR,9);
		FLASH_ProgramWord(SUM_HOUR_ON_ADDR,19);
		FLASH_ProgramWord(SUM_HOUR_OFF_ADDR,5);			
		FLASH_ProgramWord(SUM_MINUTE_ON_ADDR,30);
		FLASH_ProgramWord(SUM_MINUTE_OFF_ADDR,30);
		FLASH_ProgramWord(SUM_SEC_ON_ADDR,0);
		FLASH_ProgramWord(SUM_SEC_OFF_ADDR,0);
		
		FLASH_ErasePage(WIN_ON_ADDR);
		FLASH_ErasePage(WIN_OFF_ADDR);
		FLASH_ErasePage(WIN_HOUR_ON_ADDR);
		FLASH_ErasePage(WIN_HOUR_OFF_ADDR);
		FLASH_ErasePage(WIN_MINUTE_ON_ADDR);
		FLASH_ErasePage(WIN_MINUTE_OFF_ADDR);
	  FLASH_ErasePage(WIN_SEC_ON_ADDR);
		FLASH_ErasePage(WIN_SEC_OFF_ADDR);
		
		FLASH_ProgramWord(WIN_ON_ADDR,12);
		FLASH_ProgramWord(WIN_OFF_ADDR,2);
		FLASH_ProgramWord(WIN_HOUR_ON_ADDR,18);
		FLASH_ProgramWord(WIN_HOUR_OFF_ADDR,6);			
		FLASH_ProgramWord(WIN_MINUTE_ON_ADDR,30);
		FLASH_ProgramWord(WIN_MINUTE_OFF_ADDR,30);
		FLASH_ProgramWord(WIN_SEC_ON_ADDR,0);
		FLASH_ProgramWord(WIN_SEC_OFF_ADDR,0);
		
		FLASH_Lock();
		}
		
		FLASH_Unlock();
		sum_on=*(u8*)SUM_ON_ADDR;
		sum_off=*(u8*)SUM_OFF_ADDR;
		sum_hour_on=*(u8*)SUM_HOUR_ON_ADDR;
		sum_hour_off=*(u8*)SUM_HOUR_OFF_ADDR;
		sum_minute_on=*(u8*)SUM_MINUTE_ON_ADDR;
		sum_minute_off=*(u8*)SUM_MINUTE_OFF_ADDR;
				
		win_on=*(u8*)WIN_ON_ADDR;
		win_off=*(u8*)WIN_OFF_ADDR;
		win_hour_on=*(u8*)WIN_HOUR_ON_ADDR;
		win_hour_off=*(u8*)WIN_HOUR_OFF_ADDR;
		win_minute_on=*(u8*)WIN_MINUTE_ON_ADDR;
		win_minute_off=*(u8*)WIN_MINUTE_OFF_ADDR;
		FLASH_Lock();
		POINT_COLOR=BLUE;
		LCD_ShowString(60,132,40,16,16,"month");
		LCD_ShowString(115,132,60,16,16,"everyday");
		
		LCD_ShowString(0,150,50,16,16,"summer:");
		LCD_ShowNum(52,150,sum_on,2,16);
		LCD_ShowString(70,150,10,16,16,"-");
		LCD_ShowNum(81,150,sum_off,2,16);

		LCD_ShowNum(105,150,sum_hour_on,2,16);
		LCD_ShowString(123,150,6,16,16,":");
		LCD_ShowNum(131,150,sum_minute_on,2,16);
		LCD_ShowString(150,150,8,16,16,"-");
		LCD_ShowNum(161,150,sum_hour_off,2,16);
		LCD_ShowString(179,150,6,16,16,":");
		LCD_ShowNum(187,150,sum_minute_off,2,16);
		
		
		
		LCD_ShowString(0,172,50,16,16,"winter:");
		LCD_ShowNum(52,172,win_on,2,16);
		LCD_ShowString(70,172,10,16,16,"-");
		LCD_ShowNum(81,172,win_off,2,16);

		LCD_ShowNum(105,172,win_hour_on,2,16);
		LCD_ShowString(123,172,6,16,16,":");
		LCD_ShowNum(131,172,win_minute_on,2,16);
		LCD_ShowString(150,172,8,16,16,"-");
		LCD_ShowNum(161,172,win_hour_off,2,16);
		LCD_ShowString(179,172,6,16,16,":");
		LCD_ShowNum(187,172,win_minute_off,2,16);
	  LCD_ShowString(220,105,60,24,24,"Auto");
	
}

/*
自动模式下读取flash中的设定值后自动控制
*/
void Auto_Control(){
	if(calendar.w_month>=(int)sum_on && calendar.w_month<=sum_off)
	{
		if(calendar.hour>sum_hour_on || calendar.hour<sum_hour_off)
		{
			LED0=1;
		}
		if(calendar.hour==(int)sum_hour_on && calendar.min<=sum_hour_off)
	{
			LED0=1;
	}
		if(calendar.hour==(int)sum_hour_off && calendar.min>=sum_off)
	{
			LED0=1;
	}
		else
	{
			LED0=0;
	}
	}
	
		if(calendar.w_month>=(int)win_on && calendar.w_month<=win_off)
	{
		if(calendar.hour>win_hour_on || calendar.hour<win_hour_off)
		{
			LED0=1;
		}
		if(calendar.hour==(int)win_hour_on && calendar.min<=win_hour_off)
	{
			LED0=1;
	}
		if(calendar.hour==(int)win_hour_off && calendar.min>=win_off)
	{
			LED0=1;
	}
		else
	{
			LED0=0;
	}
	}
}



/*
光控模式下的控制
*/
void Lsens_Control()
{
	 LCD_ShowString(220,105,60,24,24,"Env");
	 POINT_COLOR=BLUE;
	 adcx=Lsens_Get_Val();
	 LCD_ShowString(10,132,200,16,16,"Control the light by lsens");
	 LCD_ShowString(10,150,100,16,16,"Light Sens:");
	 LCD_ShowxNum(120,150,adcx,3,16,16);//显示ADC的值
	 LCD_ShowString(10,170,125,16,16,"Light Shreshold:");
	 LCD_ShowxNum(135,170,light_shreshold,3,16,16);//显示ADC的值
	/*
	此处通过flag_light避免光强度值在阈值附近时的闪烁
	*/  
	 if(adcx>=light_shreshold)
	 {
		 LED0=1;
	 }
	 else
	 {
		LED0=0;
	 }
}
/*
显示一个路路灯的熄灭和开启两个状态
*/
void light_show(u8 on)
{
	POINT_COLOR=BLACK;
	LCD_DrawLine(230,320,300,320);
	LCD_DrawLine(230,320,260,310);
	LCD_DrawLine(300,320,270,310);
	
	LCD_DrawLine(260,310,260,230);
	LCD_DrawLine(270,310,270,230);
	
	LCD_DrawLine(270,230,280,220);
	LCD_DrawLine(260,230,250,220);
	
	LCD_DrawLine(250,220,250,185);
	LCD_DrawLine(280,220,280,185);
	
	LCD_DrawLine(250,185,265,170);
	LCD_DrawLine(280,185,265,170);
	
	LCD_DrawLine(250,185,280,185);
	LCD_DrawLine(250,220,280,220);
	LCD_DrawLine(260,230,270,230);

	if(on==1)
	{
		LCD_Fill(251,186,279,219,YELLOW);
	}
	else
	{
		LCD_Fill(251,186,279,219,WHITE);
	}
}


/*
LED的控制
*/
void led_control()
{
	
	FLASH_Unlock();
	FLASH_Unlock();
	mode_flag=*(u8*)LASH_MODE;
	FLASH_Lock();
	LED1=1; //只控制LED0，于是将LED1置为0
	if(mode_flag==1)
	{
		Handle_Control();
	}
	else if(mode_flag==2)
	{
	
		auto_time();       
		Auto_Control();   
	}
	else if(mode_flag==3)
	{
	  Lsens_Control();
	}
	
	if(LED0==1)
	{
		LCD_Fill(250,75,320,105,WHITE);
		POINT_COLOR=RED;
		LCD_ShowString(250,75,60,24,24,"OFF");
		light_show(0);
	}
	else if(LED0==0)
	{
		LCD_Fill(250,75,320,105,WHITE);
		POINT_COLOR=RED;
		LCD_ShowString(250,75,60,24,24,"ON");
		light_show(1);
	}
}



int sum=0;
int i;
u16 input_flag=0;
u32 input[30];
u16 flag=0;
u32 key_set[15]={1,2,3,0,999,4,5,6,999,999,7,8,9,999,999};
int year,month,day,hour,minute,second;
int summer_month_b,summer_month_e,summer_hour_b,summer_hour_e,summer_minute_b,summer_minute_e,
	  winter_month_b,winter_month_e,winter_hour_b,winter_hour_e,winter_minute_b,winter_minute_e;
/*
矩阵扫描触摸屏
*/

int scan_key()   
{
	u16 i,j;
	int key = -1;
  tp_dev.scan(0);
	if(tp_dev.sta & TP_PRES_DOWN)
	{
		if((tp_dev.x[0]<lcddev.width)&&(tp_dev.y[0]<lcddev.height)
			&&(tp_dev.y[0]>330))
		{
			for(i=0;i<3;i++)
		{
			for(j=0;j<5;j++)
			{
				if(((tp_dev.x[0]<(x_start+j*64+64))&&(tp_dev.x[0]>(x_start+j*64)))
					&&((tp_dev.y[0]<(y_start+i*50+50))&&(tp_dev.y[0]>(y_start+i*50))))
					{
						LCD_Fill(x_start+j*64,y_start+i*50,
						x_start+j*64+64,y_start+i*50+50,GREEN);
						key = i*5+j;
						delay_ms(10);
						LCD_Fill(x_start+j*64,y_start+i*50,
						x_start+j*64+64,y_start+i*50+50,WHITE);
						break;			
					}
			}
			if(key>=0) 
			{						
				POINT_COLOR=RED;
				//LCD_ShowNum(50,200,key,2,24);
				
				break;}
	}
		}
		
}
	

	if(key>=0)        //扫描到触摸屏有按压
	 {
		 
		 
		 if(key==4) //key=4对应于”mode1“
		 {
			 LCD_Fill(220,105,290,130,WHITE);
			 LCD_Fill(0,132,360,195,WHITE);
			 mode_flag=1;  //mode_flag对应于手动模式
			 FLASH_Unlock();
			 FLASH_ErasePage(LASH_MODE);
       FLASH_ProgramWord(LASH_MODE,mode_flag);
			 
		 }
		 else if(key==9) //key=9对应于”mode2“
		 {
			 LCD_Fill(220,105,290,130,WHITE);
			 LCD_Fill(0,132,360,195,WHITE);
			 mode_flag=2;
			 FLASH_Unlock();
			 FLASH_ErasePage(LASH_MODE);
       FLASH_ProgramWord(LASH_MODE,mode_flag);
			 
		 }
		 else if(key==14)//key=14对应于”mode3“
		 {
			 LCD_Fill(220,105,290,130,WHITE);
			 LCD_Fill(0,132,360,195,WHITE);
			 mode_flag=3;
			 FLASH_Unlock();
			 FLASH_ErasePage(LASH_MODE);
       FLASH_ProgramWord(LASH_MODE,mode_flag);
			
		 }
		 else if(key==8)
		 {
			 setting_flag=1;
			 
		 }
		 else if(setting_flag==1&&key==0&&Data_flag==0&&Key_flag==0
		 &&Auto_flag==0&&Lsen_flag==0)
		 {
			Data_flag=1;
		 }
	  else if(setting_flag==1&&key==1&&Data_flag==0&&Key_flag==0
		 &&Auto_flag==0&&Lsen_flag==0)
		 {
			Key_flag=1;
		 }
		 else if(setting_flag==1&&key==2&&Data_flag==0&&Key_flag==0
		 &&Auto_flag==0&&Lsen_flag==0)
		 {
			Auto_flag=1;
		 }
		 
		 else if(setting_flag==1&&key==5&&Data_flag==0&&Key_flag==0
		 &&Auto_flag==0&&Lsen_flag==0)
		 {
			Lsen_flag=1;
		 }
		 
		 else if(key==13&&(Data_flag==1||Key_flag==1
		 ||Auto_flag==1||Lsen_flag==1))  //返回主菜单，flag清零
		 {
			 LCD_Fill(0,190,229,330,WHITE);  //返回主菜单时刷新一下
			 setting_flag=1;
			 Data_flag=0;
			 Key_flag=0;
			 Auto_flag=0;
			 Lsen_flag=0;
			 input_flag=0;
			 flag=0;
		 }
		 if(setting_flag==1&&Data_flag==0&&Key_flag==0
		 &&Auto_flag==0&&Lsen_flag==0)
	 {
		   POINT_COLOR=BLUE;
			 LCD_ShowString(10,210,100,16,16,"Setting:");
			 LCD_ShowString(10,230,100,16,16,"1.Date");
			 LCD_ShowString(10,250,100,16,16,"2.KeyMode");
			 LCD_ShowString(10,270,100,16,16,"3.AutoMode");
			 LCD_ShowString(10,290,100,16,16,"4.LsenMode");
	 }

	 else if(setting_flag==1&&Data_flag==1&&input_flag==0)  //进入设定日期的界面
	 {
		 flag=0;
		 input_flag=1;
		 LCD_Fill(0,190,229,330,WHITE);
		 POINT_COLOR=BLUE;
		 LCD_ShowString(10,210,100,16,16,"Data setting like:");
		 LCD_ShowString(10,230,100,16,16,"****-**-** **:**");
		 LCD_ShowString(10,250,100,16,16,"input:");
	 }
	 
	 
	 else if(setting_flag==1&&Data_flag==1&&input_flag==1&&
		 key!=4&&key!=8&&key!=9&&key!=13&&key!=14)  //需要输入日期的数据时
	 {
		 
     sum=0;
		 i=0;
		 input[flag]=key_set[key];
		 flag++;
		 POINT_COLOR=BLUE;
		 LCD_ShowString(10,300,130,16,16,"being set...");
		 if(flag<=4)  //记录年份信息
		 {
			 LCD_ShowString(10,270,50,16,16,"year:");
			 for(i=0;i<flag;i++)
			 {
					sum=sum*10+input[i];
				}
			 LCD_ShowNum(80,270,sum,4,16);
			 if(flag==4)
			 {
				 year=sum;
				 sum=0;
				 LCD_Fill(40,270,228,290,WHITE);
				 LCD_ShowString(10,270,50,16,16,"mouth:");
				 delay_ms(50);
			 }
		 }
		 else if(flag>4&&flag<=6)  //记录月份
		 {
			for(i=0;i<flag-4;i++)
			 {
					sum=sum*10+input[i+4];
				}
			 LCD_ShowNum(80,270,sum,2,16);
			 if(flag==6)
			 {
				 delay_ms(50);
				 month=sum;
				 sum=0;
				 LCD_Fill(40,270,228,290,WHITE);
				 LCD_ShowString(10,270,50,16,16,"day:");
			 }
		 }
		 else if(flag>6&&flag<=8)  //记录天数
		 {
		 
			 for(i=0;i<flag-6;i++)
			 {
					sum=sum*10+input[i+6];
				}
			 LCD_ShowNum(80,270,sum,2,16);
			 if(flag==8)
			 {
				 delay_ms(50);
				 day=sum;
				 sum=0;
				 LCD_Fill(40,270,228,290,WHITE);
				 LCD_ShowString(10,270,50,16,16,"hour:");
			 }
			 
		 }
		 else if(flag>8&&flag<=10)  //记录小时
		 {
		 		 for(i=0;i<flag-8;i++)
			 {
					sum=sum*10+input[i+8];
				}
			 LCD_ShowNum(80,270,sum,2,16);
			 if(flag==10)
			 {
				 delay_ms(50);
				 hour=sum;
				 sum=0;
				 LCD_Fill(40,270,228,290,WHITE);
				 LCD_ShowString(10,270,50,16,16,"minute:");
			 }
		 }
		 else if(flag>10&&flag<=12) //记录分钟
		 {
			  for(i=0;i<flag-10;i++)
			 {
					sum=sum*10+input[i+10];
				}
			 LCD_ShowNum(80,270,sum,2,16);
			 if(flag==12)
			 {
				 delay_ms(50);
				 minute=sum;
				 sum=0;
				 LCD_Fill(40,270,228,290,WHITE);
				 LCD_ShowString(10,270,50,16,16,"second:");
			 }
		 }
		 else if(flag>12&&flag<=14) //记录秒数
		 {
			  for(i=0;i<flag-12;i++)
			 {
					sum=sum*10+input[i+12];
				}
			 LCD_ShowNum(80,270,sum,2,16);
			 if(flag==14)
			 {
				 delay_ms(50);
				 flag=0;
				 second=sum;
				 input_flag=0;
				 sum=0;
				 LCD_Fill(10,270,228,290,WHITE);
				 LCD_ShowString(10,300,130,16,16,"have been set!");
				 
				RTC_ITConfig(RTC_IT_SEC, ENABLE);		
				RTC_WaitForLastTask();	
				RTC_EnterConfigMode();
				RTC_SetPrescaler(32767); 
				RTC_WaitForLastTask();	
				RTC_Set(year,month,day,hour,minute,second); 
				RTC_ExitConfigMode(); 
				 
			 }			 
		 }
	 }
	  else if(setting_flag==1&&Auto_flag==1&&input_flag==0)  //进入设定日期的界面
	 {
		 flag=0;
		 input_flag=1;
		 LCD_Fill(0,190,229,330,WHITE);
		 POINT_COLOR=BLUE;
		 LCD_ShowString(10,210,180,16,16,"Auto_mode setting:");
		  LCD_ShowString(10,230,130,16,16,"****************");
		 LCD_ShowString(10,250,100,16,16,"input:");
	 }

	 else if(setting_flag==1&&Auto_flag==1&&input_flag==1&&
		 key!=4&&key!=8&&key!=9&&key!=13&&key!=14&&flag<=24)  //需要输入日期的数据时
	 {
     sum=0;
		 i=0;
		 input[flag]=key_set[key];
		 flag++;
		 POINT_COLOR=BLUE;
		 LCD_ShowString(10,300,130,16,16,"being set...");
		 if(flag<=2)  //summer_month begin
		 {
			 LCD_ShowString(10,270,150,16,16,"summer_month begin:");
			 for(i=0;i<flag;i++)
			 {
					sum=sum*10+input[i];
				}
			 LCD_ShowNum(190,270,sum,2,16);
			 if(flag==2)
			 {
				 summer_month_b=sum;
				 sum=0;
				 LCD_Fill(190,270,228,290,WHITE);//擦除数字
				  LCD_Fill(10,270,170,290,WHITE);//擦除字符
				 LCD_ShowString(10,270,150,16,16,"summer_month end:");
				 delay_ms(50);
			 }		 
		 }//end of summer_month begin
		 else if(flag>2&&flag<=4)  //summer_month_end
		 {
			 for(i=0;i<flag-2;i++)
			 {
					sum=sum*10+input[i+2];
				}
			 LCD_ShowNum(190,270,sum,4,16);
			 if(flag==4)
			 {
				 summer_month_e=sum;
				 sum=0;
				 LCD_Fill(190,270,228,290,WHITE);
				  LCD_Fill(10,270,170,290,WHITE);//擦除字符
				 LCD_ShowString(10,270,160,16,16,"summer_hour begin:");
				 delay_ms(50);
			 }
		 } //end of summer_month_end
		 
		 else if(flag>4&&flag<=6) //summer_hour_begin
		 {
			 for(i=0;i<flag-4;i++)
			 {
					sum=sum*10+input[i+4];
				}
			 LCD_ShowNum(190,270,sum,4,16);
			 if(flag==6)
			 {
				 summer_hour_b=sum;
				 sum=0;
				 LCD_Fill(190,270,228,290,WHITE);
				  LCD_Fill(10,270,170,290,WHITE);//擦除字符
				 LCD_ShowString(10,270,160,16,16,"summer_minute begin:");
				 delay_ms(50);
			 }
		 }//end of summer_hour_begin
		 
		 else if(flag>6&&flag<=8) //summer_minute_begin
			{
			 for(i=0;i<flag-6;i++)
			 {
					sum=sum*10+input[i+6];
				}
			 LCD_ShowNum(190,270,sum,4,16);
			 if(flag==8)
			 {
				 summer_minute_b=sum;
				 sum=0;
				 LCD_Fill(190,270,228,290,WHITE);
				 LCD_Fill(10,270,170,290,WHITE);//擦除字符
				 LCD_ShowString(10,270,150,16,16,"summer_hour end:");
				 delay_ms(50);
			 }
		 }//end of summer_minute_begin
		 
		 else if(flag>8&&flag<=10) //summer_hour_end
			{
			 for(i=0;i<flag-8;i++)
			 {
					sum=sum*10+input[i+8];
				}
			 LCD_ShowNum(190,270,sum,4,16);
			 if(flag==10)
			 {
				 summer_hour_e=sum;
				 sum=0;
				 LCD_Fill(190,270,228,290,WHITE);
				  LCD_Fill(10,270,170,290,WHITE);//擦除字符
				 LCD_ShowString(10,270,150,16,16,"summer_minute end:");
				 delay_ms(50);
			 }
		 }//end of summer_hour_end
			
		 else if(flag>10&&flag<=12) //summer_minute_end
			{
			 for(i=0;i<flag-10;i++)
			 {
					sum=sum*10+input[i+10];
				}
			 LCD_ShowNum(190,270,sum,4,16);
			 if(flag==12)
			 {
				 summer_minute_e=sum;
				 sum=0;
				 LCD_Fill(190,270,228,290,WHITE);
				   LCD_Fill(10,270,170,290,WHITE);//擦除字符
				 LCD_ShowString(10,270,160,16,16,"winter_month begin:");
				 delay_ms(50);
			 }
		 }//end of summer_minute_end
			
		  else if(flag>12&&flag<=14) //winter_month_begin
			{
			 for(i=0;i<flag-12;i++)
			 {
					sum=sum*10+input[i+12];
				}
			 LCD_ShowNum(190,270,sum,4,16);
			 if(flag==14)
			 {
				 winter_month_b=sum;
				 sum=0;
				 LCD_Fill(190,270,228,290,WHITE);
				   LCD_Fill(10,270,170,290,WHITE);//擦除字符
				 LCD_ShowString(10,270,160,16,16,"winter_month end:");
				 delay_ms(50);
			 }
		 }//end of winter_month_begin
			
		 else if(flag>14&&flag<=16) //winter_month_end
			{
			 for(i=0;i<flag-14;i++)
			 {
					sum=sum*10+input[i+14];
				}
			 LCD_ShowNum(190,270,sum,4,16);
			 if(flag==16)
			 {
				 winter_month_e=sum;
				 sum=0;
				 LCD_Fill(190,270,228,290,WHITE);
				  LCD_Fill(10,270,170,290,WHITE);//擦除字符
				 LCD_ShowString(10,270,160,16,16,"winter_hour bagin:");
				 delay_ms(50);
			 }
		 }//end of winter_month_end
			
		 else if(flag>16&&flag<=18) //winter_hour_begin
			{
			 for(i=0;i<flag-16;i++)
			 {
					sum=sum*10+input[i+16];
				}
			 LCD_ShowNum(190,270,sum,4,16);
			 if(flag==18)
			 {
				 winter_hour_b=sum;
				 sum=0;
				 LCD_Fill(190,270,228,290,WHITE);
				  LCD_Fill(10,270,170,290,WHITE);//擦除字符
				 LCD_ShowString(10,270,160,16,16,"winter_minute end:");
				 delay_ms(50);
			 }
		 }//end of winter_hour_begin
		 
		 else if(flag>18&&flag<=20) //winter_minute_begin
			{
			 for(i=0;i<flag-18;i++)
			 {
					sum=sum*10+input[i+18];
				}
			 LCD_ShowNum(190,270,sum,4,16);
			 if(flag==20)
			 {
				 winter_minute_b=sum;
				 sum=0;
				 LCD_Fill(190,270,228,290,WHITE);
				  LCD_Fill(10,270,170,290,WHITE);//擦除字符
				 LCD_ShowString(10,270,160,16,16,"winter_hour end:");
				 delay_ms(50);
			 }
		 }//end of winter_minute_begin
			
		 else if(flag>20&&flag<=22) //winter_hour_end
			{
			 for(i=0;i<flag-20;i++)
			 {
					sum=sum*10+input[i+20];
				}
			 LCD_ShowNum(190,270,sum,4,16);
			 if(flag==22)
			 {
				 winter_hour_e=sum;
				 sum=0;
				 LCD_Fill(190,270,228,290,WHITE);
				 LCD_Fill(10,270,170,290,WHITE);//擦除字符
				 LCD_ShowString(10,270,160,16,16,"winter_minute end:");
				 delay_ms(50);
			 }
		 }//end of winter_hour_end
		 
		 else if(flag>22&&flag<=24) //winter_minute_end
			{
			 for(i=0;i<flag-22;i++)
			 {
					sum=sum*10+input[i+22];
				}
			 LCD_ShowNum(190,270,sum,4,16);
			 if(flag==24)
			 {
				 
				 winter_minute_e=sum;
				 sum=0;
				 LCD_Fill(190,270,228,290,WHITE);
				 LCD_Fill(190,270,228,290,WHITE);
				 //LCD_ShowString(10,270,150,16,16,"winter_hour end:");
				 delay_ms(50);
				 LCD_ShowString(10,300,130,16,16,"have been set!");
				 {  //将数据写入flash
				 
					 		FLASH_Unlock();

							FLASH_ErasePage(SUM_ON_ADDR);
							FLASH_ErasePage(SUM_OFF_ADDR);
							FLASH_ErasePage(SUM_HOUR_ON_ADDR);
							FLASH_ErasePage(SUM_HOUR_OFF_ADDR);
							FLASH_ErasePage(SUM_MINUTE_ON_ADDR);
							FLASH_ErasePage(SUM_MINUTE_OFF_ADDR);
	
							FLASH_ProgramWord(SUM_ON_ADDR,summer_month_b);
							FLASH_ProgramWord(SUM_OFF_ADDR,summer_month_e);
							FLASH_ProgramWord(SUM_HOUR_ON_ADDR,summer_hour_b);
							FLASH_ProgramWord(SUM_HOUR_OFF_ADDR,summer_hour_e);			
							FLASH_ProgramWord(SUM_MINUTE_ON_ADDR,summer_minute_b);
							FLASH_ProgramWord(SUM_MINUTE_OFF_ADDR,summer_minute_e);
					 
							FLASH_ErasePage(WIN_ON_ADDR);
							FLASH_ErasePage(WIN_OFF_ADDR);
							FLASH_ErasePage(WIN_HOUR_ON_ADDR);
							FLASH_ErasePage(WIN_HOUR_OFF_ADDR);
							FLASH_ErasePage(WIN_MINUTE_ON_ADDR);
							FLASH_ErasePage(WIN_MINUTE_OFF_ADDR);

							FLASH_ProgramWord(WIN_ON_ADDR,winter_month_b);
							FLASH_ProgramWord(WIN_OFF_ADDR,winter_month_e);
							FLASH_ProgramWord(WIN_HOUR_ON_ADDR,winter_hour_b);
							FLASH_ProgramWord(WIN_HOUR_OFF_ADDR,winter_hour_e);			
							FLASH_ProgramWord(WIN_MINUTE_ON_ADDR,winter_minute_b);
							FLASH_ProgramWord(WIN_MINUTE_OFF_ADDR,winter_minute_e);
							
							FLASH_Lock();
					 
				 }  //flash写数据完成
				 
			 }
		 }//end of winter_hour_end
		 
	 }
	 
	 }  //end of if
	 
}	





void TIM3_IRQHandler(void)
{
	 int key=-1;
	 initial_show();
	 Key_show();				//将键盘显示在界面
	 led_control();    //控制LED
	 scan_key();   //定时扫描键盘
	 
}	  //TIM3中断


 int main(void)
 {	 
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
 	LED_Init();			     //LED端口初始化
	LCD_Init();			 	
	//TPAD_Init(6);
	usmart_dev.init(SystemCoreClock/1000000);	//初始化USMART	
	RTC_Init();	  			//RTC初始化
	Lsens_Init(); 			//初始化光敏传感器
	key_scan=0;
	KEY_Init(); 
	tp_dev.init();
	TIM3_Int_Init(9,7199);

}

