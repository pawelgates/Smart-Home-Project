
// HC05 Bluetooth module
// pin1 : KEY   N.C
// pin2 : VCC   to Vcc +5V
// pin3 : GND   to GND
// pin4 : TXD   to NUC140 UART0-RX (GPB0)
// pin5 : RXD   to NUC140 UART0-TX (GPB1)
// pin6 : STATE N.C.

#include <stdio.h>
#include "NUC1xx.h"
#include "Driver\DrvUART.h"
#include "Driver\DrvGPIO.h"
#include "Driver\DrvSYS.h"
#include "NUC1xx-LB_002\LCD_Driver.h"
#include <string.h>
#include "Scankey.h"

#define DATASIZE 5

int number;
int i;
int pincode[] = {0, 0, 0, 0};
int exit1;
int selected;
char pin[4] = "    ";
int password[] = {1, 5, 1, 5};
int timer = 0;
char TIME[8] = "        ";
int door_status = 0;

//BUZZER Activation
void buzzer_activate(){
	
	DrvGPIO_ClrBit(E_GPB,11); 
	DrvSYS_Delay(500000);	   
	DrvGPIO_SetBit(E_GPB,11); 
	DrvSYS_Delay(500000);	
	DrvGPIO_ClrBit(E_GPB,11); 
	DrvSYS_Delay(500000);	    
	DrvGPIO_SetBit(E_GPB,11); 
	DrvSYS_Delay(500000);	 	
	DrvGPIO_ClrBit(E_GPB,11); 
	DrvSYS_Delay(500000);	   
	DrvGPIO_SetBit(E_GPB,11); 
}

//TIMER0 Initialisation
void InitTIMER0(void){
	       
	SYSCLK->CLKSEL1.TMR0_S = 0;
  	SYSCLK->APBCLK.TMR0_EN =1;
	TIMER0->TCSR.MODE=1;
	TIMER0->TCSR.PRESCALE=127;
	TIMER0->TCMPR = 46875;
																
	TIMER0->TCSR.IE = 1;
	TIMER0->TISR.TIF = 1;
	NVIC_EnableIRQ(TMR0_IRQn);
	
	TIMER0->TCSR.CRST = 1;
	TIMER0->TCSR.CEN = 1;			
}

void Init_LED(){
	
	//Pin set to output mode
	DrvGPIO_Open(E_GPC, 12, E_IO_OUTPUT); 
	DrvGPIO_Open(E_GPC, 13, E_IO_OUTPUT); 
	DrvGPIO_Open(E_GPC, 14, E_IO_OUTPUT); 
	DrvGPIO_Open(E_GPC, 15, E_IO_OUTPUT); 
	DrvGPIO_Open(E_GPA, 12, E_IO_OUTPUT); 
	DrvGPIO_Open(E_GPA, 13, E_IO_OUTPUT);
	DrvGPIO_Open(E_GPA, 14, E_IO_OUTPUT);
	
	//LEDs off
	DrvGPIO_SetBit(E_GPC, 12); 
	DrvGPIO_SetBit(E_GPC, 13); 
	DrvGPIO_SetBit(E_GPC, 14); 
	DrvGPIO_SetBit(E_GPC, 15); 
	DrvGPIO_SetBit(E_GPA, 12); 
	DrvGPIO_SetBit(E_GPA, 13); 
	DrvGPIO_SetBit(E_GPA, 14); 
}

//TIMER0 Interrupt Handler
void TMR0_IRQHandler(void){ 
	
	if(door_status == 1){
		timer++;
		sprintf(TIME,"%d ",timer); // display on LCD
		print_lcd(1, "                ");
		print_lcd(1, TIME);
		DrvGPIO_ClrBit(E_GPA, 13);
		if(timer > 20){
			timer = 0;
			door_status = 0;
			print_lcd(1, "                ");
			print_lcd(0, "Door Closed");	
		}
		
	}
	if(door_status == 0){
		DrvGPIO_SetBit(E_GPA, 13);
		timer++;
		sprintf(TIME,"%d ",timer); // display on LCD
		print_lcd(1, "                ");
		print_lcd(0, "Door Closed");
		if(timer > 5){
			TIMER0->TCSR.CEN = 0;
			timer = 0;
			door_status = 0;
			print_lcd(0, "                ");
			print_lcd(1, "Welcome Home    ");
		}
	}
	TIMER0->TISR.TIF =1;
	
	while(door_status == 0 && DrvGPIO_GetBit(E_GPC,14) == 1){
		buzzer_activate();
	}
	
}

