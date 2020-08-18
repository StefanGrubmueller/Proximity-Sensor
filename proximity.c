/************************************************************************
* proximity.c																														*
* Grubmüller Stefan, Marx Clemens																				*
* May 2020																															*
* 																																			*
* Function: In this file you can find any functions used in the main		*
* file.																																	*
*																																				*
* More information in the scritum by @JosefReisinger and in the 				*
* Specifications by @ClemensMarx and @StefanGrubmueller or in the				*	
* datasheet by @sparkfun:																								*
* https://cdn.sparkfun.com/datasheets/Sensors/Proximity/apds9960.pdf		*
************************************************************************/

/* ---------------------------Includes --------------------------------*/ 
#include "proximity.h"


/*----------------------- Static Variables ----------------------------*/
int h, min, sek, milsek;	// used for the timer
char buffer[30];

I2C_PIN_CONF SCL =			// defenition of SCL (I2C1)
{
	.GPIOx = GPIOB,
	.Pin = 6							// Pin PB6
};

I2C_PIN_CONF SDA =			// defenition of SDA (I2C1)
{
	.GPIOx = GPIOB,
	.Pin = 7							// Pin PB7
};

I2C_Device device =
{
	.Adress = 0x39,				// hardware adress = 0x39
	.I2C_Pereph = I2C1,		// I2C1 peripherals
	.pclr = 8,						// 36 MHz perepherial clock rate
	.sclr = 80,						// 80 kHz SCL Clock Rate (0 - 400kHZ)
	.maxRiseTime = 200		// 200ns max rise time (0 - 300ns)
};




/* ----------------------- Initalitations -----------------------------*/

// init GPIO ports PB6 and PB7 for I2C
void InitI2CPorts(void)
{
	// activate GPIO clock
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

	// init PB6 (SCL) as AF Open Drain
	int tmp = GPIOB->CRL;
	tmp &= 0xF0FFFFFF;			// delete configuration for PB6
	tmp |= 0x0F000000;			// define port PB6 as AF Open Drain
	GPIOB->CRL = tmp;
	
	// init PB7 (SDA) as AF Open Drain
	tmp = GPIOB->CRL;
	tmp &= 0x0FFFFFFF;			// delete configuration for PB7 
	tmp |= 0xF0000000;			// define Port PB7 as AF Open Drain
	GPIOB->CRL = tmp;
}




/* -------------------------- Interrupts -------------------------------*/

// interrupt service routine timer3 (General Purpose Timer) 
void TIM3_IRQHandler(void)	//Timer 3, very 100ms 
{
	TIM3->SR &=~0x01;	// clear interrupt pending bit(precent Interrupt-trigger)
	milsek = milsek + 2;
}


// External Interrupt Service Routine PA1                  
void EXTI1_IRQHandler(void)//ISR
{
	EXTI->PR |= (0x01 << 1); // reset pending bit EXT0 (otherwise ISR will repeat) 
	
	lcd_set_cursor(0,0);            
	sprintf(&buffer[0],"IN RANGE"); 							// if the object is in range
	lcd_put_string(&buffer[0]);
	
	char en_reg[2] = {ENABLE_REG, DEL_PIEN};			// disable proximity interupt		
	i2c_write(&device, en_reg, 2, END_WITH_STOP); // set enable register
}  

// initialisation Timer3 (General Purpose Timer)     
void TIM3_Config(void)
{	
	/*-------------------------- configuration timer 3 -------------------------*/
	RCC->APB1ENR |= 0x0002;	// timer 3 clock enable
	TIM3->SMCR = 0x0000;		// timer 3 clock Selection: CK_INT wird verwendet
	TIM3->CR1  = 0x0000;		// selcet timer mode: Upcounter --> counts 0 to value of autoreload-register

	// Tck_INT = 27,78ns, Presc = 54 ---> Auto Reload value = 3,6Mio (=0xFFFF) --> 0,1s Update Event
	TIM3->PSC = 23;				//value of prescalers (Taktverminderung)
	TIM3->ARR = 0xFFFF;		//Auto-Reload Wert = Maximaler Zaehlerstand des Upcounters

	/*-------------------- configuration Interrupt Timer 3  --------------------*/
	TIM3->DIER = 0x01;	   			// enable Interrupt bei einem UEV (Überlauf / Unterlauf)
	NVIC_init(TIM3_IRQn,3);	   	// enable Timer 3 Update Interrupt, Priority 3

	/*---------------------------- start Timer 3  -------------------------------*/
  TIM3->CR1 |= 0x0001;   			// set counter-Enable bit 
}


