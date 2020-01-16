/*
*
* Arduino Micro PIUIO Clone ROAD24
*
* Copyright (c) 2020
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/** \file
 *
 *  Main source for piuio clone based on arduino pro mini, this code is based on LUFA's bulk vendor example
 */

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>

#include "Descriptors.h"

void SetupHardware(void);
void PIUIO_INPUTTask(void);
void PIUIO_LEDSTask(void);

/** 
 *	Global vars used to extra functions
 */

// my usbio object
uint8_t	LampData[8];
uint8_t SensorData[8];

/** Main program entry point. This routine configures the hardware required by the application, then
 *  enters a loop to run the application tasks in sequence.
 */
int main(void)
{
	SetupHardware();
	GlobalInterruptEnable();
	
	memset(LampData,0xFF,8);
	memset(SensorData,0xFF,8);
	
	for (;;)
	{
		PIUIO_INPUTTask();	// update input
		USB_USBTask();
		PIUIO_LEDSTask();	// update leds to reflect status			
	}
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

	/* Hardware Initialization */
	// Setup your  harware here
	DDRD &= ~( (1<<0) | (1<<1) | (1<<4) | (1<<7) );
	DDRC &= ~( (1<<6) );
	DDRB &= ~( (1<<1) | (1<<2) | (1<<3) | (1<<6) | (1<<5));
	DDRF &= ~( (1<<4) | (1<<5) | (1<<6) | (1<<7) );
	
	//enable pull-up
	PORTD |= ( (1<<0) | (1<<1) | (1<<4) | (1<<7) );
	PORTC |= ( (1<<6) );
	PORTB |= ( (1<<1) | (1<<2) | (1<<3) | (1<<6) | (1<<5));
	PORTF |= ( (1<<4) | (1<<5) | (1<<6) | (1<<7) );
	
	USB_Init();
}

/** Event handler for the USB_Connect event. This indicates that the device is enumerating via the status LEDs. */
void EVENT_USB_Device_Connect(void)
{

}

/** Event handler for the USB_Disconnect event. This indicates that the device is no longer connected to a host via
 *  the status LEDs.
 */
void EVENT_USB_Device_Disconnect(void)
{

}

/** Event handler for the USB_ConfigurationChanged event. This is fired when the host set the current configuration
 *  of the USB device after enumeration - the device endpoints are configured.
 */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	/* Setup Vendor Data Endpoints */
	ConfigSuccess &= Endpoint_ConfigureEndpoint(VENDOR_IN_EPADDR,  EP_TYPE_BULK, VENDOR_IO_EPSIZE, 1);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(VENDOR_OUT_EPADDR, EP_TYPE_BULK, VENDOR_IO_EPSIZE, 1);

	/* Indicate endpoint configuration success or failure */
	//LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);

	// enable start of frame events
	USB_Device_EnableSOFEvents();
}

/** Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to
 *  the device from the USB host before passing along unhandled control requests to the library for processing
 *  internally.
 */
void EVENT_USB_Device_ControlRequest(void)
{
	
    if( USB_ControlRequest.bRequest == 0xAE )
	{
		if( USB_ControlRequest.bmRequestType  == 0xC0 )
		{
			Endpoint_ClearSETUP();
			PIUIO_INPUTTask();
			Endpoint_Write_Control_Stream_LE(SensorData, 8);
			Endpoint_ClearOUT();
		}
		else if( USB_ControlRequest.bmRequestType  == 0x40 )
		{
			Endpoint_ClearSETUP();
			Endpoint_Read_Control_Stream_LE(LampData, 8);
			Endpoint_ClearIN();
		}
	}
}

#define	PORTD_MASK ( (1<<0) | (1<<1) | (1<<4) | (1<<7) )
#define	PORTC_MASK ( (1<<6) )
#define	PORTB_MASK ( (1<<1) | (1<<2) | (1<<3) | (1<<6) | (1<<5))
#define	PORTF_MASK ( (1<<4) | (1<<5) | (1<<6) | (1<<7) )

void PIUIO_INPUTTask(void)
{
	// clear data
	SensorData[0] = 0xFF;
	SensorData[1] = 0xFF;
	SensorData[2] = 0xFF;
	SensorData[3] = 0xFF;
	
	uint8_t dataB = PINB & PORTB_MASK;
	uint8_t dataC = PINC & PORTC_MASK;
	uint8_t dataD = PIND & PORTD_MASK;
	uint8_t dataF = PINF & PORTF_MASK;
	
	
	// ugly stuff
	// P1
	if(  !( dataD & (1<<1))  ) SensorData[2] &= ~(1<<0); else SensorData[2] |= (1<<0);
	if(  !( dataD & (1<<0))  ) SensorData[2] &= ~(1<<1); else SensorData[2] |= (1<<1);
	if(  !( dataD & (1<<7))  ) SensorData[2] &= ~(1<<2); else SensorData[2] |= (1<<2);
	if(  !( dataC & (1<<6))  ) SensorData[2] &= ~(1<<3); else SensorData[2] |= (1<<3);
	if(  !( dataD & (1<<4))  ) SensorData[2] &= ~(1<<4); else SensorData[2] |= (1<<4);
	// P2
	if(  !( dataF & (1<<7))  ) SensorData[0] &= ~(1<<0); else SensorData[0] |= (1<<0);
	if(  !( dataB & (1<<1))  ) SensorData[0] &= ~(1<<1); else SensorData[0] |= (1<<1);
	if(  !( dataB & (1<<3))  ) SensorData[0] &= ~(1<<2); else SensorData[0] |= (1<<2);
	if(  !( dataB & (1<<2))  ) SensorData[0] &= ~(1<<3); else SensorData[0] |= (1<<3);
	if(  !( dataB & (1<<6))  ) SensorData[0] &= ~(1<<4); else SensorData[0] |= (1<<4);	

}

void EVENT_USB_Device_StartOfFrame(void)
{
	// TODO
}

void PIUIO_LEDSTask(void)
{
	// TODO:
}


