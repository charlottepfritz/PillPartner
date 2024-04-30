/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#include "main.h"

#include "dfr0151.h"
#include "config.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "liquidcrystal_i2c.h"

//Initialize fields
uint8_t hour = 0;
uint8_t min = 0;
uint8_t sec = 0;
	
uint8_t week_day = 4;
uint8_t day = 0x0F;
uint8_t month = 10;
uint8_t year = 3;

uint8_t alarm_hour = -1;
uint8_t alarm_min = -1;
uint8_t alarm_sec = -1;

uint8_t alarm_wkday = -1;
uint8_t alarm_day = -1;
uint8_t alarm_month = -1;
uint8_t alarm_year = -1;

uint8_t updatedAlarmFlag = 0;

uint8_t motorRotateVal = 55;

//Button Interrupts
int8_t current_row = -1, current_col = -1;
int8_t column = -1;
int8_t prev_column = 0;

void print_LCD(char * msg);
void print_LCD_scroll(char * msg);

void print_LCD_bottom(char * msg);
void print_LCD_top(char * msg);

int main(void)
{
  /* Reset of all peripherals. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();  
  //rtc_init();

  char msg[100];

  // Your start up code here
	
	//READ THE EEPROM AND WRITE TO THE TIME AND DATE
	
	/* Initial Setting */
	
	hour = alarm_hour = 0;
	min = 0;
	alarm_min = min + 1;
	sec = alarm_sec = 0;
	
	week_day = alarm_wkday = 0;
	day = alarm_day = 0;
	month = alarm_month = 0;
	year = alarm_year = 0;
	
	  /* USER CODE BEGIN 2 */
    HD44780_Init(2);
    HD44780_Clear();
    HD44780_SetCursor(0,0);
    HD44780_PrintStr("pillpartner");
    HD44780_SetCursor(6,1);
    HD44780_PrintStr("WELCOME!");
    HAL_Delay(2000);

	
	//Write to eeprom initially
	
	eeprom_write(0x0,&sec,1);
	eeprom_write(0x1,&min,1);
	eeprom_write(0x2,&hour,1);
	eeprom_write(0x3,&week_day,1);
	eeprom_write(0x4,&day,1);
	eeprom_write(0x5,&month,1);
	eeprom_write(0x6,&year,1);
	
	eeprom_write(0x7,&alarm_sec,1);
	eeprom_write(0x8,&alarm_min,1);
	eeprom_write(0x9,&alarm_hour,1);
	eeprom_write(0xA,&alarm_wkday,1);
	eeprom_write(0xB,&alarm_day,1);
	eeprom_write(0xC,&alarm_month,1);
	eeprom_write(0xD,&alarm_year,1);
	
	//Read from the eeprom initially
	sec = eeprom_read(0x0);
	min = eeprom_read(0x1);
	hour = eeprom_read(0x2);
	week_day = eeprom_read(0x3);
	day = eeprom_read(0x4);
	month = eeprom_read(0x5);
	year = eeprom_read(0x6);
	
	alarm_sec = eeprom_read(0x7);
	alarm_min = eeprom_read(0x8);
	alarm_hour = eeprom_read(0x9);
	alarm_wkday = eeprom_read(0xA);
	alarm_day = eeprom_read(0xB);
	alarm_month = eeprom_read(0xC);
	alarm_year = eeprom_read(0xD);
	
	//Set the initial date and time
	rtc_set_date(week_day, day, month, year);
	rtc_set_time(hour, min, sec);
	//sprintf(msg, "okay");
//	print_msg(msg);
	
	//Print initially
	//sprintf(msg, "INITIAL TIME:hour: %02d | min: %02d | sec: %02d | week_day: %d | day: %d | month: %d | year: %d ", hour, min, sec, week_day, day, month, year);
	//print_LCD_scroll(msg);
	
	//Refill variables
	bool needRefill = true;
	int refillCounter = 0;
	
	//Just hit reset flag
	bool justReset = true;

	HAL_Delay(1);
	print_LCD("BOOTING UP");
	HAL_Delay(2000);
	
  while (1)
  {
		if (needRefill) {
			HAL_Delay(1);
			print_LCD_top("Refill required."); 
			HAL_Delay(2000);
			
			while (refillCounter < 14) {
				if(refillCounter == 5){
					motorRotateVal+=2;
				}
				HAL_Delay(1);
				sprintf(msg, "fill slot %d &", refillCounter + 1);
				//print_msg(msg);
				print_LCD_top(msg);
				
				sprintf(msg, "press interact");
				print_LCD_bottom(msg);
				
				current_col = -1;
				
				volatile int dummy = 0;
				while (current_col != 0) {
						dummy++;
						HAL_Delay(1);
				}
				
				current_col = -1;
				refillCounter++;
				
								HAL_Delay(1);
				
				HAL_GPIO_WritePin(ROW3_GPIO_Port, ROW3_Pin, GPIO_PIN_SET);
				
				HAL_Delay(1);
				
				//Since we pressed interact, let's actuate the motor to reveal the next slot
				HAL_GPIO_WritePin(ROW2_GPIO_Port, ROW2_Pin, GPIO_PIN_SET);
				for(int x=0; x<motorRotateVal; x=x+1)
				{
														
					HAL_GPIO_WritePin(ROW1_GPIO_Port, ROW1_Pin, GPIO_PIN_SET);
					HAL_Delay(1);
					HAL_GPIO_WritePin(ROW1_GPIO_Port, ROW1_Pin, GPIO_PIN_RESET);
					HAL_Delay(1);
					
					
				}
				
				HAL_GPIO_WritePin(ROW2_GPIO_Port, ROW2_Pin, GPIO_PIN_RESET);
				
				HAL_GPIO_WritePin(ROW3_GPIO_Port, ROW3_Pin, GPIO_PIN_RESET);
				
				HAL_Delay(1);
				
				if(refillCounter == 5){
					motorRotateVal-=2;
				}

			}
			


			
			HAL_Delay(1);
			print_LCD_top("Refill complete.");
			HAL_Delay(2000);

			refillCounter = 0;
			needRefill = false;
			
		}
		
		if (justReset) {
			HAL_Delay(1);
			print_LCD_top("set alarm time");
			print_LCD_bottom("& press interact");
			
			current_col = -1;
			
			while (current_col != 0) {
				if (current_col == 2) {
					hour = 0;
					min = 0;
					sec = 0;
					rtc_set_time(hour, min, sec);
					alarm_min++;
					
					if (alarm_min == 61) {
						alarm_min = 0;
					}
					
					HAL_Delay(10);
					print_LCD_top("Time Until Dose");
					sprintf(msg, "%02d : %02d", alarm_hour - hour, alarm_min - min);
					print_LCD_bottom(msg);	
					current_col = -1;
				}
				else if (current_col == 3) {
					hour = 0;
					min = 0;
					sec = 0;
					rtc_set_time(hour, min, sec);
					alarm_hour++;
					HAL_Delay(10);
					print_LCD_top("Time Until Dose");
					sprintf(msg, "%02d : %02d", alarm_hour - hour, alarm_min - min);
					print_LCD_bottom(msg);	
					current_col = -1;
				}
				
				HAL_Delay(200);
					
			}
			
			print_LCD_top("Time set");
			print_LCD_bottom("was success.");
			HAL_Delay(2000);
			
			current_col = -1;
			justReset = false;
			
			hour = 0;
			min = 0;
			sec = 0;
			rtc_set_time(hour, min, sec);
		}
    
		HAL_Delay(5);
				
		char result[100];
		
		//READ WHAT BUTTONS WERE PRESSED
		if (current_col == 0) {
			HAL_Delay(1);
			print_LCD("INTERACT");
			HAL_Delay(2000);
			
			HAL_GPIO_WritePin(ROW3_GPIO_Port, ROW3_Pin, GPIO_PIN_SET);
				
			HAL_Delay(1);
			
			HAL_GPIO_WritePin(ROW2_GPIO_Port, ROW2_Pin, GPIO_PIN_SET);
			for(int x=0; x<motorRotateVal; x=x+1)
			{
				HAL_GPIO_WritePin(ROW1_GPIO_Port, ROW1_Pin, GPIO_PIN_SET);
				HAL_Delay(1);
				HAL_GPIO_WritePin(ROW1_GPIO_Port, ROW1_Pin, GPIO_PIN_RESET);
				HAL_Delay(1);
			}
			
			HAL_GPIO_WritePin(ROW2_GPIO_Port, ROW2_Pin, GPIO_PIN_RESET);
			
			HAL_Delay(1);
			
			HAL_GPIO_WritePin(ROW3_GPIO_Port, ROW3_Pin, GPIO_PIN_RESET);
				
			HAL_Delay(1);
			
		}
		else if (current_col == 1) {
			alarm_min = 1;
			alarm_hour = 0;
			alarm_sec = 0;
			
			HAL_Delay(1);
			print_LCD("RESET");
			HAL_Delay(2000);
			justReset = true;
		}
		else if (current_col == 2) {
			hour = 0;
			min = 0;
			sec = 0;
			rtc_set_time(hour, min, sec);
			alarm_min++;
			if (alarm_min == 61) {
				alarm_min = 0;
			}
			
		}
		else if (current_col == 3) {
			hour = 0;
			min = 0;
			sec = 0;
			rtc_set_time(hour, min, sec);
			alarm_hour++;
		}
		
		//PRINT THE MESSAGE IF THEY PRINTED SOMETHING		
		if (current_col != -1) {
				print_msg(result);
		}
		
		current_col = -1;
	
			
		// Your code here
    HAL_Delay(500);
		
		//Get the time and date
		rtc_get_time(&hour, &min, &sec);
		
		
		HAL_Delay(1);
					HAL_Delay(10);
					print_LCD_top("Time Until Dose");
					sprintf(msg, "%02d : %02d", alarm_hour - hour, alarm_min - min);
					print_LCD_bottom(msg);	
		
		//If we have reached the alarm
		if (sec >= alarm_sec && min >= alarm_min && hour >= alarm_hour) {
		  
			uint8_t secondStop = sec + 10;
			
			current_col = -1;
			
			while (current_col != 0) {
				//Toggle the LEDs
				HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
				HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
				HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
				HAL_GPIO_TogglePin(ROW0_GPIO_Port, ROW0_Pin);
				
				//Print the ALARM
			//	sprintf(msg,"TIME FOR MEDICATION - PRESS INTERACT");
				print_LCD_top("time for dose!!");
				print_LCD_bottom("press interact.");
				
				//1 second delay
				HAL_Delay(1000);
				
				//Update the time
				rtc_get_time(&hour, &min, &sec);
			}
			
			current_col = -1;
			
						HAL_Delay(1);
			
			//Since we pressed interact, let's actuate the motor to reveal the next slot
			HAL_GPIO_WritePin(ROW3_GPIO_Port, ROW3_Pin, GPIO_PIN_SET);
				
			HAL_Delay(1);
			
			HAL_GPIO_WritePin(ROW2_GPIO_Port, ROW2_Pin, GPIO_PIN_SET);
			for(int x=0; x<motorRotateVal; x=x+1)
			{
				HAL_GPIO_WritePin(ROW1_GPIO_Port, ROW1_Pin, GPIO_PIN_SET);
				HAL_Delay(1);
				HAL_GPIO_WritePin(ROW1_GPIO_Port, ROW1_Pin, GPIO_PIN_RESET);
				HAL_Delay(1);
			}
			
			//motorRotateVal += 2;
			
			HAL_GPIO_WritePin(ROW2_GPIO_Port, ROW2_Pin, GPIO_PIN_RESET);
			
			HAL_Delay(1);
			
			HAL_GPIO_WritePin(ROW3_GPIO_Port, ROW3_Pin, GPIO_PIN_RESET);
				
			HAL_Delay(1);
			
			//Prompt the user
			refillCounter++;
				
			if (refillCounter == 14) {
				needRefill = true;
			}
			
			sprintf(msg, "%d doses", 14 - refillCounter);
			print_LCD_top(msg);
			print_LCD_bottom("until refill");
			
			//Automatically reset the time
			hour = 0;
			min = 0;
			sec = 0;
			rtc_set_time(hour, min, sec);
			
			//IF LEDS ARE ON, TURN THEM OFF
			if (HAL_GPIO_ReadPin(ROW0_GPIO_Port, ROW0_Pin)) {
				//Toggle the LEDs
				HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
				HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
				HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
				HAL_GPIO_TogglePin(ROW0_GPIO_Port, ROW0_Pin);
			}
			
		}
		
		//If we updated the alarm in the interrupt handler
		if (updatedAlarmFlag) {
			//Write to the eeprom registers
			eeprom_write(0x7,&alarm_sec,1);
			eeprom_write(0x8,&alarm_min,1);
			eeprom_write(0x9,&alarm_hour,1);
			eeprom_write(0xA,&alarm_wkday,1);
			eeprom_write(0xB,&alarm_day,1);
			eeprom_write(0xC,&alarm_month,1);
			eeprom_write(0xD,&alarm_year,1);
			
			//Reset flag
			updatedAlarmFlag = 0;
		}
		
    HAL_Delay(500);
	
	}
			
}

void print_LCD(char * msg){
    HD44780_Clear();
    HD44780_SetCursor(0,0);
    HD44780_PrintStr(msg);
}


void print_LCD_top(char * msg){
    HD44780_Clear();
    HD44780_SetCursor(0,0);
    HD44780_PrintStr(msg);
}

void print_LCD_bottom(char * msg){
   // HD44780_Clear();
    HD44780_SetCursor(0,1);
    HD44780_PrintStr(msg);
}

void print_LCD_scroll(char * msg){
    HD44780_Clear();
    HD44780_SetCursor(4,0);
    HD44780_PrintStr(msg);
    int x;
    for(int x=0; x<40; x=x+1)
    {
     HD44780_ScrollDisplayLeft();  //HD44780_ScrollDisplayRight();
     HAL_Delay(500);
    }
}