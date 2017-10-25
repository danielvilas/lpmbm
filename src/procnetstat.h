/* 
 * File:   procnetstat.h
 * Author: Daniel
 *
 * Created on 10 de Julio de 2016, 19:34
 */

#ifndef PROCNETSTAT_H
#define PROCNETSTAT_H

#ifdef __cplusplus
"C" {
#endif
#include "proccommon.h"
	struct sIpExt {
		bool alive;
		num InNoRoutes;
		num InTruncatedPkts;
		num InMcastPkts;
		num OutMcastPkts;
		num InBcastPkts;
		num OutBcastPkts;
		num InOctets;
		num OutOctets;
		num InMcastOctets;
		num OutMcastOctets;
		num InBcastOctets;
		num OutBcastOctets;
	};

	typedef struct sIpExt tIpExt;
	
	int readProcNetstat(int pid, tIpExt* netInfo);
	
#ifdef __cplusplus
}
#endif

#endif /* PROCNETSTAT_H */