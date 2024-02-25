#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "usmart.h"
#include "sram.h"
#include "malloc.h" 
#include "sdio_sdcard.h"
#include "w25qxx.h"
#include "ff.h"
#include "exfuns.h"
#include "string.h"
#include "fontupd.h"
#include "text.h"
#include "piclib.h"	
#include "math.h"
#include "exti.h"
#include "dht11.h"
//#include "ds18b20.h"
#include "adc3.h"
#include "lsens.h"

uint8_t temperature;
uint8_t humidity;
uint16_t adcx;
uint8_t indexa=0;
//得到path路径下,目标文件的总个数
//path:路径		    
//返回值:总有效文件数
u16 pic_get_tnum(u8 *path)
{	  
	u8 res;
	u16 rval=0;
 	DIR tdir;	 		//临时目录
	FILINFO *tfileinfo;	//临时文件信息	    			     
	tfileinfo=(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));//申请内存
    res=f_opendir(&tdir,(const TCHAR*)path); 	//打开目录 
	if(res==FR_OK&&tfileinfo)
	{
		while(1)//查询总的有效文件数
		{
	        res=f_readdir(&tdir,tfileinfo);       		//读取目录下的一个文件  	 
	        if(res!=FR_OK||tfileinfo->fname[0]==0)break;//错误了/到末尾了,退出	 		 
			res=f_typetell((u8*)tfileinfo->fname);
			if((res&0XF0)==0X50)//取高四位,看看是不是图片文件	
			{
				rval++;//有效文件数增加1
			}	    
		}  
	}  
	myfree(SRAMIN,tfileinfo);//释放内存
	return rval;
}

void weather(void)
{
    POINT_COLOR=GBLUE;
    Show_Str(50,5,200,24,"温湿度检测",24,0);
    //ai_load_picfile("0:/PICTURE/气温-常温.bmp",0,50,120,120,1);
    ai_load_picfile("0:/PICTURE/湿度.bmp",130,50,100,100,1);
    POINT_COLOR=BLUE;
    LCD_ShowString(30, 180, 200, 16, 16, "Temp:   C");
    LCD_ShowString(160, 180, 200, 16, 16, "Humi:   %");
    if(temperature<=18)
    {
        ai_load_picfile("0:/PICTURE/气温-低温.bmp",0,50,120,120,1);
        ai_load_picfile("0:/PICTURE/加热器.bmp",50,200,50,50,1);
    }
    if(temperature>18&&temperature<=26)
    {
        //LCD_Fill(0,50,120,170,WHITE);
        LCD_Fill(50,200,100,250,WHITE);
        ai_load_picfile("0:/PICTURE/气温-常温.bmp",0,50,120,120,1);
    }
    if(temperature>26)
    {
        ai_load_picfile("0:/PICTURE/气温-高温.bmp",0,50,120,120,1);
        ai_load_picfile("0:/PICTURE/风扇.bmp",50,200,50,50,1);
    }
    if(humidity<=50)
    {
        ai_load_picfile("0:/PICTURE/加湿器.bmp",150,200,50,50,1);
    }
    if(humidity>50&&humidity<=75)
    {
        LCD_Fill(150,200,200,250,WHITE);
    }
    if(humidity>75)
    {
        ai_load_picfile("0:/PICTURE/风扇.bmp",150,200,50,50,1);
    }
}

void sunshine(void)
{
    POINT_COLOR=RED;
    Show_Str(40,5,200,24,"光照强度检测",24,0);
    ai_load_picfile("0:/PICTURE/光照强度.bmp",0,50,120,120,1);
    LCD_ShowString(30, 200, 200, 16, 16, "LSENS_VAL:  ");
    if(adcx<=50)
    {
        LED1=0;
        ai_load_picfile("0:/PICTURE/电灯开.bmp",140,50,70,70,1);
    }
    else
    {
        LED1=0;
        ai_load_picfile("0:/PICTURE/电灯关.bmp",140,50,70,70,1);
    }
}

void sky(void)
{
    POINT_COLOR=GREEN;
    Show_Str(40,5,200,24,"空气质量检测",24,0);
    ai_load_picfile("0:/PICTURE/空气质量分析.bmp",0,50,120,120,1);
    
}

void ledwind(void)
{
    POINT_COLOR=BLUE;
    Show_Str(60,5,200,24,"电灯风扇",24,0);
    ai_load_picfile("0:/PICTURE/电灯关.bmp",10,50,100,100,1);
    ai_load_picfile("0:/PICTURE/风扇.bmp",130,50,100,100,1);
    
}

void music(void)
{
    POINT_COLOR=MAGENTA;
    Show_Str(50,5,200,24,"音乐播放器",24,0);
    ai_load_picfile("0:/PICTURE/千千静听.bmp",60,50,120,120,1);
    
}

void heart(void)
{
    POINT_COLOR=RED;
    Show_Str(60,5,200,24,"心率检测",24,0);
    //ai_load_picfile("0:/PICTURE/.bmp",60,50,120,120,1);
    
}


