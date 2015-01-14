/* **************************************************************************																					
 *                                OpenPicus                 www.openpicus.com
 *                                                            italian concept
 * 
 *            openSource wireless Platform for sensors and Internet of Things	
 * **************************************************************************
 *  FileName:        iNEMO.c
 *  Module:          FlyPort WI-FI - FlyPort ETH
 *  Compiler:        Microchip C30 v3.12 or higher
 *
 *  Author               Rev.    Date              Comment
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Saccucci Stefano     1.0     01/14/2015		   First release  
 *  
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Software License Agreement
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  This is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License (version 2) as published by 
 *  the Free Software Foundation AND MODIFIED BY OpenPicus team.
 *  
 *  ***NOTE*** The exception to the GPL is included to allow you to distribute
 *  a combined work that includes OpenPicus code without being obliged to 
 *  provide the source code for proprietary components outside of the OpenPicus
 *  code. 
 *  OpenPicus software is distributed in the hope that it will be useful, but 
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details. 
 * 
 * 
 * Warranty
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * WE ARE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 **************************************************************************/
#include "iNEMO.h"

char dbg_string[500];

/// @cond
struct _iNEMO
{
	int DRDY_G;
	int INT_G;
	int INT1_XM;
	int INT2_XM;
	int DEN_G;
	
} iNEMO;

unsigned char iNEMOGetAddr(unsigned char * reg)
{
	if((*reg)>0x80)
	{
		*reg=(*reg)-0x80;
		return LSM9D0_ADDR_ANGULAR;
	}
	else
		return LSM9D0_ADDR_LIN_MAG;
}

double iNEMOGetRes(unsigned char reg)
{
	unsigned char data[1];
	double res = 0;
	if(reg>0x80)
		reg=reg-0x80;
	if(reg>=iNEMO_OUT_X_L_A&&reg<=iNEMO_OUT_Z_H_A)
	{
		iNEMOReadByte(iNEMO_CTRL_REG2_XM, 1, data);
		data[0]=(data[0]&0b00111000)>>3;
		if(data[0]==4)
			res = 0.732/1000;
		else
			res = 0.061*(data[0]+1)/1000;	
	}
	else if(reg>=iNEMO_OUT_X_L_M&&reg<=iNEMO_OUT_Z_H_M)
	{
		iNEMOReadByte(iNEMO_CTRL_REG6_XM, 1, data);
		data[0]=(data[0]&0b01100000)>>5;
		if(data[0]==3)
			res = 0.48/1000;
		else
			res = 0.08*pow(2,data[0])/1000;	
	}
	else if(reg>=iNEMO_OUT_X_L_G&&reg<=iNEMO_OUT_Z_H_G)
	{
		iNEMOReadByte(iNEMO_CTRL_REG4_G, 1, data);
		data[0]=(data[0]&0b00110000)>>4;
		if(data[0]<2)
			res = 8.75*(data[0]+1)/1000;
		else
			res = 70/1000;
	}
	return res;
}
/// @endcond

/**
 * Function to initialize the iNEMO
 * \param pinDRDY_G - gyroscope data ready pin (input, 0 if it is unused)
 * \param pinINT_G - gyroscope programmable interrupt (input, 0 if it is unused)
 * \param pinINT1_XM - accelerometer and magnetic sensor interrupt 1 (input, 0 if it is unused)
 * \param pinINT2_XM - accelerometer and magnetic sensor interrupt 2 (input, 0 if it is unused)
 * \param pinDEN_G - gyroscope data enable (input, 0 if it is unused)
 */
void iNEMOInit(int pinDRDY_G, int pinINT_G, int pinINT1_XM, int pinINT2_XM,	int pinDEN_G)
{
	iNEMO.DRDY_G = pinDRDY_G;
	iNEMO.INT_G = pinINT_G;
	iNEMO.INT1_XM = pinINT1_XM;
	iNEMO.INT2_XM = pinINT2_XM;
	iNEMO.DEN_G = pinDEN_G;
	
	I2C2Init(HIGH_SPEED);
	if(pinDRDY_G>0)
		IOInit(iNEMO.DRDY_G, in);
	if(pinINT_G>0)
		IOInit(iNEMO.INT_G, in);
	if(pinINT1_XM>0)
		IOInit(iNEMO.INT1_XM, in);
	if(pinINT2_XM>0)
		IOInit(iNEMO.INT2_XM, in);
	if(pinDEN_G>0)
		IOInit(iNEMO.DEN_G, in);
	
	vTaskDelay(5);

	iNEMOWrite1Byte(iNEMO_CTRL_REG1_G,0x0F); //95Hz, 12.5 Cutoff, Normal-mode, all axis enabled
	iNEMOWrite1Byte(iNEMO_CTRL_REG5_G,0b01000000); //normal mode boot, FIFO enabled, HP disabled, INT1 disabled, OUT disable
	iNEMOWrite1Byte(iNEMO_FIFO_CTRL_REG_G,0b01000000); //Gyro: stream mode, no FIFO threshold
	iNEMOWrite1Byte(iNEMO_CTRL_REG5_XM,0b10001100); //Temp sens. enabled, magnetic: low res., magnetic 25Hz, no latch 
	iNEMOWrite1Byte(iNEMO_CTRL_REG0_XM,0b01000000); //normal mode boot, FIFO enabled, watermark disabled, HP click disabled, HPIS1 e HPIS2 disabled
	iNEMOWrite1Byte(iNEMO_CTRL_REG1_XM,0b01100111); //Acc: 100Hz, continuos update, all axis enabled 
	iNEMOWrite1Byte(iNEMO_CTRL_REG7_XM,0b00000000); //HP normal mode, no filter, magnetic: continuos conversion
	iNEMOWrite1Byte(iNEMO_FIFO_CTRL_REG,0b01000000); //FIFO stream mode, no watermark
	iNEMOWrite1Byte(iNEMO_CLICK_CFG,0x00); //no-click
}

