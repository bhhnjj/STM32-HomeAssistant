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
//�õ�path·����,Ŀ���ļ����ܸ���
//path:·��		    
//����ֵ:����Ч�ļ���
u16 pic_get_tnum(u8 *path)
{	  
	u8 res;
	u16 rval=0;
 	DIR tdir;	 		//��ʱĿ¼
	FILINFO *tfileinfo;	//��ʱ�ļ���Ϣ	    			     
	tfileinfo=(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));//�����ڴ�
    res=f_opendir(&tdir,(const TCHAR*)path); 	//��Ŀ¼ 
	if(res==FR_OK&&tfileinfo)
	{
		while(1)//��ѯ�ܵ���Ч�ļ���
		{
	        res=f_readdir(&tdir,tfileinfo);       		//��ȡĿ¼�µ�һ���ļ�  	 
	        if(res!=FR_OK||tfileinfo->fname[0]==0)break;//������/��ĩβ��,�˳�	 		 
			res=f_typetell((u8*)tfileinfo->fname);
			if((res&0XF0)==0X50)//ȡ����λ,�����ǲ���ͼƬ�ļ�	
			{
				rval++;//��Ч�ļ�������1
			}	    
		}  
	}  
	myfree(SRAMIN,tfileinfo);//�ͷ��ڴ�
	return rval;
}

void weather(void)
{
    POINT_COLOR=GBLUE;
    Show_Str(50,5,200,24,"��ʪ�ȼ��",24,0);
    //ai_load_picfile("0:/PICTURE/����-����.bmp",0,50,120,120,1);
    ai_load_picfile("0:/PICTURE/ʪ��.bmp",130,50,100,100,1);
    POINT_COLOR=BLUE;
    LCD_ShowString(30, 180, 200, 16, 16, "Temp:   C");
    LCD_ShowString(160, 180, 200, 16, 16, "Humi:   %");
    if(temperature<=18)
    {
        ai_load_picfile("0:/PICTURE/����-����.bmp",0,50,120,120,1);
        ai_load_picfile("0:/PICTURE/������.bmp",50,200,50,50,1);
    }
    if(temperature>18&&temperature<=26)
    {
        //LCD_Fill(0,50,120,170,WHITE);
        LCD_Fill(50,200,100,250,WHITE);
        ai_load_picfile("0:/PICTURE/����-����.bmp",0,50,120,120,1);
    }
    if(temperature>26)
    {
        ai_load_picfile("0:/PICTURE/����-����.bmp",0,50,120,120,1);
        ai_load_picfile("0:/PICTURE/����.bmp",50,200,50,50,1);
    }
    if(humidity<=50)
    {
        ai_load_picfile("0:/PICTURE/��ʪ��.bmp",150,200,50,50,1);
    }
    if(humidity>50&&humidity<=75)
    {
        LCD_Fill(150,200,200,250,WHITE);
    }
    if(humidity>75)
    {
        ai_load_picfile("0:/PICTURE/����.bmp",150,200,50,50,1);
    }
}

void sunshine(void)
{
    POINT_COLOR=RED;
    Show_Str(40,5,200,24,"����ǿ�ȼ��",24,0);
    ai_load_picfile("0:/PICTURE/����ǿ��.bmp",0,50,120,120,1);
    LCD_ShowString(30, 200, 200, 16, 16, "LSENS_VAL:  ");
    if(adcx<=50)
    {
        LED1=0;
        ai_load_picfile("0:/PICTURE/��ƿ�.bmp",140,50,70,70,1);
    }
    else
    {
        LED1=0;
        ai_load_picfile("0:/PICTURE/��ƹ�.bmp",140,50,70,70,1);
    }
}

void sky(void)
{
    POINT_COLOR=GREEN;
    Show_Str(40,5,200,24,"�����������",24,0);
    ai_load_picfile("0:/PICTURE/������������.bmp",0,50,120,120,1);
    
}

void ledwind(void)
{
    POINT_COLOR=BLUE;
    Show_Str(60,5,200,24,"��Ʒ���",24,0);
    ai_load_picfile("0:/PICTURE/��ƹ�.bmp",10,50,100,100,1);
    ai_load_picfile("0:/PICTURE/����.bmp",130,50,100,100,1);
    
}

