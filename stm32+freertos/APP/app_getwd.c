#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "AD.h"

uint16_t ADValue , t;			//定义AD值变量
float Voltage;				//定义电压变量

#define T25 298.15    //电压转温度的公式的采用
#define R25 10
#define B	3435

//模拟量转电阻
float temp_Get_R(u16 adct)
{
	//单片机3.3v 供电 ==》 3v，先求出电压
	float v1 = (float)(adct*3.3)/4096;//ntc上的电压
	float v2 = 3.3 - v1;
	float r = (v1/v2)*10;
	
	return r;
}



//实现ln(x) 的计算
double myln(double a)
{
   int N = 15;//取了前15+1项来估算
   int k,nk;
   double x,xx,y;
   x = (a-1)/(a+1);
   xx = x*x;
   nk = 2*N+1;
   y = 1.0/nk;
   for(k=N;k>0;k--)
   {
     nk = nk - 2;
     y = 1.0/nk+xx*y;
     
   }
   return 2.0*x*y;
}
 
float Get_Kelvin_Temperature(u16 t)
{
	float N1,N2,N3,N4;
	float Rntc = temp_Get_R(t);
	N1 = (myln(R25)-myln(Rntc))/B;
	N2 = 1/T25 - N1;
	N3 = 1/N2;
	N4 = N3-273.15;//开尔文转摄氏度
	
	return N4;
}


uint16_t getwd(void)
{			
	
	ADValue = AD_GetValue();					
	
	float temp2 = Get_Kelvin_Temperature(ADValue);
	t = (u16)temp2;
	
	return t;
}

void display_wd(void)
{
	OLED_ShowString(1,1,"Tempture is ");
	OLED_ShowNum(2,7,t,4);
	//OLED_ShowString(1, 1, "ADValue:");
	//OLED_Clear();
}