/**
 * Function to read from the iNEMO
 * \param reg - register to read (use define, see iNEMO.h)
 * \param nbyte - number of bytes to read
 * \param dest - array where to store the data
 */
void iNEMOReadByte(unsigned char reg, unsigned char nbyte, unsigned char *dest)
{
	unsigned char ACTUAL_ADDR = iNEMOGetAddr(&reg);
	if(nbyte==1)
		dest[0]=I2C2ReadReg(ACTUAL_ADDR, reg, 1);
	else
		I2C2ReadMulti(ACTUAL_ADDR,reg+0x80, dest, nbyte, 1);
}

/**
 * Function to write a byte into the iNEMO
 * \param reg - register where you want to write data (use define, see iNEMO.h)
 * \param data - array with data to be written
 */
void iNEMOWrite1Byte(unsigned char reg, unsigned char data)
{
	unsigned char ACTUAL_ADDR = iNEMOGetAddr(&reg);	
	I2C2WriteReg(ACTUAL_ADDR, reg, data);
}

/**
 * Function to read the acceleration measurement (3-axis, X, Y, Z)
 * \param dest - array where to store the data [g]
 */
void iNEMOReadAcc(double * dest)
{
	unsigned char data[6];
	double res = iNEMOGetRes(iNEMO_OUT_X_L_A);
	iNEMOReadByte(iNEMO_OUT_X_L_A, 6, data);
	dest[0]=((((int)data[1])<<8)+data[0])*res;
	dest[1]=((((int)data[3])<<8)+data[2])*res;
	dest[2]=((((int)data[5])<<8)+data[4])*res;
}

/**
 * Function to read the acceleration measurement (only X)
 * \return the acceleration measurement [g]
 */
double iNEMOReadAccX()
{
	unsigned char data[2];
	double res = iNEMOGetRes(iNEMO_OUT_X_L_A);
	double realdata = 0;
	iNEMOReadByte(iNEMO_OUT_X_L_A, 2, data);
	realdata=((((int)data[1])<<8)+data[0])*res;
	return realdata;
}

/**
 * Function to read the acceleration measurement (only Y)
 * \return the acceleration measurement [g]
 */
double iNEMOReadAccY()
{
	unsigned char data[2];
	double res = iNEMOGetRes(iNEMO_OUT_Y_L_A);
	double realdata = 0;
	iNEMOReadByte(iNEMO_OUT_Y_L_A, 2, data);
	realdata=((((int)data[1])<<8)+data[0])*res;
	return realdata;
}

/**
 * Function to read the acceleration measurement (only Z)
 * \return the acceleration measurement [g]
 */
double iNEMOReadAccZ()
{
	unsigned char data[2];
	double res = iNEMOGetRes(iNEMO_OUT_Z_L_A);
	double realdata = 0;
	iNEMOReadByte(iNEMO_OUT_Z_L_A, 2, data);
	realdata=((((int)data[1])<<8)+data[0])*res;
	return realdata;
}

/**
 * Function to read the measurement of the angular rate (3-axis, X, Y, Z)
 * \param dest - array where to store the data [dps]
 */
void iNEMOReadGyro(double * dest)
{
	unsigned char data[6];
	double res = iNEMOGetRes(iNEMO_OUT_X_L_G);
	iNEMOReadByte(iNEMO_OUT_X_L_G, 6, data);
	dest[0]=((((int)data[1])<<8)+data[0])*res;
	dest[1]=((((int)data[3])<<8)+data[2])*res;
	dest[2]=((((int)data[5])<<8)+data[4])*res;
}

