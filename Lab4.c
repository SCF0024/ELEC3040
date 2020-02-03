/*====================================================*/
/* Stephen Fortner and Tivon Evans */
/* ELEC 3040/3050 - Lab 2, Program 1 */
/* Cuount up/down depending on specified direction */
/*====================================================*/


#include "STM32L1xx.h" 						//Microcontroller information

/* Define global variables */
unsigned char counter1; 			//number currently displayed
unsigned char counter2;       //number currently displayed
unsigned char LED8;
unsigned char LED9;
unsigned char counter2direction; 					//direction

/*---------------------------------------------------*/
/* Initialize GPIO pins used in the program */
/* PA1 = start/stop PA2 = direction */
/* PC[0,3] = output */
/*---------------------------------------------------*/

void PinSetup () {
 /* Configure PA1 and PA2 as input pin to read push button */
 RCC->AHBENR |= 0x01; 						//Enable GPIOA clock (bit 0)
 GPIOA->MODER &= ~(0x0000000F); 	//General purpose input mode
	
 /* Configure PC[0,3] as output pins to drive LEDs */
 RCC->AHBENR |= 0x04; 						//Enable GPIOC clock (bit 2)
 GPIOC->MODER &= ~(0x000FFFFF); 	//Clear PC[0,3] mode bits
 GPIOC->MODER |= (0x00055555); 		//General purpose output mode
}

/*----------------------------------------------------------*/
/* set up interrupt method*/
/*----------------------------------------------------------*/
void interruptSetUp(){
	SYSCFG->EXTICR[0] &= 0xFFF0;
	
	SYSCFG->EXTICR[0] &= 0xFF0F;
	SYSCFG->EXTICR[0] |= 0x0010;
	
	EXTI->RTSR |= 0x0003;
	EXTI->IMR |= 0x0003;
	EXTI->PR |= 0x0003;
	
	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI1_IRQn);
	
	NVIC_ClearPendingIRQ(EXTI0_IRQn);
	NVIC_ClearPendingIRQ(EXTI1_IRQn);
}
/*----------------------------------------------------------*/
/* interrupt handler*/
/*----------------------------------------------------------*/
void EXTI0_IRQHandler () {
	EXTI->PR |= 0x0001;
	counter2direction = 0;
	LED8 = ~LED8;
	if(LED8) {
		GPIOC->BSRR = 0x0100; 	//Set PC8=1 to turn ON blue LED (in BSRR lower half)
	}
	else {
		GPIOC->BSRR = 0x0100 << 16; //Reset PC8=0 to turn OFF blue LED (in BSRR upper half)
	}
	NVIC_ClearPendingIRQ(EXTI0_IRQn);
}
/*----------------------------------------------------------*/
/* interrupt handler*/
/*----------------------------------------------------------*/
void EXT1_IRQHandler () {
	EXTI->PR |= 0x0002;
	counter2direction = 1;
	LED9 = ~LED9;
	if(LED9){
		GPIOC->BSRR = 0x0200; 	//Set PC9=1 to turn ON blue LED (in BSRR lower half)
	}
	else {
		GPIOC->BSRR = 0x0200 << 16; //Reset PC9=0 to turn OFF blue LED (in BSRR upper half)
	}
	NVIC_ClearPendingIRQ(EXTI1_IRQn);
	}
/*----------------------------------------------------------*/
/* Delay function - do nothing for about .5 second */
/*----------------------------------------------------------*/

void delay () {
 int i,j,n;
	i = j = n = 0;
 for (i=0; i<10; i++) { 		//outer loop
	for (j=0; j<20000; j++) { //inner loop
		n = j;									//dummy operation for single-step test
	} 												//do nothing
 }
}
/*----------------------------------------------------------*/
/* Delay function - do nothing for about .5 second */
/*----------------------------------------------------------*/

void smallDelay () {
 int j,n;
	j = n = 0;
	for (j=0; j<100000; j++) { //loop
		n = j;									//dummy operation for single-step test
	} 												//do nothing
}
/*------------------------------------------------*/
/* Count function - increment or decrement according to direction*/
/*------------------------------------------------*/

void count1 () {
	counter1 = (counter1 + 1) % 10;
}
/*------------------------------------------------*/
/* Count function - increment or decrement according to direction*/
/*------------------------------------------------*/

void count2 (unsigned char dir) {
	//if direction = 1 count down or if currently 0 go to 9
	if (dir){
		  counter2 = (counter2 + (10 - 1)) % 10;
	}
	else {
			counter2 = (counter2 + 1) % 10;
	}
}
/*------------------------------------------------*/
/* Count function - increment or decrement according to direction*/
/*------------------------------------------------*/

void updateLEDs () {
	//display count information
	GPIOC->BSRR |= (~counter1 & 0x0F) << 16;	//clear bits
	GPIOC->BSRR |=(counter1 & 0x0F);	//write bits
	
	GPIOC->BSRR |= 0xF0 << 16;	//cleat bits
	GPIOC->BSRR |= counter2 << 4;		// write bits
}

/*------------------------------------------------*/
/* Main program */
/*------------------------------------------------*/

int main(void) {
	PinSetup();					  			//Configure GPIO pins
	interruptSetUp();
	counter1 = 0;
	counter2 = 0;
	counter2direction = 1; 					//direction
	GPIOC->BSRR |= 0x0100 << 16;
	LED8 = 0;
	GPIOC->BSRR |= 0x0200;
	LED9 = 1;
	
	__enable_irq();
	
	/* Endless loop */
	while (1) {
		delay();
		count1();
		updateLEDs();
		delay(); 									//Time delay for counting
		count2(counter2direction);
		count1();
		updateLEDs();
 }														/* repeat forever */
}
