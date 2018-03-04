#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "procstat.h"
#include "procnetstat.h"
#include "ina219.h"
#include "GpioCtl.h"
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>


static volatile bool need_exit = false;

void monitorPid(int pid, bool i2c, char* name);
int forkCommand(int start_index, int argc, char *argv[]);
int waitForCommand(char* cmd);

long long current_timestamp() {
	struct timeval te; 
	gettimeofday(&te, NULL); // get current time
	long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000; // caculate milliseconds
	// printf("milliseconds: %lld\n", milliseconds);
	return milliseconds;
}

void printUsage(char* command)
{
	printf("usage: %s -i <-s|-w> cmd cmd_params ...\n", command);
}

static void on_sigint(int unused)
{
	printf("Cleaning and exiting\n");
	need_exit = true;
}

int main(int argc, char *argv[])
{
	int start_index = -1;
	char start_mode = 0;
	bool i2c=false;
	if (argc <= 1)
	{
		printUsage(argv[0]);
		exit(-1);
	}
	else
	{
		for (int i = 0; i < argc; i++)
		{
			printf("argv[%i]: %s\n", i, argv[i]);
			if (strcmp("-s", argv[i]) == 0)
			{
				if (start_mode != 0)
				{
					printUsage(argv[0]);
					exit(-1);	
				}
				start_index = i + 1;
				start_mode = 's';
				
			}
			if (strcmp("-w", argv[i]) == 0)
			{
				if (start_mode != 0)
				{
					printUsage(argv[0]);
					exit(-1);	
				}
				start_index = i + 1;
				start_mode = 'w';
			}
			if (strcmp("-i", argv[i]) == 0)
			{
				start_index = i + 1;
				i2c=true;
			}
		}
		if (start_index == -1 || start_index == argc)
		{
			printUsage(argv[0]);
			exit(-1);
		}
	}

	if(i2c){
		init_ina_i2c();
	}
	
	signal(SIGINT, &on_sigint);
	siginterrupt(SIGINT, true);
	signal(SIGTERM, &on_sigint);
	siginterrupt(SIGTERM, true);

	GPIOExport(POUT);
	GPIODirection(POUT,OUT);
    GPIOWrite(POUT,0);
    GPIOWrite(POUT,1);
    GPIOWrite(POUT,0);
    GPIOWrite(POUT,1);

    int pid = 0;
	if (start_mode == 's')
	{
		pid = forkCommand(start_index, argc, argv);
	}
	else if (start_mode == 'w')
	{
		pid = waitForCommand(argv[start_index]);
	}
	if (pid > 0)
	{
		monitorPid(pid, i2c,argv[start_index]);
	}
	else
	{
		exit(pid);
	}
	

}

int checkRunning(tProcStat* info, bool isChild)
{
	int status=0;
	int pid = info->pid;
	switch (info->state)
	{//Casos conocidos que estan funcionando
	case 'S': //Sleep
	case 'D': //Wait Sleep
	case 'R': //Running 
		status = 0;
		return status;
	//casos conocidos que la app esta parada
	case 'Z':
		status = -1;
		return status;
	
	}
	printf("Check Running (%i) for %c\n", pid, info->state);
	if (isChild) {
		waitpid(pid, &status, WNOHANG);
	}
	else
	{
		status = kill(pid, 0);
		if (status == -1 && errno == EPERM)
		{
			status = 0;
		}	
	}
	return status;
}

void monitorPid(int pid, bool i2c, char* name)
{
	tickspersec = sysconf(_SC_CLK_TCK);
    char *ptr = strrchr( name, '/' );
	if(ptr==NULL){
        ptr=name;
    } else{
        ptr++;
    }

	char filepath[160];
	int n = snprintf(filepath, 159, "/tmp/monitor_%s_%i.out",ptr, pid);
	filepath[n] = 0;
	FILE* out = fopen(filepath, "w");
    if(out==NULL){
        perror(filepath);
        exit(-1);
    }
	printf("Monitors out to: %s\n", filepath);

fprintf(out, "currentTs;total_time;user_time;sys_time;virtual_size;in_octets;out_octets");
	if(i2c)fprintf(out, ";current");
	fprintf(out, "\n");
	tProcStat info;
	tIpExt netinfo;
	bool running = true;
	int status = 0;
	bool isChild = waitpid(pid, &status, WNOHANG) == 0;
	bool pin = false;
    long long start = current_timestamp();
	while (running && !need_exit)
	{	
		long long currentTs = current_timestamp();
		readProcStat(pid, &info); 
		if (info.alive)
		{
			status = 0;
			if (info.state != 'R')
			{
				status = checkRunning(&info, isChild);
			}
		}
		else
		{
			status = 1;
		}
		if (status == 0)
		{ 
			readProcNetstat(pid, &netinfo);
			num total_utime = info.utime + info.cutime;
			num total_stime = info.stimev + info.cstime;
			num total_time = info.timesinceboot - info.start_time;
			num total_in_octets = netinfo.InOctets; //+ netinfo.InBcastOctets + netinfo.InMcastOctets
			num total_out_octets = netinfo.OutOctets; //+ netinfo.OutBcastOctets + netinfo.OutMcastOctets
			
    
			fprintf(out, "%lld;%lld;%lld;%lld;%lld;%lld;%lld", currentTs , total_time, total_utime, total_stime, info.vsize, total_in_octets, total_out_octets);
			if(i2c){
				fprintf(out,";%f",readCurrent());
			}
			fprintf(out, "\n");
			GPIOWrite(POUT,pin?1:0);
			pin=!pin;
			long wait = 100 + currentTs - current_timestamp();
			if(wait>0) //Prevents infinite wait on debug
				usleep(wait * 1000);
		}
		else
		{
			running = false;
		}

	}
    long long end = current_timestamp();
    printf("Run Time: %lld ms\n", end - start);
	fclose(out);
	printf("Monitored data written to: %s\n", filepath);
	GPIOWrite(POUT,0);
    GPIOWrite(POUT,1);
    GPIOWrite(POUT,0);
    GPIOWrite(POUT,1);

    GPIOUnexport(POUT);
	exit(0);
}

int forkCommand(int start_index, int argc, char *argv[])
{
	int pid = vfork();
	if (pid < 0)
	{
		printf("Error fork");
		exit(pid);
	}
	else if (pid == 0)
	{
		int nArgs = argc - start_index + 1;
		char** args = malloc(nArgs*sizeof(char*));
		printf("Exec: ");
		for (int i = 0; i < nArgs - 1; i++)
		{
			args[i] = argv[i + start_index];
			printf("%s ", args[i]);
		}
		printf("\n");
		args[nArgs - 1] = NULL;
		execvp(args[0], args);
	}
	else
	{
		return pid;
	}
	return 0;
}
