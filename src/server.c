/**
 * Main routine
 *
 * @author Steffen Vogel <stvogel@eonerc.rwth-aachen.de>
 * @copyright 2014, Institute for Automation of Complex Power Systems, EONERC
 */

#include <stdlib.h>
#include <stdio.h>
#include <error.h>

#include <sched.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "config.h"
#include "cfg.h"
#include "msg.h"
#include "utils.h"
#include "path.h"
#include "node.h"

/** Linked list of nodes */
static struct node *nodes;

/** Linked list of paths */
static struct path *paths;

/** Default settings */
static struct settings settings = {
	.priority = 0,
	.affinity = 0xC0,
	.protocol = 0
};

static config_t config;

static void start()
{
	/* Connect and bind nodes to their sockets, set socket options */
	for (struct node *n = nodes; n; n = n->next) {
		node_connect(n);

		debug(1, "  We listen for node '%s' at %s:%u", n->name, inet_ntoa(n->local.sin_addr), ntohs(n->local.sin_port));
		debug(1, "  We sent to node '%s' at %s:%u", n->name, inet_ntoa(n->remote.sin_addr), ntohs(n->remote.sin_port));
	}

	/* Start on thread per path for asynchronous processing */
	for (struct path *p = paths; p; p = p->next) {
		path_start(p);

		info("Starting path: %12s => %s => %-12s", p->in->name, settings.name, p->out->name);
	}
}

static void stop()
{
	/* Join all threads and print statistics */
	for (struct path *p = paths; p; p = p->next) {
		path_stop(p);

		info("Stopping path: %12s => %s => %-12s", p->in->name, settings.name, p->out->name);
		info("  %u messages received", p->received);
		info("  %u messages duplicated", p->duplicated);
		info("  %u messages delayed", p->delayed);
	}

	/* Close all sockets we listing on */
	for (struct node *n = nodes; n; n = n->next) {
		node_disconnect(n);
	}
}

static void quit()
{
	stop();

	// TODO: free nodes and paths

	config_destroy(&config);

	_exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
	/* Setup signals */
	struct sigaction sa_quit = {
		.sa_flags = SA_SIGINFO,
		.sa_sigaction = quit
	};

	sigemptyset(&sa_quit.sa_mask);
	sigaction(SIGTERM, &sa_quit, NULL);
	sigaction(SIGINT, &sa_quit, NULL);
	atexit(&quit);

	/* Check arguments */
	if (argc != 2) {
		printf("Usage: %s CONFIG\n", argv[0]);
		printf("  CONFIG is a required path to a configuration file\n\n");
		printf("Simulator2Simulator Server %s (%s %s)\n", VERSION, __DATE__, __TIME__);
		printf(" Copyright 2014, Institute for Automation of Complex Power Systems, EONERC\n");
		printf("   Steffen Vogel <stvogel@eonerc.rwth-aachen.de>\n\n");
		exit(EXIT_FAILURE);
	}

	info("This is s2ss %s", VERSION);

	/* Parse configuration file */
	config_init(&config);
	config_parse(argv[1], &config, &settings, &nodes, &paths);

	if (!paths)
		error("No paths found. Terminating...");

	/* Lock memory */
	/*if(mlockall(MCL_CURRENT | MCL_FUTURE))
		perror("Failed mlockall");
	else
		debug(3, "Locked memory");*/

	/* Prefault stack */
	/*char dummy[MAX_SAFE_STACK];
	memset(dummy, 0, MAX_SAFE_STACK);
	debug(3, "Prefaulted stack");*/

	/* Check for realtime kernel patch */
	struct stat st;
	if (stat("/sys/kernel/realtime", &st))
		warn("This is not a a realtime patched kernel!");
	else
		debug(3, "This is a realtime patched kernel");

	/* Use FIFO scheduler with realtime priority */
	struct sched_param param;
	param.sched_priority = settings.priority;
	if (sched_setscheduler(0, SCHED_FIFO, &param))
		perror("Failed to set realtime priority");
	else
		debug(3, "Set task priority to %u", settings.priority);

	/* Pin threads to CPUs by setting the affinity */
	cpu_set_t cset = to_cpu_set_t(settings.affinity);
	pid_t pid = getpid();
	if (sched_setaffinity(pid, sizeof(cset), &cset))
		perror("Failed to set CPU affinity");
	else
		debug(3, "Set affinity to %#x", settings.affinity);

	/* Connect all nodes and start one thread per path */
	start();

	/* Process is running as root, drop privileges */
	if (getuid() == 0) {
		if (setgid(settings.gid) || setuid(settings.uid))
			perror("Unable to drop privileges");
		else
			debug(3, "Dropped privileges to uid = %u, gid = %u",
				settings.uid, settings.gid);
	}

	/* Main thread is sleeping */
	pause();

	return 0;
}