int main(void)
{
	u8 res;
 	DIR picdir;	 		//图片目录
	FILINFO *picfileinfo;//文件信息 
	u8 *pname;			//带路径的文件名
	u16 totpicnum; 		//图片文件总数
	u16 curindex;		//图片当前索引
	u8 key;				//键值
	//u8 pause=0;			//暂停标记
	u8 t;
	u16 temp;
	u32 *picoffsettbl;	//图片文件offset索引表 
    u8 index=0;         //目录索引
    
	
    HAL_Init();                   	//初始化HAL库    
    Stm32_Clock_Init(336,8,2,7);  	//设置时钟,168Mhz
	delay_init(168);               	//初始化延时函数
	uart_init(115200);             	//初始化USART
	usmart_dev.init(84); 		    //初始化USMART
	LED_Init();						//初始化LED	
	KEY_Init();						//初始化KEY
	LCD_Init();                     //初始化LCD
    EXTI_Init();                    //外部中断初始化
    lsens_init();                           /* 初始化光敏传感器 */
    
	SRAM_Init();					//初始化外部SRAM  
	W25QXX_Init();				    //初始化W25Q256
	my_mem_init(SRAMIN);			//初始化内部内存池
	my_mem_init(SRAMEX);			//初始化外部内存池
	my_mem_init(SRAMCCM);			//初始化CCM内存池 
	exfuns_init();					//为fatfs相关变量申请内存  
 	f_mount(fs[0],"0:",1); 			//挂载SD卡 
 	f_mount(fs[1],"1:",1); 			//挂载FLASH.
	POINT_COLOR=RED;      
	while(font_init()) 				//检查字库
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//清除显示	     
		delay_ms(200);				  
	}  	 
 	Show_Str(30,50,200,16,"阿波罗STM32F4/F7开发板",16,0);				    	 
	Show_Str(30,70,200,16,"智能家居系统",16,0);				    	 
	Show_Str(30,90,200,16,"KEY0:NEXT KEY_UP:PREV",16,0);				    	 		    	 
	Show_Str(30,130,200,16,"李铭宇",16,0);				    	 
	Show_Str(30,150,200,16,"2024年2月6日",16,0);
 	while(f_opendir(&picdir,"0:/PICTURE"))//打开图片文件夹
 	{	    
		Show_Str(30,170,240,16,"PICTURE文件夹错误!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//清除显示	     
		delay_ms(200);				  
	}  
	totpicnum=pic_get_tnum("0:/PICTURE"); //得到总有效文件数
  	while(totpicnum==NULL)//图片文件为0		
 	{	    
		Show_Str(30,170,240,16,"没有图片文件!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//清除显示	     
		delay_ms(200);				  
	} 
	picfileinfo=(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));	//申请内存
 	pname=mymalloc(SRAMIN,_MAX_LFN*2+1);					//为带路径的文件名分配内存
 	picoffsettbl=mymalloc(SRAMIN,4*totpicnum);				//申请4*totpicnum个字节的内存,用于存放图片索引
 	while(!picfileinfo||!pname||!picoffsettbl)				//内存分配出错
 	{	    	
		Show_Str(30,170,240,16,"内存分配失败!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//清除显示	     
		delay_ms(200);				  
	}  	
	//记录索引
    res=f_opendir(&picdir,"0:/PICTURE"); //打开目录
	if(res==FR_OK)
	{
		curindex=0;//当前索引为0
		while(1)//全部查询一遍
		{
			temp=picdir.dptr;								//记录当前dptr偏移
	        res=f_readdir(&picdir,picfileinfo);       		//读取目录下的一个文件
	        if(res!=FR_OK||picfileinfo->fname[0]==0)break;	//错误了/到末尾了,退出	 	 
			res=f_typetell((u8*)picfileinfo->fname);	
			if((res&0XF0)==0X50)//取高四位,看看是不是图片文件	
			{
				picoffsettbl[curindex]=temp;//记录索引
				curindex++;
			}	    
		} 
	} 
    while (dht11_init())    /* DHT11初始化 */
    {
        LCD_ShowString(30, 170, 200, 16, 16, "DHT11 Error");
        delay_ms(200);
        LCD_Fill(30, 170, 239, 170 + 16, WHITE);
        delay_ms(200);
    }
    Show_Str(30,170,200,16,"DHT11 OK!",16,0); 
	Show_Str(30,190,240,16,"开始显示...",16,0); 
	delay_ms(1500);
	piclib_init();										//初始化画图	   	   
	//curindex=1;											//从0开始显示
   	res=f_opendir(&picdir,(const TCHAR*)"0:/PICTURE"); 	//打开目录
	while(res==FR_OK)//打开成功
	{	
		dir_sdi(&picdir,picoffsettbl[curindex]);			//改变当前目录索引	   
        res=f_readdir(&picdir,picfileinfo);       		//读取目录下的一个文件
        if(res!=FR_OK||picfileinfo->fname[0]==0)break;	//错误了/到末尾了,退出
		//strcpy((char*)pname,"0:/PICTURE/");				//复制路径(目录)
		//strcat((char*)pname,(const char*)picfileinfo->fname);//将文件名接在后面
 		LCD_Clear(WHITE);
 		//ai_load_picfile(pname,0,0,lcddev.width,lcddev.height,1);//显示图片
        ai_load_picfile("0:/PICTURE/智能科技-icon-智能仓储.bmp",0,0,lcddev.width,lcddev.height,1);//显示开机界面
        //Show_Str(50,2,lcddev.width,24,"HomeAssistant",24,1);
        LCD_ShowString(50,5,lcddev.width,24,24,"HomeAssistant");
		//Show_Str(2,2,lcddev.width,16,pname,16,1); 				//显示图片名字
        //delay_ms(1000);
        //LCD_Clear(WHITE);
		t=0;
		while(1) 
		{
			key=KEY_Scan(0);		//扫描按键
			//if(t>250)key=1;			//模拟一次按下KEY0 
            if((t%40)==0)LED0=!LED0;//LED0闪烁,提示程序正在运行
            LED1=1;
            /*
			if((t%10)==0)
            {
                dht11_read_data(&temperature, &humidity);             // 读取温湿度值 
                adcx = lsens_get_val();                               // 获取ADC值 
            }
            */
			if(key==WKUP_PRES)		//翻页
			{
                LCD_Clear(WHITE);
                POINT_COLOR=BRRED;
                Show_Str(90,5,200,24,"目录",24,0);
                POINT_COLOR=RED;
                Show_Str(20,40,200,16,"温湿度",16,0);
                Show_Str(20,70,200,16,"光照强度",16,0);
                Show_Str(20,100,200,16,"空气质量",16,0);
                Show_Str(20,130,200,16,"点灯风扇",16,0);
                Show_Str(20,160,200,16,"音乐",16,0);
                Show_Str(20,190,200,16,"心率测量",16,0);
                index++;
                if(index>=6)index=0;
                switch(index)
                {
                    case 0:{LCD_Fill(10,35,200,60,LGRAY);break;}
                    case 1:{LCD_Fill(10,65,200,90,LGRAY);break;}
                    case 2:{LCD_Fill(10,95,200,120,LGRAY);break;}
                    case 3:{LCD_Fill(10,125,200,150,LGRAY);break;}
                    case 4:{LCD_Fill(10,155,200,180,LGRAY);break;}
                    case 5:{LCD_Fill(10,185,200,210,LGRAY);break;}
                }
                //continue;
				//break;
			}
            if(key==KEY0_PRES)     //确定
			{
                LCD_Clear(WHITE);
				while(index==0)
                {
                    indexa=0;
                    LED1=0;
                    //key=KEY_Scan(0);
                    //if(key==KEY1){indexa+=1;}
                    weather();
                    POINT_COLOR=BLACK;
                    dht11_read_data(&temperature, &humidity);
                    LCD_ShowNum(30 + 40, 180, temperature, 2, 16); /* 显示温度 */
                    POINT_COLOR=BLUE;
                    LCD_ShowNum(160 + 40, 180, humidity, 2, 16);    /* 显示湿度 */
                    
                    if(indexa!=0)break;
                     //break;
                }
                while(index==1)
                { 
                    indexa=1;
                    LED1=0;
                    //key=KEY_Scan(0);
                    //if(key==KEY1){indexa+=1;}
                    sunshine();
                    POINT_COLOR=RED;
                    adcx = lsens_get_val();
                    LCD_ShowxNum(30 + 10 * 8, 200, adcx, 3, 16, 0);  /* 显示ADC的值 */
                    //LCD_Fill(110,200,130,220,WHITE);
                    if(indexa!=1)break;
                    //break;
                }   
                while(index==2)
                {
                    indexa=2;
                    LED1=0;
                    //key=KEY_Scan(0);
                    //if(key==KEY1){indexa+=1;}
                    sky();
                    if(indexa!=2)break;
                     //break;
                }
                while(index==3)  
                {
                    indexa=3;
                    LED1=0;
                    //key=KEY_Scan(0);
                    //if(key==KEY1){indexa+=1;}
                    ledwind();
                    if(indexa!=3)break;
                     //break;
                }
                while(index==4)
                {
                    indexa=4;
                    LED1=0;
                    //key=KEY_Scan(0);
                    //if(key==KEY1){indexa+=1;}
                    music();
                    if(indexa!=4)break;
                     //break;
                }
                while(index==5)
                {
                    indexa=5;
                    LED1=0;
                    //key=KEY_Scan(0);
                    //if(key==KEY1){indexa+=1;}
                    heart();
                    if(indexa!=5)break;
                     //break;
                }
				//continue;
			}
            
			t++;
			delay_ms(10);            
		}					    
		//res=0;
        
	} 							    
	myfree(SRAMIN,picfileinfo);			//释放内存						   		    
	myfree(SRAMIN,pname);				//释放内存			    
	myfree(SRAMIN,picoffsettbl);		//释放内存	
}

