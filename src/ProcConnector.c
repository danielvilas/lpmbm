//http://netsplit.com/the-proc-connector-and-socket-filters
//http://bewareofgeek.livejournal.com/2945.html
//http://stackoverflow.com/questions/6075013/detect-launching-of-programs-on-linux-platform

/* This file is licensed under the GPL v2 (http://www.gnu.org/licenses/gpl2.txt) (some parts was originally borrowed from proc events example)

pmon.c

code highlighted with GNU source-highlight 3.1
*/

#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/connector.h>
#include <linux/cn_proc.h>
#include <signal.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "procstat.h"

/*
* connect to netlink
* returns netlink socket, or -1 on error
*/
static int nl_connect()
{
	int rc;
	int nl_sock;
	struct sockaddr_nl sa_nl;

	nl_sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_CONNECTOR);
	if (nl_sock == -1) {
		perror("socket");
		return -1;
	}

	sa_nl.nl_family = AF_NETLINK;
	sa_nl.nl_groups = CN_IDX_PROC;
	sa_nl.nl_pid = getpid();

	rc = bind(nl_sock, (struct sockaddr *)&sa_nl, sizeof(sa_nl));
	if (rc == -1) {
		perror("bind");
		close(nl_sock);
		return -1;
	}

	return nl_sock;
}

/*
* subscribe on proc events (process notifications)
*/
static int set_proc_ev_listen(int nl_sock, bool enable)
{
	int rc;
	struct __attribute__((aligned(NLMSG_ALIGNTO))) {
		struct nlmsghdr nl_hdr;
		struct __attribute__((__packed__)) {
			struct cn_msg cn_msg;
			enum proc_cn_mcast_op cn_mcast;
		};
	} nlcn_msg;

	memset(&nlcn_msg, 0, sizeof(nlcn_msg));
	nlcn_msg.nl_hdr.nlmsg_len = sizeof(nlcn_msg);
	nlcn_msg.nl_hdr.nlmsg_pid = getpid();
	nlcn_msg.nl_hdr.nlmsg_type = NLMSG_DONE;

	nlcn_msg.cn_msg.id.idx = CN_IDX_PROC;
	nlcn_msg.cn_msg.id.val = CN_VAL_PROC;
	nlcn_msg.cn_msg.len = sizeof(enum proc_cn_mcast_op);

	nlcn_msg.cn_mcast = enable ? PROC_CN_MCAST_LISTEN : PROC_CN_MCAST_IGNORE;

	rc = send(nl_sock, &nlcn_msg, sizeof(nlcn_msg), 0);
	if (rc == -1) {
		perror("netlink send");
		return -1;
	}

	return 0;
}

/*
* handle a single process event
*/
static volatile bool need_exit = false;
static int handle_proc_ev(int nl_sock, char* cmd)
{
	int rc;
	struct __attribute__((aligned(NLMSG_ALIGNTO))) {
		struct nlmsghdr nl_hdr;
		struct __attribute__((__packed__)) {
			struct cn_msg cn_msg;
			struct proc_event proc_ev;
		};
	} nlcn_msg;

	while (!need_exit) {
		rc = recv(nl_sock, &nlcn_msg, sizeof(nlcn_msg), 0);
		if (rc == 0) {
		/* shutdown? */
			return 0;
		}
		else if (rc == -1) {
			if (errno == EINTR) continue;
			perror("netlink recv");
			return -1;
		}
		char buff[4096];
		if (nlcn_msg.proc_ev.what == PROC_EVENT_NONE)
		{
			printf("Registed OK\n");
		}
		else if (nlcn_msg.proc_ev.what == PROC_EVENT_EXEC)
		{
			readProcName(nlcn_msg.proc_ev.event_data.exec.process_pid, buff, sizeof(buff));
			printf("exec: tid=%d pid=%d cmd=%s\n",
				nlcn_msg.proc_ev.event_data.exec.process_pid,
				nlcn_msg.proc_ev.event_data.exec.process_tgid,
				buff);
			if (strcmp(buff, cmd) == 0)
			{
				printf("Found...\n");
				return nlcn_msg.proc_ev.event_data.exec.process_pid;
			}
		}
	}

	return 0;
}


int waitForCommand(char* cmd)
{
	int nl_sock;
	int rc = EXIT_SUCCESS;
	printf("Connecting to Kernel...\n");
	nl_sock = nl_connect();
	if (nl_sock == -1)
		exit(EXIT_FAILURE);

	printf("Self Registering...\n");
	rc = set_proc_ev_listen(nl_sock, true);
	if (rc == -1) {
		rc = EXIT_FAILURE;
		goto out;
	}
	
	printf("Waiting for: %s...\n",cmd);
	rc = handle_proc_ev(nl_sock,cmd);
	if (rc == -1) {
		rc = EXIT_FAILURE;
		goto out;
	}

	set_proc_ev_listen(nl_sock, false);

out:
	close(nl_sock);
	return rc;
}