//DOOR Activation function
void door(void){
	int j = 0;
	print_lcd(0, "                ");	
	print_lcd(1, "Enter Password");
	print_lcd(2, "                ");
	
	//Password scan
	for(i=0; i<4; i++){
		exit1 = 0;
		selected = 0;
		while(exit1 == 0){
			number = Scankey();
			if(number != 0){
				selected = number;
			}
			if(number == 0 && selected != 0){
				pincode[i] = selected;
				exit1 = 1;
			}
			sprintf(pin,"%d%d%d%d",pincode[0], pincode[1], pincode[2], pincode[3]);
			print_lcd(2, pin);
		}
	}					
	
	//Password check
	for(i=0; i<4; i++){
		if(pincode[i] == password[i]){
			j++;
		}
	}
	sprintf(pin,"%d",j);
	print_lcd(1, pin);
	
	//DOOR Status set
	if(j == 3){
		print_lcd(0, "Door Opened");
		print_lcd(3, "Correct");
		door_status = 1;
		pincode[0] = 0;
		pincode[1] = 0;
		pincode[2] = 0;
		pincode[3] = 0;
		
		TIMER0->TCSR.CEN = 1;
	}
	else{
		print_lcd(0, "Door Closed");
		print_lcd(3, "Not Correct");
		door_status = 0;
		pincode[0] = 0;
		pincode[1] = 0;
		pincode[2] = 0;
		pincode[3] = 0;
		
		TIMER0->TCSR.CEN = 1;
	}		
}

//GPIO GPC14 Interrupt Handler
void GPIOAB_INT_CallBack(uint32_t GPA_IntStatus, uint32_t GPB_IntStatus)
{
	while(door_status == 0 && DrvGPIO_GetBit(E_GPC,14) == 1){
				buzzer_activate();
	}
}

void GPIOCDE_INT_CallBack(uint32_t GPA_IntStatus, uint32_t GPB_IntStatus)
{
	while(door_status == 0 && DrvGPIO_GetBit(E_GPC,14) == 1){
				buzzer_activate();
	}
}

//MAIN 
int32_t main()
{
	uint8_t  read_buf[16] = "        ";
	uint8_t  write_buf[5] = "Hello";
	uint8_t write_byte[1];
    	char     TEXT[8]     = "        ";
    	STR_UART_T sParam;
	int led_state = 0;
	int rgb_state = 0;

	//Initialisation
    	UNLOCKREG();
    	DrvSYS_Open(48000000);
    	LOCKREG();
	Init_LED();
	OpenKeyPad();
	InitTIMER0();
  	TIMER0->TCSR.CEN = 0;
    	DrvGPIO_InitFunction(E_FUNC_UART0); // Set UART pins
	Initial_panel();                 // initialize LCD panel
  	clr_all_panel();                 // clear LCD panel					 
	
	//INTERUPT
	DrvGPIO_Open(E_GPC,14,E_IO_INPUT);
	DrvGPIO_EnableInt(E_GPC, 14, E_IO_RISING, E_MODE_EDGE);
	DrvGPIO_SetDebounceTime(5, 1);
	DrvGPIO_EnableDebounce(E_GPC, 14);
	DrvGPIO_SetIntCallback(GPIOAB_INT_CallBack, GPIOCDE_INT_CallBack);
		
	//UART Setting
	sParam.u32BaudRate 	= 9600;
	sParam.u8cDataBits 	= DRVUART_DATABITS_8;
	sParam.u8cStopBits 	= DRVUART_STOPBITS_1;
	sParam.u8cParity 	= DRVUART_PARITY_NONE;
	sParam.u8cRxTriggerLevel= DRVUART_FIFO_1BYTES;
	if(DrvUART_Open(UART_PORT0,&sParam) != E_SUCCESS);

	//Start settings	
	print_lcd(1, TEXT);
	DrvGPIO_ClrBit(E_GPB, 14);
	DrvGPIO_ClrBit(E_GPB, 15);
	DrvGPIO_ClrBit(E_GPC, 4);
    		   
	while(1) {
		print_lcd(0, "                ");
		print_lcd(1, "Welcome Home    ");	
		print_lcd(2, "                ");
		print_lcd(3, "                ");

		DrvUART_Read(UART_PORT0,read_buf,DATASIZE); 
		sprintf(TEXT,"%s",read_buf); // display on LCD
		print_lcd(0, TEXT);

		// LED 1
		if(strncmp(TEXT, "ld1on", 5) == 0){
				DrvGPIO_SetBit(E_GPB, 14); 
		}
		else if(strncmp(TEXT, "ld1of", 5) == 0){
				DrvGPIO_ClrBit(E_GPB, 14); 
		}

		// LED 2
		if(strncmp(TEXT, "ld2on", 5) == 0){
				DrvGPIO_SetBit(E_GPB, 15); 
		}
		else if(strncmp(TEXT, "ld2of", 5) == 0){
				DrvGPIO_ClrBit(E_GPB, 15); 
		}

		// FAN
		if(strncmp(TEXT, "fanon", 5) == 0){
				DrvGPIO_SetBit(E_GPC, 4); 
		}
		else if(strncmp(TEXT, "fanof", 5) == 0){
				DrvGPIO_ClrBit(E_GPC, 4); 
		}

		//DOOR
		if(strncmp(TEXT, "opend", 5) == 0){
				if(TIMER0->TCSR.CEN == 0){
					door();
				}
		}
	}
}


