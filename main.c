#include "stm32f4xx.h"

uint32_t msTicks;
 
void SysTick_Handler()
{
    if (msTicks != 0)msTicks--;
}

void DelayMs(uint32_t ms)
{
    msTicks = ms;
    while (msTicks);
}

void Send(u8 isCommand, u8 data) {
    if(isCommand) 
			GPIO_ResetBits(GPIOE,GPIO_Pin_7);
		else 
			GPIO_SetBits(GPIOE,GPIO_Pin_7);
    DelayMs(1);

    if((data&0x80)==0x80) 
			GPIO_SetBits(GPIOE,GPIO_Pin_15); 
		else 
			GPIO_ResetBits(GPIOE,GPIO_Pin_15); 
    if((data&0x40)==0x40) 
			GPIO_SetBits(GPIOE,GPIO_Pin_14); 
		else 
			GPIO_ResetBits(GPIOE,GPIO_Pin_14);
    if((data&0x20)==0x40) 
			GPIO_SetBits(GPIOE,GPIO_Pin_13); 
		else 
			GPIO_ResetBits(GPIOE,GPIO_Pin_13);
    if((data&0x10)==0x10) 
			GPIO_SetBits(GPIOE,GPIO_Pin_12); 
		else 
			GPIO_ResetBits(GPIOE,GPIO_Pin_12);

    GPIO_SetBits(GPIOE,GPIO_Pin_11);
    DelayMs(1);
		GPIO_ResetBits(GPIOE,GPIO_Pin_11);

    if((data&0x08)==0x08) 
			GPIO_SetBits(GPIOE,GPIO_Pin_15); 
		else 
			GPIO_ResetBits(GPIOE,GPIO_Pin_15); 
    if((data&0x04)==0x04) 
			GPIO_SetBits(GPIOE,GPIO_Pin_14); 
		else 
			GPIO_ResetBits(GPIOE,GPIO_Pin_14);
    if((data&0x02)==0x02) 
			GPIO_SetBits(GPIOE,GPIO_Pin_13); 
		else 
			GPIO_ResetBits(GPIOE,GPIO_Pin_13);
    if((data&0x01)==0x01) 
			GPIO_SetBits(GPIOE,GPIO_Pin_12); 
		else 
			GPIO_ResetBits(GPIOE,GPIO_Pin_12);

    GPIO_SetBits(GPIOE,GPIO_Pin_11);
    DelayMs(1);
		GPIO_ResetBits(GPIOE,GPIO_Pin_11);
}

void Command(u8 cmd) {
    Send(1, cmd);
		DelayMs(2);
}

void Data(const char chr) {
    Send(0, (u8)chr);
}

void String(char* str){
	for(;str!='\0';){
		Data(*(str++));
	}
}

void Cursor(u8 c, u8 r)
{
	Command(0x80|(c|r*0x40));
}

void Clear()
{
	Command(0x01);
}

void init(void){
	DelayMs(40);
	Command(0x02); 
	Command(0x28); 
	Command(0x28); 
	Command(0x0C);
	Command(0x01);
	Command(0x06);
}

uint8_t mode = 0;

void EXTI15_10_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line11) != RESET) {
		mode = 1;
		EXTI_ClearITPendingBit(EXTI_Line11);
	}
}

void Number(uint8_t* arr,int count,uint8_t isNegative){
    if(isNegative) Data('-');
    for(int i=count;i>=0;i--){
        Data(48+arr[i]);
    }
}

void UNumber(uint32_t val){
    uint8_t arr[10];
    int i=0;
    while(val>9){
        arr[i++]=val%10;
        val/=10;
        
    }
    arr[i]=val;
    Number(arr,i,0);
}

int main()
{
	SystemCoreClockUpdate();
  SysTick_Config(SystemCoreClock / 1000);
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | 
																GPIO_Pin_10 | 
																GPIO_Pin_11 | 
																GPIO_Pin_12 | 
																GPIO_Pin_13 | 
																GPIO_Pin_14 | 
																GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOE,GPIO_Pin_10);
	
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource11);

	EXTI_InitStruct.EXTI_Line = EXTI_Line11;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);
	
	RCC_AHB2PeriphResetCmd(RCC_AHB2Periph_RNG, ENABLE);
	RNG_Cmd(ENABLE);
	
	while(1){
		if(mode==1){
			mode = 0;
			u32 ch = RNG_GetRandomNumber();
			UNumber(ch);
		}
	}
}
