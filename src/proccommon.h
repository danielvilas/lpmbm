/* 
 * File:   procnetstat.h
 * Author: Daniel
 *
 * Created on 10 de Julio de 2016, 19:34
 */

#ifndef PROC_COMMON_H
#define PROC_COMMON_H

#ifdef __cplusplus
"C" {
#endif
	
#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
	extern 	long tickspersec;
	typedef long long int num;
	
	void readone(FILE *input, num *x);
	void readunsigned(FILE *input, unsigned long long *x);
	void readstr(FILE *input, char *x);
	void readchar(FILE *input, char *x);

	void printone(char *name, num x);
	void printonex(char *name, num x);
	void printunsigned(char *name, unsigned long long x);
	void printchar(char *name, char x);
	void printstr(char *name, char *x);
	void printtime(char *name, num x);
	int gettimesinceboot();
	void printtimediff(char *name, num x);

#ifdef __cplusplus
}
#endif

#endif /* PROC_COMMON_H */
