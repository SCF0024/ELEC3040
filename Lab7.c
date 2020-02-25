/*====================================================*/
/* Stephen Fortner and Tivon Evans */
/* ELEC 3040/3050 - Lab 7					 */
/*====================================================*/

//Microcontroller information
#include "STM32L1xx.h"

/*------------------------------------------------*/
/* Creates a structure for keypads */
/*------------------------------------------------*/
struct {
	int row;								//row that was pressed
	int column;							//column that was pressed
	unsigned char event;		//event has happened
	unsigned char value;		//value of key pressed
	//matrix key values
	const int row1[4];
	const int row2[4];
	const int row3[4];
	const int row4[4];
	const int* keys[];
} typedef matrix_keypad;

//functions used in the program
void PinSetup (void);
void interuptSetup(void);
void timerSetup(void);
void smallDelay (void);
void updateLEDs (void);
unsigned char count(unsigned char value);
int readColumn(void);
int readRow(void);

//definining keypad instance
matrix_keypad keypad = {
	.row = ~0,
	.column = ~0,
	.event = 0,
	.value = 0,
	.row1 = {1, 2, 3, 10},
	.row2 = {4, 5, 6, 11},
	.row3 = {7, 8, 9, 12},
	.row4 = {14, 0, 15, 13},
	.keys = {keypad.row1, keypad.row2, keypad.row3, keypad.row4},
};
/*------------------------------------------------*/
/* Main program */
/*------------------------------------------------*/

int main(void) {
	PinSetup();					  	//Configure GPIO pins
	interuptSetup();				//enable external intterupt
	timerSetup();						//enable internal intterupt
	__enable_irq();					//ready to accept interrupt
	
	
	/* Endless loop */
	while (1) {
		if (keypad.event == 1 && keypad.value < 11) {
			int debug = keypad.value * (TIM10 -> ARR + 1) / 10;
			TIM10 -> CCR1 = debug;
			keypad.event = 0;
		}
 }	/* repeat forever */
}

/*---------------------------------------------------*/
/* Initialize GPIO pins used in the program */
/*---------------------------------------------------*/