void music(void)
{
    POINT_COLOR=MAGENTA;
    Show_Str(50,5,200,24,"���ֲ�����",24,0);
    ai_load_picfile("0:/PICTURE/ǧǧ����.bmp",60,50,120,120,1);
    
}

void heart(void)
{
    POINT_COLOR=RED;
    Show_Str(60,5,200,24,"���ʼ��",24,0);
    //ai_load_picfile("0:/PICTURE/.bmp",60,50,120,120,1);
    
}


int main(void)
{
	u8 res;
 	DIR picdir;	 		//ͼƬĿ¼
	FILINFO *picfileinfo;//�ļ���Ϣ 
	u8 *pname;			//��·�����ļ���
	u16 totpicnum; 		//ͼƬ�ļ�����
	u16 curindex;		//ͼƬ��ǰ����
	u8 key;				//��ֵ
	//u8 pause=0;			//��ͣ���
	u8 t;
	u16 temp;
	u32 *picoffsettbl;	//ͼƬ�ļ�offset������ 
    u8 index=0;         //Ŀ¼����
    
	
    HAL_Init();                   	//��ʼ��HAL��    
    Stm32_Clock_Init(336,8,2,7);  	//����ʱ��,168Mhz
	delay_init(168);               	//��ʼ����ʱ����
	uart_init(115200);             	//��ʼ��USART
	usmart_dev.init(84); 		    //��ʼ��USMART
	LED_Init();						//��ʼ��LED	
	KEY_Init();						//��ʼ��KEY
	LCD_Init();                     //��ʼ��LCD
    EXTI_Init();                    //�ⲿ�жϳ�ʼ��
    lsens_init();                           /* ��ʼ������������ */
    
	SRAM_Init();					//��ʼ���ⲿSRAM  
	W25QXX_Init();				    //��ʼ��W25Q256
	my_mem_init(SRAMIN);			//��ʼ���ڲ��ڴ��
	my_mem_init(SRAMEX);			//��ʼ���ⲿ�ڴ��
	my_mem_init(SRAMCCM);			//��ʼ��CCM�ڴ�� 
	exfuns_init();					//Ϊfatfs��ر��������ڴ�  
 	f_mount(fs[0],"0:",1); 			//����SD�� 
 	f_mount(fs[1],"1:",1); 			//����FLASH.
	POINT_COLOR=RED;      
	while(font_init()) 				//����ֿ�
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}  	 
 	Show_Str(30,50,200,16,"������STM32F4/F7������",16,0);				    	 
	Show_Str(30,70,200,16,"���ܼҾ�ϵͳ",16,0);				    	 
	Show_Str(30,90,200,16,"KEY0:NEXT KEY_UP:PREV",16,0);				    	 		    	 
	Show_Str(30,130,200,16,"������",16,0);				    	 
	Show_Str(30,150,200,16,"2024��2��6��",16,0);
 	while(f_opendir(&picdir,"0:/PICTURE"))//��ͼƬ�ļ���
 	{	    
		Show_Str(30,170,240,16,"PICTURE�ļ��д���!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}  
	totpicnum=pic_get_tnum("0:/PICTURE"); //�õ�����Ч�ļ���
  	while(totpicnum==NULL)//ͼƬ�ļ�Ϊ0		
 	{	    
		Show_Str(30,170,240,16,"û��ͼƬ�ļ�!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//�����ʾ	     
		delay_ms(200);				  
	} 
	picfileinfo=(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));	//�����ڴ�
 	pname=mymalloc(SRAMIN,_MAX_LFN*2+1);					//Ϊ��·�����ļ��������ڴ�
 	picoffsettbl=mymalloc(SRAMIN,4*totpicnum);				//����4*totpicnum���ֽڵ��ڴ�,���ڴ��ͼƬ����
 	while(!picfileinfo||!pname||!picoffsettbl)				//�ڴ�������
 	{	    	
		Show_Str(30,170,240,16,"�ڴ����ʧ��!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}  	
	//��¼����
    res=f_opendir(&picdir,"0:/PICTURE"); //��Ŀ¼
	if(res==FR_OK)
	{
		curindex=0;//��ǰ����Ϊ0
		while(1)//ȫ����ѯһ��
		{
			temp=picdir.dptr;								//��¼��ǰdptrƫ��
	        res=f_readdir(&picdir,picfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
	        if(res!=FR_OK||picfileinfo->fname[0]==0)break;	//������/��ĩβ��,�˳�	 	 
			res=f_typetell((u8*)picfileinfo->fname);	
			if((res&0XF0)==0X50)//ȡ����λ,�����ǲ���ͼƬ�ļ�	
			{
				picoffsettbl[curindex]=temp;//��¼����
				curindex++;
			}	    
		} 
	} 
    while (dht11_init())    /* DHT11��ʼ�� */
    {
        LCD_ShowString(30, 170, 200, 16, 16, "DHT11 Error");
        delay_ms(200);
        LCD_Fill(30, 170, 239, 170 + 16, WHITE);
        delay_ms(200);
    }
    Show_Str(30,170,200,16,"DHT11 OK!",16,0); 
	Show_Str(30,190,240,16,"��ʼ��ʾ...",16,0); 
	delay_ms(1500);
	piclib_init();										//��ʼ����ͼ	   	   
	//curindex=1;											//��0��ʼ��ʾ
   	res=f_opendir(&picdir,(const TCHAR*)"0:/PICTURE"); 	//��Ŀ¼
	while(res==FR_OK)//�򿪳ɹ�
	{	
		dir_sdi(&picdir,picoffsettbl[curindex]);			//�ı䵱ǰĿ¼����	   
        res=f_readdir(&picdir,picfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
        if(res!=FR_OK||picfileinfo->fname[0]==0)break;	//������/��ĩβ��,�˳�
		//strcpy((char*)pname,"0:/PICTURE/");				//����·��(Ŀ¼)
		//strcat((char*)pname,(const char*)picfileinfo->fname);//���ļ������ں���
 		LCD_Clear(WHITE);
 		//ai_load_picfile(pname,0,0,lcddev.width,lcddev.height,1);//��ʾͼƬ
        ai_load_picfile("0:/PICTURE/���ܿƼ�-icon-���ִܲ�.bmp",0,0,lcddev.width,lcddev.height,1);//��ʾ��������
        //Show_Str(50,2,lcddev.width,24,"HomeAssistant",24,1);
        LCD_ShowString(50,5,lcddev.width,24,24,"HomeAssistant");
		//Show_Str(2,2,lcddev.width,16,pname,16,1); 				//��ʾͼƬ����
        //delay_ms(1000);
        //LCD_Clear(WHITE);
		t=0;
		while(1) 
		{
			key=KEY_Scan(0);		//ɨ�谴��
			//if(t>250)key=1;			//ģ��һ�ΰ���KEY0 
            if((t%40)==0)LED0=!LED0;//LED0��˸,��ʾ������������
            LED1=1;
            /*
			if((t%10)==0)
            {
                dht11_read_data(&temperature, &humidity);             // ��ȡ��ʪ��ֵ 
                adcx = lsens_get_val();                               // ��ȡADCֵ 
            }
            */
			if(key==WKUP_PRES)		//��ҳ
			{
                LCD_Clear(WHITE);
                POINT_COLOR=BRRED;
                Show_Str(90,5,200,24,"Ŀ¼",24,0);
                POINT_COLOR=RED;
                Show_Str(20,40,200,16,"��ʪ��",16,0);
                Show_Str(20,70,200,16,"����ǿ��",16,0);
                Show_Str(20,100,200,16,"��������",16,0);
                Show_Str(20,130,200,16,"��Ʒ���",16,0);
                Show_Str(20,160,200,16,"����",16,0);
                Show_Str(20,190,200,16,"���ʲ���",16,0);
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
            if(key==KEY0_PRES)     //ȷ��
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
                    LCD_ShowNum(30 + 40, 180, temperature, 2, 16); /* ��ʾ�¶� */
                    POINT_COLOR=BLUE;
                    LCD_ShowNum(160 + 40, 180, humidity, 2, 16);    /* ��ʾʪ�� */
                    
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
                    LCD_ShowxNum(30 + 10 * 8, 200, adcx, 3, 16, 0);  /* ��ʾADC��ֵ */
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
	myfree(SRAMIN,picfileinfo);			//�ͷ��ڴ�						   		    
	myfree(SRAMIN,pname);				//�ͷ��ڴ�			    
	myfree(SRAMIN,picoffsettbl);		//�ͷ��ڴ�	
}

