/*
 * tasks.h
 *
 *  Created on: 2014.10.13.
 *      Author: csabi
 */

#ifndef TASKS_H_
#define TASKS_H_

WORKING_AREA(waThread1, 48);
msg_t Thread1(void *arg);

WORKING_AREA(waThread2, 224); //216
msg_t Thread2(void *arg);

WORKING_AREA(waThread3, 152);
msg_t Thread3(void *arg);

WORKING_AREA(waThread4, 192); //184 192
msg_t Thread4(void *arg);

WORKING_AREA(waThread5, 88);
msg_t Thread5(void *arg);


#endif /* TASKS_H_ */
