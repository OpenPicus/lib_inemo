#include "iNEMO.h"

char dbg_string[500];

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

double iNEMOGetRes(unsigned char reg) //no temp
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

void iNEMOInit(int pinDRDY_G, int pinINT_G, int pinINT1_XM, int pinINT2_XM,	int pinDEN_G)
{
	iNEMO.DRDY_G = pinDRDY_G;
	iNEMO.INT_G = pinINT_G;
	iNEMO.INT1_XM = pinINT1_XM;
	iNEMO.INT2_XM = pinINT2_XM;
	iNEMO.DEN_G = pinDEN_G;
	
	I2C2Init(HIGH_SPEED);
	IOInit(iNEMO.DRDY_G, in);
	IOInit(iNEMO.INT_G, in);
	IOInit(iNEMO.INT1_XM, in);
	IOInit(iNEMO.INT2_XM, in);
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
}

void iNEMOReadByte(unsigned char reg, unsigned char nbyte, unsigned char *dest)
{
	unsigned char ACTUAL_ADDR = iNEMOGetAddr(&reg);
	if(nbyte==1)
		dest[0]=I2C2ReadReg(ACTUAL_ADDR, reg, 1);
	else
		I2C2ReadMulti(ACTUAL_ADDR,reg+0x80, dest, nbyte, 1);
}

void iNEMOWrite1Byte(unsigned char reg, unsigned char data)
{
	unsigned char ACTUAL_ADDR = iNEMOGetAddr(&reg);	
	I2C2WriteReg(ACTUAL_ADDR, reg, data);
}

void iNEMOReadAcc(double * dest)
{
	unsigned char data[6];
	double res = iNEMOGetRes(iNEMO_OUT_X_L_A);
	iNEMOReadByte(iNEMO_OUT_X_L_A, 6, data);
	dest[0]=((((int)data[1])<<8)+data[0])*res;
	dest[1]=((((int)data[3])<<8)+data[2])*res;
	dest[2]=((((int)data[5])<<8)+data[4])*res;
}

double iNEMOReadAccX()
{
	unsigned char data[2];
	double res = iNEMOGetRes(iNEMO_OUT_X_L_A);
	double realdata = 0;
	iNEMOReadByte(iNEMO_OUT_X_L_A, 2, data);
	realdata=((((int)data[1])<<8)+data[0])*res;
	return realdata;
}

double iNEMOReadAccY()
{
	unsigned char data[2];
	double res = iNEMOGetRes(iNEMO_OUT_Y_L_A);
	double realdata = 0;
	iNEMOReadByte(iNEMO_OUT_Y_L_A, 2, data);
	realdata=((((int)data[1])<<8)+data[0])*res;
	return realdata;
}

double iNEMOReadAccZ()
{
	unsigned char data[2];
	double res = iNEMOGetRes(iNEMO_OUT_Z_L_A);
	double realdata = 0;
	iNEMOReadByte(iNEMO_OUT_Z_L_A, 2, data);
	realdata=((((int)data[1])<<8)+data[0])*res;
	return realdata;
}

void iNEMOReadGyro(double * dest)
{
	unsigned char data[6];
	double res = iNEMOGetRes(iNEMO_OUT_X_L_G);
	iNEMOReadByte(iNEMO_OUT_X_L_G, 6, data);
	dest[0]=((((int)data[1])<<8)+data[0])*res;
	dest[1]=((((int)data[3])<<8)+data[2])*res;
	dest[2]=((((int)data[5])<<8)+data[4])*res;
}

double iNEMOReadGyroX()
{
	unsigned char data[2];
	double res = iNEMOGetRes(iNEMO_OUT_X_L_G);
	double realdata = 0;
	iNEMOReadByte(iNEMO_OUT_X_L_G, 2, data);
	realdata=((((int)data[1])<<8)+data[0])*res;
	return realdata;
}

double iNEMOReadGyroY()
{
	unsigned char data[2];
	double res = iNEMOGetRes(iNEMO_OUT_Y_L_G);
	double realdata = 0;
	iNEMOReadByte(iNEMO_OUT_Y_L_G, 2, data);
	realdata=((((int)data[1])<<8)+data[0])*res;
	return realdata;
}

double iNEMOReadGyroZ()
{
	unsigned char data[2];
	double res = iNEMOGetRes(iNEMO_OUT_Z_L_G);
	double realdata = 0;
	iNEMOReadByte(iNEMO_OUT_Z_L_G, 2, data);
	realdata=((((int)data[1])<<8)+data[0])*res;
	return realdata;
}

void iNEMOReadMagn(double * dest)
{
	unsigned char data[6];
	double res = iNEMOGetRes(iNEMO_OUT_X_L_M);
	iNEMOReadByte(iNEMO_OUT_X_L_M, 6, data);
	dest[0]=((((int)data[1])<<8)+data[0])*res;
	dest[1]=((((int)data[3])<<8)+data[2])*res;
	dest[2]=((((int)data[5])<<8)+data[4])*res;
}

double iNEMOReadMagnX()
{
	unsigned char data[2];
	double res = iNEMOGetRes(iNEMO_OUT_X_L_M);
	double realdata = 0;
	iNEMOReadByte(iNEMO_OUT_X_L_M, 2, data);
	realdata=((((int)data[1])<<8)+data[0])*res;
	return realdata;
}

double iNEMOReadMagnY()
{
	unsigned char data[2];
	double res = iNEMOGetRes(iNEMO_OUT_Y_L_M);
	double realdata = 0;
	iNEMOReadByte(iNEMO_OUT_Y_L_M, 2, data);
	realdata=((((int)data[1])<<8)+data[0])*res;
	return realdata;
}

double iNEMOReadMagnZ()
{
	unsigned char data[2];
	double res = iNEMOGetRes(iNEMO_OUT_Z_L_M);
	double realdata = 0;
	iNEMOReadByte(iNEMO_OUT_Z_L_M, 2, data);
	realdata=((((int)data[1])<<8)+data[0])*res;
	return realdata;
}

double iNEMOReadTemp()
{
	unsigned char data[2];
	double res = 8;
	double realdata = 0;
	iNEMOReadByte(iNEMO_OUT_TEMP_L_XM, 2, data);
	realdata=((((int)(data[1]&0x0F))<<8)+data[0])/res;
	return realdata;
}
