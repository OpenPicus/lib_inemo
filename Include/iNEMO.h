/* **************************************************************************																					
 *                                OpenPicus                 www.openpicus.com
 *                                                            italian concept
 * 
 *            openSource wireless Platform for sensors and Internet of Things	
 * **************************************************************************
 *  FileName:        iNEMO.h
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

#include "HWlib.h"
#include <math.h>

#define LSM9D0_ADDR_LIN_MAG	0x3C>>1
#define LSM9D0_ADDR_ANGULAR	0xD4>>1

#define iNEMO_WHO_AM_I_G 0x0F+0x80
#define iNEMO_CTRL_REG1_G 0x20+0x80
#define iNEMO_CTRL_REG2_G 0x21+0x80
#define iNEMO_CTRL_REG3_G 0x22+0x80
#define iNEMO_CTRL_REG4_G 0x23+0x80
#define iNEMO_CTRL_REG5_G 0x24+0x80
#define iNEMO_REFERENCE_G 0x25+0x80
#define iNEMO_STATUS_REG_G 0x27+0x80
#define iNEMO_OUT_X_L_G 0x28+0x80
#define iNEMO_OUT_X_H_G 0x29+0x80
#define iNEMO_OUT_Y_L_G 0x2A+0x80
#define iNEMO_OUT_Y_H_G 0x2B+0x80
#define iNEMO_OUT_Z_L_G 0x2C+0x80
#define iNEMO_OUT_Z_H_G 0x2D+0x80
#define iNEMO_FIFO_CTRL_REG_G 0x80+0x2E
#define iNEMO_FIFO_SRC_REG_G 0x2F+0x80
#define iNEMO_INT1_CFG_G 0x30+0x80
#define iNEMO_INT1_SRC_G 0x31+0x80
#define iNEMO_INT1_TSH_XH_G 0x32+0x80
#define iNEMO_INT1_TSH_XL_G 0x33+0x80
#define iNEMO_INT1_TSH_YH_G 0x34+0x80
#define iNEMO_INT1_TSH_YL_G 0x35+0x80
#define iNEMO_INT1_TSH_ZH_G 0x36+0x80
#define iNEMO_INT1_TSH_ZL_G 0x37+0x80
#define iNEMO_INT1_DURATION_G 0x38+0x80
#define iNEMO_OUT_TEMP_L_XM 0x05
#define iNEMO_OUT_TEMP_H_XM 0x06
#define iNEMO_STATUS_REG_M 0x07
#define iNEMO_OUT_X_L_M 0x08
#define iNEMO_OUT_X_H_M 0x09
#define iNEMO_OUT_Y_L_M 0x0A
#define iNEMO_OUT_Y_H_M 0x0B
#define iNEMO_OUT_Z_L_M 0x0C
#define iNEMO_OUT_Z_H_M 0x0D
#define iNEMO_WHO_AM_I_XM 0x0F
#define iNEMO_INT_CTRL_REG_M 0x12
#define iNEMO_INT_SRC_REG_M 0x13
#define iNEMO_INT_THS_L_M 0x14
#define iNEMO_INT_THS_H_M 0x15
#define iNEMO_OFFSET_X_L_M 0x16
#define iNEMO_OFFSET_X_H_M 0x17
#define iNEMO_OFFSET_Y_L_M 0x18
#define iNEMO_OFFSET_Y_H_M 0x19
#define iNEMO_OFFSET_Z_L_M 0x1A
#define iNEMO_OFFSET_Z_H_M 0x1B
#define iNEMO_REFERENCE_X 0x1C
#define iNEMO_REFERENCE_Y 0x1D
#define iNEMO_REFERENCE_Z 0x1E
#define iNEMO_CTRL_REG0_XM 0x1F
#define iNEMO_CTRL_REG1_XM 0x20
#define iNEMO_CTRL_REG2_XM 0x21
#define iNEMO_CTRL_REG3_XM 0x22
#define iNEMO_CTRL_REG4_XM 0x23
#define iNEMO_CTRL_REG5_XM 0x24
#define iNEMO_CTRL_REG6_XM 0x25
#define iNEMO_CTRL_REG7_XM 0x26
#define iNEMO_STATUS_REG_A 0x27
#define iNEMO_OUT_X_L_A 0x28
#define iNEMO_OUT_X_H_A 0x29
#define iNEMO_OUT_Y_L_A 0x2A
#define iNEMO_OUT_Y_H_A 0x2B
#define iNEMO_OUT_Z_L_A 0x2C
#define iNEMO_OUT_Z_H_A 0x2D
#define iNEMO_FIFO_CTRL_REG 0x2E
#define iNEMO_FIFO_SRC_REG 0x2F
#define iNEMO_INT_GEN_1_REG 0x30
#define iNEMO_INT_GEN_1_SRC 0x31
#define iNEMO_INT_GEN_1_THS 0x32
#define iNEMO_INT_GEN_1_DURATION 0x33
#define iNEMO_INT_GEN_2_REG 0x34
#define iNEMO_INT_GEN_2_SRC 0x35
#define iNEMO_INT_GEN_2_THS 0x36
#define iNEMO_INT_GEN_2_DURATION 0x37
#define iNEMO_CLICK_CFG 0x38
#define iNEMO_CLICK_SRC 0x39
#define iNEMO_CLICK_THS 0x3A
#define iNEMO_TIME_LIMIT 0x3B
#define iNEMO_TIME_LATENCY 0x3C
#define iNEMO_TIME_WINDOW 0x3D
#define iNEMO_Act_THS 0x3E
#define iNEMO_Act_DUR 0x3F

#define iNEMO_AXIS_X 0
#define iNEMO_AXIS_Y 1
#define iNEMO_AXIS_Z 2

void iNEMOInit(int, int, int, int,	int);
void iNEMOReadByte(unsigned char, unsigned char, unsigned char *);
void iNEMOWrite1Byte(unsigned char, unsigned char);
void iNEMOReadAcc(double *);
double iNEMOReadAccX();
double iNEMOReadAccY();
double iNEMOReadAccZ();
void iNEMOReadGyro(double *);
double iNEMOReadGyroX();
double iNEMOReadGyroY();
double iNEMOReadGyroZ();
void iNEMOReadMagn(double *);
double iNEMOReadMagnX();
double iNEMOReadMagnY();
double iNEMOReadMagnZ();
double iNEMOGetRes(unsigned char);
unsigned char iNEMOGetAddr(unsigned char *);
void iNEMOReadAll(double *);