/**
 * Function to read the measurement of the angular rate (only X)
 * \return the measurement of the angular rate [dps]
 */
double iNEMOReadGyroX()
{
	unsigned char data[2];
	double res = iNEMOGetRes(iNEMO_OUT_X_L_G);
	double realdata = 0;
	iNEMOReadByte(iNEMO_OUT_X_L_G, 2, data);
	realdata=((((int)data[1])<<8)+data[0])*res;
	return realdata;
}

/**
 * Function to read the measurement of the angular rate (only Y)
 * \return the measurement of the angular rate [dps]
 */
double iNEMOReadGyroY()
{
	unsigned char data[2];
	double res = iNEMOGetRes(iNEMO_OUT_Y_L_G);
	double realdata = 0;
	iNEMOReadByte(iNEMO_OUT_Y_L_G, 2, data);
	realdata=((((int)data[1])<<8)+data[0])*res;
	return realdata;
}

/**
 * Function to read the measurement of the angular rate (only Z)
 * \return the measurement of the angular rate [dps]
 */
double iNEMOReadGyroZ()
{
	unsigned char data[2];
	double res = iNEMOGetRes(iNEMO_OUT_Z_L_G);
	double realdata = 0;
	iNEMOReadByte(iNEMO_OUT_Z_L_G, 2, data);
	realdata=((((int)data[1])<<8)+data[0])*res;
	return realdata;
}

/**
 * Function to read the measurement of the magnetic field (3-axis, X, Y, Z)
 * \param dest - array where to store the data [gauss]
 */
void iNEMOReadMagn(double * dest)
{
	unsigned char data[6];
	double res = iNEMOGetRes(iNEMO_OUT_X_L_M);
	iNEMOReadByte(iNEMO_OUT_X_L_M, 6, data);
	dest[0]=((((int)data[1])<<8)+data[0])*res;
	dest[1]=((((int)data[3])<<8)+data[2])*res;
	dest[2]=((((int)data[5])<<8)+data[4])*res;
}

/**
 * Function to read the measurement of the magnetic field (only X)
 * \return the measurement of the magnetic field [gauss]
 */
double iNEMOReadMagnX()
{
	unsigned char data[2];
	double res = iNEMOGetRes(iNEMO_OUT_X_L_M);
	double realdata = 0;
	iNEMOReadByte(iNEMO_OUT_X_L_M, 2, data);
	realdata=((((int)data[1])<<8)+data[0])*res;
	return realdata;
}

/**
 * Function to read the measurement of the magnetic field (only Y)
 * \return the measurement of the magnetic field [gauss]
 */
double iNEMOReadMagnY()
{
	unsigned char data[2];
	double res = iNEMOGetRes(iNEMO_OUT_Y_L_M);
	double realdata = 0;
	iNEMOReadByte(iNEMO_OUT_Y_L_M, 2, data);
	realdata=((((int)data[1])<<8)+data[0])*res;
	return realdata;
}

/**
 * Function to read the measurement of the magnetic field (only Z)
 * \return the measurement of the magnetic field [gauss]
 */
double iNEMOReadMagnZ()
{
	unsigned char data[2];
	double res = iNEMOGetRes(iNEMO_OUT_Z_L_M);
	double realdata = 0;
	iNEMOReadByte(iNEMO_OUT_Z_L_M, 2, data);
	realdata=((((int)data[1])<<8)+data[0])*res;
	return realdata;
}

/**
 * Function to read all measurement
 * \param dest - array where to store the data, in this way: acceleration X,Y,Z [g]; angular rate X,Y,Z [dps]; magnetic field X,Y,Z [gauss]
 */
void iNEMOReadAll(double * dest)
{
	unsigned char data[6];
	
	double res = iNEMOGetRes(iNEMO_OUT_X_L_A);
	iNEMOReadByte(iNEMO_OUT_X_L_A, 6, data);
	dest[0]=((((int)data[1])<<8)+data[0])*res;
	dest[1]=((((int)data[3])<<8)+data[2])*res;
	dest[2]=((((int)data[5])<<8)+data[4])*res;
	
	res = iNEMOGetRes(iNEMO_OUT_X_L_G);
	iNEMOReadByte(iNEMO_OUT_X_L_G, 6, data);
	dest[3]=((((int)data[1])<<8)+data[0])*res;
	dest[4]=((((int)data[3])<<8)+data[2])*res;
	dest[5]=((((int)data[5])<<8)+data[4])*res;
	
	res = iNEMOGetRes(iNEMO_OUT_X_L_M);
	iNEMOReadByte(iNEMO_OUT_X_L_M, 6, data);
	dest[6]=((((int)data[1])<<8)+data[0])*res;
	dest[7]=((((int)data[3])<<8)+data[2])*res;
	dest[8]=((((int)data[5])<<8)+data[4])*res;
}

