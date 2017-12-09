/*
 * Displays linux /proc/pid/stat in human-readable format
 *
 * Build: gcc -o procstat procstat.c
 * Usage: procstat pid
 *    cat /proc/pid/stat | procstat
 *
 * Homepage: http://www.brokestream.com/procstat.html
 * Version : 2009-03-05
 *
 * Ivan Tikhonov, http://www.brokestream.com, kefeer@netangels.ru
 *
 * 2007-09-19 changed HZ=100 error to warning
 *
 * 2009-03-05 tickspersec are taken from sysconf (Sabuj Pattanayek)
 *
 */


/* Copyright (C) 2009 Ivan Tikhonov

 This software is provided 'as-is', without any express or implied
 warranty. In no event will the authors be held liable for any damages
 arising from the use of this software.

 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it
 freely, subject to the following restrictions:

 1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
 2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
 3. This notice may not be removed or altered from any source distribution.

 Ivan Tikhonov, kefeer@brokestream.com

*/

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <linux/limits.h>
#include <sys/times.h>
#include <errno.h>
#include "procstat.h"

int readProcStat(int queryPid, tProcStat* info) {
	if (info == NULL) return -1;
	
	FILE *input;
	
	input = NULL;
	char filepath[160];
	int n = snprintf(filepath, 159, "/proc/%i/stat", queryPid);
	filepath[n] = 0;
	input = fopen(filepath, "r");
	if (!input) {
		info->alive = false;
		if (errno != ENOENT)
		{
			printf("Error is %i\n", errno);
			perror(filepath);	
		}
		return 1;
	}
	info->alive = true;
  
	readone(input, &info->pid);
	readstr(input, info->tcomm);
	readchar(input, &info->state);
	readone(input, &info->ppid);
	readone(input, &info->pgid);
	readone(input, &info->sid);
	readone(input, &info->tty_nr);
	readone(input, &info->tty_pgrp);
	readone(input, &info->flags);
	readone(input, &info->min_flt);
	readone(input, &info->cmin_flt);
	readone(input, &info->maj_flt);
	readone(input, &info->cmaj_flt);
	readone(input, &info->utime); //utime
	readone(input, &info->stimev);//stime
	readone(input, &info->cutime);//cutime
	readone(input, &info->cstime); //cstime
	readone(input, &info->priority);
	readone(input, &info->nicev);
	readone(input, &info->num_threads);
	readone(input, &info->it_real_value);
	readunsigned(input, &info->start_time); //start_time
	readone(input, &info->vsize);
	readone(input, &info->rss);
	readone(input, &info->rsslim);
	readone(input, &info->start_code);
	readone(input, &info->end_code);
	readone(input, &info->start_stack);
	readone(input, &info->esp);
	readone(input, &info->eip);
	readone(input, &info->pending);
	readone(input, &info->blocked);
	readone(input, &info->sigign);
	readone(input, &info->sigcatch);
	readone(input, &info->wchan);
	readone(input, &info->zero1);
	readone(input, &info->zero2);
	readone(input, &info->exit_signal);
	readone(input, &info->cpu);
	readone(input, &info->rt_priority);
	readone(input, &info->policy);
	fclose(input);
	info->timesinceboot = gettimesinceboot();
	return 1;
}

void printLastInfo(tProcStat* info) {

	printone("pid", info->pid);
	printstr("tcomm", info->tcomm);
	printchar("state", info->state);
	printone("ppid", info->ppid);
	printone("pgid", info->pgid);
	printone("sid", info->sid);
	printone("tty_nr", info->tty_nr);
	printone("tty_pgrp", info->tty_pgrp);
	printone("flags", info->flags);
	printone("min_flt", info->min_flt);
	printone("cmin_flt", info->cmin_flt);
	printone("maj_flt", info->maj_flt);
	printone("cmaj_flt", info->cmaj_flt);
	printtime("utime", info->utime);
	printtime("stime", info->stimev);
	printtime("cutime", info->cutime);
	printtime("cstime", info->cstime);
	printone("priority", info->priority);
	printone("nice", info->nicev);
	printone("num_threads", info->num_threads);
	printtime("it_real_value", info->it_real_value);
	printtimediff("start_time", info->start_time);
	printone("vsize", info->vsize);
	printone("rss", info->rss);
	printone("rsslim", info->rsslim);
	printone("start_code", info->start_code);
	printone("end_code", info->end_code);
	printone("start_stack", info->start_stack);
	printone("esp", info->esp);
	printone("eip", info->eip);
	printonex("pending", info->pending);
	printonex("blocked", info->blocked);
	printonex("sigign", info->sigign);
	printonex("sigcatch", info->sigcatch);
	printone("wchan", info->wchan);
	printone("zero1", info->zero1);
	printone("zero2", info->zero2);
	printonex("exit_signal", info->exit_signal);
	printone("cpu", info->cpu);
	printone("rt_priority", info->rt_priority);
	printone("policy", info->policy);
}

int readProcName(int queryPid, char* buff, int size)
{
	char buffer[4096];
	FILE* fd;
	ssize_t ret;

	buff[0] = '\0';
	snprintf(buffer, size, "/proc/%d/comm", queryPid);
	if ((fd = fopen(buffer, "r")) < 0) {
		return -1;
	}
	if ((ret = fread(buff,1,size,fd))<=0) {
		(void)fclose(fd);
		return -1;
	}
	(void)fclose(fd);
	buff[ret - 1] = '\0';		/* remove trailing '\n' */
	return ret;
}
