/*====================================================*/
/* Stephen Fortner and Tivon Evans */
/* ELEC 3040/3050 - Lab 2, Program 1 */
/* Cuount up/down depending on specified direction */
/*====================================================*/


#include "STM32L1xx.h" 					//Microcontroller information

/* Define global variables */
unsigned char counter; 					//number currently displayed

/*---------------------------------------------------*/
/* Initialize GPIO pins used in the program */
/* PA1 = start/stop PA2 = direction */
/* PC[0,3] = output */
/*---------------------------------------------------*/

void PinSetup () {
 /* Configure PA1 and PA2 as input pin to read push button */
 RCC->AHBENR |= 0x01; 					//Enable GPIOA clock (bit 0)
 GPIOA->MODER &= ~(0x0000003c); 			//General purpose input mode
	
 /* Configure PC[0,3] as output pins to drive LEDs */
 RCC->AHBENR |= 0x04; 					//Enable GPIOC clock (bit 2)
 GPIOC->MODER &= ~(0x000000FF); 			//Clear PC[0,3] mode bits
 GPIOC->MODER |= (0x00000055); 				//General purpose output mode
}
/*----------------------------------------------------------*/
/* Delay function - do nothing for about .5 second */
/*----------------------------------------------------------*/

void delay () {
 int i,j,n;
 for (i=0; i<10; i++) { 				//outer loop
	for (j=0; j<20000; j++) { 			//inner loop
		n = j;					//dummy operation for single-step test
	} 						//do nothing
 }
}

/*------------------------------------------------*/
/* Count function - increment or decrement according to direction*/
/*------------------------------------------------*/

void count (unsigned char dir) {
	//if direction = 1 count down or if currently 0 go to 9
	if (!dir){
			counter = (counter + 1) % 10;
	}
	else {
			counter = (counter +(10 - 1)) % 10;
	}
	
	//display count information
	GPIOC ->BSRR |= (~counter & 0x0F) << 16; 	//clear bits
	GPIOC ->BSRR |= (counter & 0x0F);		//write bits
}

/*------------------------------------------------*/
/* Main program */
/*------------------------------------------------*/

int main(void) {
	PinSetup();					//Configure GPIO pins
	
	unsigned char direction = 0; 			//start/stop
	unsigned char enable = 0;			//direction
	
	/* Endless loop */
	while (1) {
		enable = GPIOA->IDR & 0x00000002; 	//Read GPIOA and mask all but bit 1
		direction = GPIOA->IDR & 0x00000004;	//Read GPIOA and mask all but bit 2
		if (enable != 0){
			count(direction);
		}
		delay(); 				//Time delay for counting
		
 }	/* repeat forever */
}
