/* 
 * File:   procstat.h
 * Author: Daniel
 *
 * Created on 18 de abril de 2016, 19:06
 */

#ifndef PROCSTAT_H
#define PROCSTAT_H

#ifdef __cplusplus
"C" {
#endif
#include "proccommon.h"
	struct sProcStat
	{
		bool alive;
		num pid;
		char tcomm[PATH_MAX];
		char state;

		num ppid;
		num pgid;
		num sid;
		num tty_nr;
		num tty_pgrp;

		num flags;
		num min_flt;
		num cmin_flt;
		num maj_flt;
		num cmaj_flt;
		num utime;
		num stimev;

		num cutime;
		num cstime;
		num priority;
		num nicev;
		num num_threads;
		num it_real_value;

		unsigned long long start_time;

		num vsize;
		num rss;
		num rsslim;
		num start_code;
		num end_code;
		num start_stack;
		num esp;
		num eip;

		num pending;
		num blocked;
		num sigign;
		num sigcatch;
		num wchan;
		num zero1;
		num zero2;
		num exit_signal;
		num cpu;
		num rt_priority;
		num policy;
		int timesinceboot;
	};
 
	typedef struct sProcStat tProcStat;

	int readProcStat(int queryPid, tProcStat* info);
	int readProcName(int queryPid, char* buff, int size);

#ifdef __cplusplus
}
#endif

#endif /* PROCSTAT_H */