// EXTI1_config                              
// connecct PA1 with EXTI1, Interrupt at falling edge, priority 2
void EXTI_config(pin p, int n)
{
	NVIC_init(n + 6, 2);		//init NVIC for EXTI Line1 (Position n+6, Priority 2)

  RCC->APB2ENR |= 0x0001;		   //AFIOEN  - Clock enable
	AFIO->EXTICR[0] &= ~((0xF & ~p) << (4 * n)); //Interrupt-Line EXTIn mit Portpin PAn verbinden
	// ...
	EXTI->FTSR |= (0x01 << n);	   //Falling Edge Trigger für EXITn Aktivieren
  EXTI->RTSR &= ~(0x01 << n);	   //Rising Edge Trigger für EXTIn Deaktivieren

	EXTI->PR |= (0x01 << n);	//EXTI_clear_pending: Das Auslösen auf vergangene Vorgänge nach	dem enablen verhindern
	EXTI->IMR |= (0x01 << n);   // Enable Interrupt EXTIn-Line. Kann durch den NVIC jedoch noch maskiert werden
}


// NVIC_init(char position, char priority)    			   
// initialisation of an interrupts in the Nested Vectored Interrupt  
// Controller (set priority, prevent trigger, enable interrupt                                                          
// parameters: "position" = 0-67 (number of interrupt)               
//             "priority" = 0-15 (priority of interrupt)		      
void NVIC_init(char position, char priority) 
{	
	NVIC->IP[position]=(priority<<4);	//Interrupt priority register: Setzen der Interrupt Priorität
	//Interrupt Clear Pendig Register: prevent trigger after enable
	NVIC->ICPR[position >> 0x05] |= (0x01 << (position & 0x1F));
	//Interrupt Set Enable Register: Enable interrupt
	NVIC->ISER[position >> 0x05] |= (0x01 << (position & 0x1F));
} 

/* ---------------------------- functions ---------------------------------*/ 

// after 0.1 seconds an interrupt is being triggered (milsek ++)  
// function prints output real clock on lcd in hh:mm:ss:z	 (hour:minute:second:millisecond)							
void clock_lcd(void)
{
	if(milsek==10)
	{
		milsek=0;
		if(++sek==60)
		{
			sek=0;
			if(++min==60)
			{
				min=0;
				if(++h==24)
				{
					h=0;
				}
			}
		}
	}
	else if(milsek>=10)		// error detection
	{
		milsek=0;
	}
	
	// output of lcd
	sprintf(&buffer[0], "%02d:%02d:%02d:%d", h, min, sek, milsek);
	lcd_set_cursor(1,0);
	lcd_put_string(&buffer[0]);
}

void check_device_con()
{
	// check if device is there and connected
	char ack;
	int i;
	char dev_con[] = {"DEVICE CONNECTED"};								// device connected
	char dev_discon[] = {"NOT CONNECTED"};				// device disconnected
	char buffer[1] = {0x1};
	
	i2c_write(&device, buffer, 1, END_WITH_STOP);	// write bit to test gettin ack for checking connection
	ack = SDA_IN;					// read one bit of data pin
	lcd_set_cursor(1,0);	// read acknowledge
	if (ack == 0x0)				// if there is an acknowledge
	{
		// output
		for(i=0; dev_con[i] != '\0'; i++)
		{
			lcd_put_char(dev_con[i]);									// device connected
		}
	}
	else									// if no ack detected
	{
		// output
		for(i=0; dev_discon[i] != '\0'; i++)
		{
			lcd_put_char(dev_discon[i]);							// device disconnected
		}
	}
}


