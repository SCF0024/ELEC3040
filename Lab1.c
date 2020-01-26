/*====================================================*/
/* Stephen Fortner and Tivon Evans*/
/* ELEC 3040/3050 - Lab 1, Program 1 */
/* Toggle LED1 while button pressed, with short delay inserted */
/*====================================================*/


#include "STM32L1xx.h" 						//Microcontroller information

/* Define global variables */
int toggles; 							//number of times LED state toggled


/*---------------------------------------------------*/
/* Initialize GPIO pins used in the program */
/* PA0 = push button */
/* PC8 = blue LED, PC9 = green LED */
/*---------------------------------------------------*/

void PinSetup () {
 /* Configure PA0 as input pin to read push button */
 RCC->AHBENR |= 0x01; 						//Enable GPIOA clock (bit 0)
 GPIOA->MODER &= ~(0x00000003); 				//General purpose input mode
	
 /* Configure PC8,PC9 as output pins to drive LEDs */
 RCC->AHBENR |= 0x04; 						//Enable GPIOC clock (bit 2)
 GPIOC->MODER &= ~(0x000F0000); 				//Clear PC9-PC8 mode bits
 GPIOC->MODER |= (0x00050000); 					//General purpose output mode
}
/*----------------------------------------------------------*/
/* Delay function - do nothing for about 1 second */
/*----------------------------------------------------------*/

void delay () {
 int i,j,n;
 for (i=0; i<20; i++) { 					//outer loop
	for (j=0; j<20000; j++) { 				//inner loop
		n = j; 						//dummy operation for single-step test
	} 							//do nothing
 }
}
/*------------------------------------------------*/
/* Main program */
/*------------------------------------------------*/

int main(void) {
 unsigned char sw1; 						//state of SW1
 unsigned char led1; 						//state of LED1
 PinSetup();					  		//Configure GPIO pins
 led1 = 0;		 					//Initial LED state
 toggles = 0; 							//#times LED state changed

 /* Endless loop */
 while (1) {
		if (led1 == 0) {				//LED off?
			GPIOC->BSRR = 0x0100 << 16; 		//Reset PC8=0 to turn OFF blue LED (in BSRR upper half)
			GPIOC->BSRR = 0x0200; 			//Set PC9=1 to turn ON blue LED (in BSRR lower half)
}
		else {						//LED on
			GPIOC->BSRR = 0x0100; 			//Set PC8=1 to turn ON blue LED (in BSRR lower half)
			GPIOC->BSRR = 0x0200 << 16; 		//Reset PC9=0 to turn OFF blue LED (in BSRR upper half)
		}
		sw1 = GPIOA->IDR & 0x01; 			//Read GPIOA and mask all but bit 0

		/* Wait in loop until SW1 pressed */
		while (sw1 == 0) { 				//Wait for SW1 = 1 on PE0
		sw1 = GPIOA->IDR & 0x01; 			//Read GPIOA and mask all but bit 0
		}
		delay(); 					//Time delay for button release
		led1 = ~led1; 					//Complement LED1 state
toggles++; 							//Increment #times LED toggled
 }/* repeat forever */
}
