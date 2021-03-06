/*
 * RTC_r2051.c
 *
 *  Created on: 2014.05.03.
 *      Author: csabi
 */

#include "ch.h"
#include "hal.h"
#include "globals.h"
#include "RTC_r2051.h"


/*!
* A Real Time Clock áramkör inicializálása.
*/
void RTC_init()
{
	msg_t rc = RDY_OK;
	uint8_t RTC_control_regs[3]  = { 0xE0, 0b00100000, 0b00010000 }; // Control regs base addr., 24H mode, 0

	RTC_time_t timeToSet;
	RTC_date_t dateToSet;

	i2cAcquireBus(&I2CD1);
	rc = i2cMasterTransmitTimeout( &I2CD1, (i2caddr_t)RTC_ADDRESS,
			                       RTC_control_regs, 2,
			                       0, 0, TIME_INFINITE );
	i2cReleaseBus(&I2CD1);
/*
	timeToSet.hour = 1;
	timeToSet.minute = 30;
	timeToSet.second = 00;
	RTC_setTime(timeToSet);

	dateToSet.day = 29;
	dateToSet.dayOfWeek = 7;
	dateToSet.month = 3;
	dateToSet.year = 15;
	RTC_setDate(dateToSet);
*/
}


/*!
* Beállítja az időt az RTC áramkörben.
* @param[in] timeToBeSet A beállítandó idő.
*/
void RTC_setTime(RTC_time_t timeToBeSet)
{
	msg_t rc = RDY_OK;
	uint8_t i = 1;
	timeToBeSet.timeCounterAddress = 0x00;

	uint8_t * timeToBeSet_p = &timeToBeSet.timeCounterAddress;

    /* Converting binary to BCD */
	for ( ; i <= 3; i++ )
	{
		*(timeToBeSet_p + i) = BIN_TO_BCD(*(timeToBeSet_p + i)); //((*(timeToBeSet_p + i) / 10) << 4) | (*(timeToBeSet_p + i) % 10);
	}

	i2cAcquireBus(&I2CD1);
	rc = i2cMasterTransmitTimeout( &I2CD1, (i2caddr_t)RTC_ADDRESS,
			                       timeToBeSet_p, 4,
			                       0, 0, TIME_INFINITE );
	i2cReleaseBus(&I2CD1);
}


/*!
* Beállítja a dátumot az RTC áramkörben.
* @param[in] dateToBeSet A beállítandó dátum.
*/
void RTC_setDate(RTC_date_t dateToBeSet)
{
	msg_t rc = RDY_OK;
	uint8_t i = 2;
	dateToBeSet.calendarCounterAddress = 0x30;

	uint8_t * dateToBeSet_p = &dateToBeSet.calendarCounterAddress;

    /* Converting binary to BCD */
	for ( ; i <= 4; i++ )
	{
		*(dateToBeSet_p + i) = BIN_TO_BCD(*(dateToBeSet_p + i));
	}

	dateToBeSet.month |= 0b10000000;

	i2cAcquireBus(&I2CD1);
	rc = i2cMasterTransmitTimeout( &I2CD1, (i2caddr_t)RTC_ADDRESS,
								   dateToBeSet_p, 5,
			                       0, 0, TIME_INFINITE );
	i2cReleaseBus(&I2CD1);
}


/*!
* Az idő kiolvasása az RTC áramkörből.
* @return Az aktuális idő.
*/
RTC_time_t RTC_getTime()
{
	msg_t rc = RDY_OK;
	RTC_time_t timeBuffer;
//	timeBuffer.timeCounterAddress = 0x00;
	uint8_t i = 0;

	uint8_t * timeBuffer_p = &(timeBuffer.timeCounterAddress);

	i2cAcquireBus(&I2CD1);
	rc = i2cMasterReceiveTimeout( &I2CD1, (i2caddr_t)RTC_ADDRESS,
			                      timeBuffer_p,
						          4, TIME_INFINITE);

/*
	rc = i2cMasterTransmitTimeout( &I2CD1, (i2caddr_t)RTC_ADDRESS,
			                      (uint8_t *)( &timeBuffer ), 1,
			                       timeBuffer_p,
			                       sizeof(timeBuffer) - 1, TIME_INFINITE);
*/

	i2cReleaseBus(&I2CD1);

	/* Converting BCD to binary */
	timeBuffer_p = &(timeBuffer.second);
	for ( ; i < 3; i++ )
	{
		*(timeBuffer_p + i) = BCD_TO_BIN(*(timeBuffer_p + i)); //((*(timeBuffer_p + i) >> 4) * 10) + (*(timeBuffer_p + i) & 0b00001111);
	}

	return timeBuffer;
}


/*!
* A dátum kiolvasása az RTC áramkörből.
* @return Az aktuális dátum.
*/
RTC_date_t RTC_getDate()
{
	msg_t rc = RDY_OK;
	RTC_date_t dateBuffer;
//	dateBuffer.calendarCounterAddress = 0x30;
	uint8_t i = 0;

	uint8_t * dateBuffer_p;

	i2cAcquireBus(&I2CD1);
	rc = i2cMasterReceiveTimeout( &I2CD1, (i2caddr_t)RTC_ADDRESS,
			                      &dateBuffer, 8, TIME_INFINITE);
/*
	rc = i2cMasterTransmitTimeout( &I2CD1, (i2caddr_t)RTC_ADDRESS,
								   &(dateBuffer.calendarCounterAddress), 1,
								   &(dateBuffer.dayOfWeek), 4, TIME_INFINITE );
*/
/* Lehet, hogy ez a megoldás kell...
	if ( rc == RDY_OK )
	{
		rc = i2cMasterReceiveTimeout( &I2CD1, (i2caddr_t)RTC_ADDRESS,
									  &(dateBuffer.dayOfWeek), 4, TIME_INFINITE );
	}
*/
	i2cReleaseBus(&I2CD1);

	dateBuffer.month &= 0b01111111;

	/* Converting BCD to binary */
	dateBuffer_p = &(dateBuffer.day);
	for ( ; i < 3; i++ )
	{
		*(dateBuffer_p + i) = BCD_TO_BIN(*(dateBuffer_p + i));
	}

	return dateBuffer;
}