void PinSetup () {
	/* Configure PA1 for external interrupt input */
	RCC->AHBENR |= 0x01; 						//Enable GPIOA clock (bit 0)
	GPIOA->MODER &= ~(0x0000000C); 	//General purpose input mode
	
	/*GPIOB*/
	RCC->AHBENR |= 0x02;						//Enable GPIOB clock (bit1)
	GPIOB->MODER &= ~(0x0000FFFF);	//clear
	GPIOB->MODER |= (0x00005500);		//SET
	GPIOB->ODR = 0;
	GPIOB->PUPDR &= ~(0x000000FF);	//clear pu/pd
	GPIOB->PUPDR |= (0x00000055);		//pull up/ pull down
	
	/* Configure PC[0,7] as output pins to drive LEDs */
	RCC->AHBENR |= 0x04; 						//Enable GPIOC clock (bit 2)
	GPIOC->MODER &= ~(0x0000FFFF); 	//Clear
	GPIOC->MODER |= (0x00005555); 	//General purpose output mode

	/* Change PA6 to altrnative function mode */
  MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER6, 0x00002000);
  
  /* Change the alternatice function to be the CC*/
  MODIFY_REG(GPIOA->AFR[0], GPIO_AFRL_AFRL6, 0x03000000);
}
/*---------------------------------------------------*/
/* Initialize external interrupt */
/*---------------------------------------------------*/
void interuptSetup(){
	NVIC_EnableIRQ (EXTI1_IRQn);
	NVIC_ClearPendingIRQ (EXTI1_IRQn);
	
	NVIC_EnableIRQ (TIM10_IRQn);
	NVIC_ClearPendingIRQ (TIM10_IRQn);
	
	SYSCFG->EXTICR[0] &= 0xFF0F;
	SYSCFG->EXTICR[0] |= 0x0000;
	
	EXTI->FTSR |= 0x0002;
	EXTI->IMR |= 0x0002;
	EXTI->PR |= 0x0002;
}
/*---------------------------------------------------*/
/* Initialize internal timmer */
/*---------------------------------------------------*/
void timerSetup() {
	RCC->CR |= RCC_CR_HSION; // Turn on 16MHz HSI oscillator
  while ((RCC->CR & RCC_CR_HSIRDY) == 0); // Wait until HSI ready
  RCC->CFGR |= RCC_CFGR_SW_HSI; // Select HSI as system clock
  
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM10EN); //enable clock source
  TIM10->ARR = 99; //set auto reload. assumes 2MHz
  TIM10->PSC = 159; //set prescale.
  TIM10->CCR1 = 10; //Set compair value
  TIM10->CNT = 0;
  MODIFY_REG(TIM10->CCMR1, TIM_CCMR1_CC1S, 0x0000); // Capture compair select
  MODIFY_REG(TIM10->CCMR1, TIM_CCMR1_OC1M, 0x0060); // Active to inactive
  SET_BIT(TIM10->CCER, TIM_CCER_CC1E); // drive output pin 
  SET_BIT(TIM10->CR1, TIM_CR1_CEN); //enable counting
}
/*----------------------------------------------------------*/
/* External interrupt handler*/
/*----------------------------------------------------------*/
void EXTI1_IRQHandler () {
	EXTI->PR |= 0x0002;
	
	keypad.row = readRow();
	keypad.column = readColumn();
	if ((keypad.row != -1) && (keypad.column != -1)) {
		smallDelay();
		keypad.value = keypad.keys[keypad.row][keypad.column];
		keypad.event = 1;
		MODIFY_REG(GPIOC->ODR, 0x000F, keypad.value);
	}
	
	RCC->AHBENR |= 0x02; // Sets up the Keypad
	GPIOB->MODER &= ~(0x0000FFFF);
	GPIOB->PUPDR &= ~(0x0000FF00);
	GPIOB->PUPDR |= (0x00005500);
	
	GPIOB->MODER |= (0x00005500);
	GPIOB->ODR = 0;
	GPIOB->PUPDR &= ~(0x000000FF);
	GPIOB->PUPDR |= (0x00000055);
	NVIC_ClearPendingIRQ(EXTI1_IRQn);
	EXTI->PR |= 0x0002;
}

/*----------------------------------------------------------*/
/* Delay function - do nothing for a short time */
/*----------------------------------------------------------*/
void smallDelay() {
  int i;
  for (i=0; i<10; i++) {
    asm("nop");
  }
}

/*---------------------------------------------------*/
/* Read column */
/*---------------------------------------------------*/
int readColumn() {
	GPIOB->MODER &= ~(0x0000FFFF);
	GPIOB->MODER |= (0x00000055);
	GPIOB->ODR = 0;
	GPIOB->PUPDR &= ~(0x0000FF00);
	GPIOB->PUPDR |= (0x00005500);
	int something1 = 4;
	
	while (something1 > 0) {
		something1 --;
	}
	int input = GPIOB ->IDR&0xF0;
	switch(input) {
		case 0xE0:
			return 0;
		case 0xD0:
			return 1;
		case 0xB0:
			return 2;
		case 0x70:
			return 3;
		default:
			return -1;
	}
}
/*---------------------------------------------------*/
/* Read row */
/*---------------------------------------------------*/
int readRow() {
	GPIOB->MODER &= ~(0x0000FFFF);
	GPIOB->MODER |= (0x00005500);
	GPIOB->ODR = 0;
	GPIOB->PUPDR &= ~(0x000000FF);
	GPIOB->PUPDR |= (0x00000055);
	int something2 = 4;
	
	while (something2 >4){
		something2--;
	}
	int input = GPIOB->IDR&0xF;
	switch(input) {
		case 0xE:
			return 0;
		case 0xD:
			return 1;
		case 0xB:
			return 2;
		case 0x7:
			return 3;
		default:
			return -1;
	}
}