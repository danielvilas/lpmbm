//
// Created by Daniel Vilas Perulan on 3/2/18.
//

#ifndef LPM_BENCHMARK_GPIOCTL_H
#define LPM_BENCHMARK_GPIOCTL_H

#define IN  0
#define OUT 1

#define LOW  0
#define HIGH 1

#define PIN  24 /* P1-18 */
#define POUT 4  /* P1-07 */

int GPIOExport(int pin);
int GPIOUnexport(int pin);
int GPIODirection(int pin, int dir);
int GPIORead(int pin);
int GPIOWrite(int pin, int value);

#endif //LPM_BENCHMARK_GPIOCTL_H
