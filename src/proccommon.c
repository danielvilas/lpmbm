#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <linux/limits.h>
#include <sys/times.h>
#include <errno.h>
#include "proccommon.h"

long tickspersec;

void readone(FILE *input, num *x) { fscanf(input, "%lld ", x); }
void readunsigned(FILE *input, unsigned long long *x) { fscanf(input, "%llu ", x); }
void readstr(FILE *input, char *x) { fscanf(input, "%s ", x); }
void readchar(FILE *input, char *x) { fscanf(input, "%c ", x); }

void printone(char *name, num x) { printf("%20s: %lld\n", name, x); }
void printonex(char *name, num x) { printf("%20s: %016llx\n", name, x); }
void printunsigned(char *name, unsigned long long x) { printf("%20s: %llu\n", name, x); }
void printchar(char *name, char x) { printf("%20s: %c\n", name, x); }
void printstr(char *name, char *x) { printf("%20s: %s\n", name, x); }
void printtime(char *name, num x) { printf("%20s: %f\n", name, (((double)x) / tickspersec)); }




int gettimesinceboot() {
	FILE *procuptime;
	int sec, ssec;

	procuptime = fopen("/proc/uptime", "r");
	fscanf(procuptime, "%d.%ds", &sec, &ssec);
	fclose(procuptime);
	return (sec*tickspersec) + ssec;
}

void printtimediff(char *name, num x) {
	int sinceboot = gettimesinceboot();
	int running = sinceboot - x;
	time_t rt = time(NULL) - (running / tickspersec);
	char buf[1024];

	strftime(buf, sizeof(buf), "%m.%d %H:%M", localtime(&rt));
	printf("%20s: %s (%u.%us)\n", name, buf, running / tickspersec, running % tickspersec);
}
