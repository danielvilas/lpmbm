#include "procnetstat.h"
#include <errno.h>

void skipLine(FILE *input) { fscanf(input, "%*[^\n]\n", NULL); }

int readProcNetstat(int queryPid, tIpExt* netInfo)
{

	if (netInfo == NULL) return -1;
	
	FILE *input;
	
	input = NULL;
	char filepath[160];
	int n = snprintf(filepath, 159, "/proc/%i/net/netstat", queryPid);
	filepath[n] = 0;
	input = fopen(filepath, "r");
	if (!input) {
		netInfo->alive = false;
		printf("Error is %i\n", errno);
		perror(filepath);
		return 1;
	}
	netInfo->alive = true;
	char tmp[10];//"IpExt:"
	//readstr(input, tmp);printf("%s\n", tmp);
	skipLine(input);
	//readstr(input, tmp); printf("%s\n", tmp);
	skipLine(input);
	//readstr(input, tmp); printf("%s\n", tmp);
	skipLine(input);
	readstr(input, tmp);// printf("%s\n", tmp);
	
	readone(input, &netInfo->InNoRoutes);
	readone(input, &netInfo->InTruncatedPkts);
	readone(input, &netInfo->InMcastPkts);
	readone(input, &netInfo->OutMcastPkts);
	readone(input, &netInfo->InBcastPkts);
	readone(input, &netInfo->OutBcastPkts);
	readone(input, &netInfo->InOctets);
	readone(input, &netInfo->OutOctets);
	readone(input, &netInfo->InMcastOctets);
	readone(input, &netInfo->OutMcastOctets);
	readone(input, &netInfo->InBcastOctets);
	readone(input, &netInfo->OutBcastOctets);
	fclose(input);
}