// start proximity engine (configuration of registers)
void start_proximity_engine()
{
	
	// Enable Register (0x80):																								
	// 00000101 (0x05)																												
	// Bit 7 = 0: is being reserved as 0																			
	// Bit 6 = 0: gesture enable (GEN)																				
	// Bit 5 = 0: proximity interrupt enable (PIEN)													
	// Bit 4 = 0: ambient light sense (ALS) interrupt enable (AIEN)					
	// Bit 3 = 0: wait enable (WEN) actives wait feature										
	// Bit 2 = 1: proximity detect enable (PEN)															
	// Bit 1 = 0: ALS enable (AEN)																						
	// Bit 0 = 1: Power ON (PON)																							
	char en_reg[2] = {ENABLE_REG, 0x05};
	i2c_write(&device, en_reg, 2, END_WITH_STOP); // set enable register
	
	// Persistance Register (0x8C):																					
	// 00000000 (0x0)																															
	// Bit 7 : 4 = 0: Controls rate of proximity interrupt to host process		
	// Bit 3	: 0 = 0: Controls rate of clear channel interrupt to host process
	char pers_reg[2] = {PERS_REG, 0x00};
	i2c_write(&device, pers_reg, 2, END_WITH_STOP);
	
	
	// Proximity Interrupt Threshold Register - Low (0x89):																					
	// set as define LOWTHRES in proximity.h 																														
	// Bit 7 : 0 = ?? (adjustable)
	char low_threshold[2] = {LOWTHRES_REG, 0x00};
	i2c_write(&device, low_threshold, 2, END_WITH_STOP);
	
	
	// Proximity Interrupt Threshold Register - High (0x8B):																					
	// set as define HIGHTHRES in proximity.h 																															
	// Bit 7 : 0 = ?? (adjustable)
	char high_threshold[2] = {HIGHTHRES_REG, HIGHTHRES};
	i2c_write(&device, high_threshold, 2, END_WITH_STOP);
	
	
	// Proximity Pulse Register (0x8E):																			
	// 01111111 (7F)																													
	// Bit 7 : 6 = 01																											
	// Bit 5 : 0 = 1: 111111
	char prox_pulse_reg[2] = {PROX_PULSE_REG, 0x7F};
	i2c_write(&device, prox_pulse_reg, 2, END_WITH_STOP); 
	
	
	//	Control Register One(0x8F):																						
	//	0000??00 																										
	//	Bit 7 : 6 = 10																											
	//	Bit 5 : 4 = reserved as 0																							
	//	Bit 3 : 2 = LIGHT_INTENSITY set in defines																										
	//	Bit 1 : 0 = 0	
	char control_reg1[2] = {CONTROL_REG1, GAIN_x2};
	i2c_write(&device, control_reg1, 2, END_WITH_STOP);

	//	Control Register Two(0x90):																						
	// 10??0000 																												
	// Bit 7 = 1																															
	// Bit 6 = 0																															
	// Bit 5 : 4 = LED_BOOST_ON/LED_BOOST_OFF																										
	// Bit 3 : 0 = reserved as 0			
	char control_reg2[2] = {CONTROL_REG2, LED_BOOST_OFF};
	i2c_write(&device, control_reg2, 2, END_WITH_STOP);	
	
	
	// Status Register(0x93):																								
	// 01100010 (0x62)												
	// The read-only Status Register provides the status of the device. 
	// Bit 7 = 0	Clear Photodiode Saturation (CPSAT)																														
	// Bit 6 = 1	Indicates that an analog saturation event occurred (PGSAT)																														
	// Bit 5 = 1	Proximity Interrupt. This bit triggers an interrupt if PIEN in ENABLE is set (PINT)																														
	// Bit 4 = 0	ALS Interrupt (AINT)																														
	// Bit 3 = 		reserved as 0																									
	// Bit 2 = 0	Gesture Interrupt (GINT)																														
	// Bit 1 = 1	Proximity Valid (PVALID)																														
	// Bit 0 = 0	ALS Valid (AVALID)																											
	char stat_reg[2] = {STAT_REG, 0x62};
	i2c_write(&device, stat_reg, 2, END_WITH_STOP);	
	
	
	// Proximity Offset UP / RIGHT Register(0x9D)														
	// 00000000 (0x00)																												
	// Bit 7 : 0 = 0x0																													
	char ur_offset_reg[2] = {UPRIGHT_OFFSET_REG, 0x00};
	i2c_write(&device, ur_offset_reg, 2, END_WITH_STOP);	
	
	
	// Proximity Offset DOWN / LEFT Register(0x9E)														
	// 00000000 (0x00)																												
	// Bit 7 : 0 = 0x0																													
	char dl_offset_reg[2] = {DLEFT_OFFSET_REG, 0x00};
	i2c_write(&device, dl_offset_reg, 2, END_WITH_STOP);	
	
	
	//Configuration Register Three(0x9F):																	  
	// 00000000 (0x00)			
	// select which photodiodes are used for proximity
	// Bit 7 : 6 = reserved as 0																							
	// Bit 5 = 0 use all diodes																														
	// Bit 4 = 0 sleep after interrupt																														
	// Bit 3 = 0 Proximity Mask UP Enable																														
	// Bit 2 = 0 Proximity Mask LEFT Enable																															
	// Bit 1 = 0 Proximity Mask LEFT Enable																														
	// Bit 0 = 0 Proximity Mask RIGHT Enable																														
	char conf_reg3[2] = {CONF_REG3, 0x00};
	i2c_write(&device, conf_reg3, 2, END_WITH_STOP);


//	// Proximity Interrupt Clear (0xE5):																	  	
//	// 00000000 (0x00)																												
//	// Bit 7 : 0 = 0x0																																
//	char prox_int_clear[2] = {PROX_INT_CLEAR, 0x00};
//	i2c_write(&device, prox_int_clear, 2, END_WITH_STOP);	


//	// Clear All Non-Gesture Interrupts (0xE7):																	  	
//	// 00000000 (0x00)																												
//	// Bit 7 : 0 = 0x0																																
//	char clear_all_int[2] = {CLEAR_ALL_INT, 0x00};
//	i2c_write(&device, clear_all_int, 2, END_WITH_STOP);		
}
