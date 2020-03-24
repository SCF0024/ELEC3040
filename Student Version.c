//03.19.2020
//ELEC 3040/3050
//Lab 10 (Special)

//Read analog input on pin PA5 and convert using ADC

//Include statements
#include "STM32L1xx.h" 										//Microcontroller info

//Global variables
static int   adc_out;											//Global: analog voltage
static int   adc_values[100];							//Global: analog voltage array
static int   adc_index = 0;								//Global: adc_values index

//Initialization subroutines
void ClockSetup()													//Enable HSI Clock
{	
	RCC->CR |= RCC_CR_HSION;								//Turn on 16MHz HSI oscillator
	while ((RCC->CR & RCC_CR_HSIRDY) == 0); //Wait for HSI to be ready
}

void PinSetup() 													//Configure Pin PA5
{
	SET_BIT (RCC->AHBENR, RCC_AHBENR_GPIOAEN);//Enable GPIOA clock									//EDIT
	GPIOA->MODER |= GPIO_MODER_MODER5;  		//Set PA5 to analog mode								//EDIT
}

void ADCSetup() 													//Configure ADC
{
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_ADC1EN);//Enable ADC1 clock										//EDIT
	ADC1->SQR5		&= ~(0x1F);								//Clear ADC1 sequence register
	ADC1->SQR5 |= 0x00000005;								//Set ADC1 to channel 5									//EDIT
	SET_BIT(ADC1->CR2, ADC_CR2_ADON); 			//Turn ADC1 on													//EDIT						
	while ((ADC1->SR & ADC_SR_ADONS) == 0); //Wait for ADC1 to be ready							//EDIT
}

void TimerSetup()													//Configure Timer 11 for 1kHz Sampling
{
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_ADC1EN);//Enable TIM11 clock										//EDIT
	TIM11->PSC = 99;										  	//Set prescaler value										//EDIT
	TIM11->ARR = 150;									    	//Set auto-reload value									//EDIT
	TIM11->CNT = 0; 												//Initialize counter value
	TIM11->SR &=~TIM_SR_UIF;								//Clear TIM11 pending flag							//EDIT
	TIM11->DIER |= TIM_DIER_UIE;						//Enable TIM11 interrupts								//EDIT
	TIM11->CR1 |= TIM_CR1_CEN;							//Enable timer counting									//EDIT
	NVIC_EnableIRQ(TIM11_IRQn);							//Enable TIM11 NVIC interrupts					//EDIT
	NVIC_ClearPendingIRQ(TIM11_IRQn);				//Clear TIM11 NVIC pending interrupts   //EDIT
}

//Interrupt handlers
void TIM11_IRQHandler()
{
	ADC1->CR2	|= ADC_CR2_SWSTART;						//Start conversion											//EDIT
	while((ADC1->SR & ADC_SR_EOC) == 0);	  //Wait for conversion to complete				//EDIT
	adc_out = (ADC1->DR & 0xFFFF);					//Read individual ADC result						//EDIT
	if (adc_index < 100)									  //Check for 100 samples									//EDIT
	{
		  adc_values[adc_index] = adc_out;    //Write results to output array					//EDIT
	}
	TIM11->SR &= ~TIM_SR_UIF;								//Clear TIM11 pending flag     					//EDIT
	NVIC_ClearPendingIRQ(TIM11_IRQn);		 		//Clear TIM11 NVIC pending interrupts 	//EDIT
}

//Main program
int main(void) 														//Main program
{
	ClockSetup();														//Enable HSI clock
	PinSetup(); 														//Configure GPIO pins
	ADCSetup();															//Configure ADC
	TimerSetup();														//Configure timer
	__enable_irq();													//CPU interrupt enable									//EDIT
	while(1);																//Endless loop (do nothing)
}
