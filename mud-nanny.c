/* MUD Nanny
 * Sean Middleditch / AwesomePlay Productions, Inc.
 * elanthis@awemud.net
 * THIS CODE IS PUBLIC DOMAIN
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int
main (int argc, char** argv)
{
	pid_t child;
	time_t start, end;
	const char* path;
	const char* script;
	int status;
	int ret;
	char time_buf[48];
	const char* ret_type;
	char ret_buf[48];

	/* Parse arguments */
	if (argc != 3) {
		fprintf(stderr, "mud-nanny <server> <script>\n");
		exit(1);
	}

	path = argv[1];
	script = argv[2];

	/* Daemonize */

	ret = fork();
	if (ret == -1) {
		fprintf(stderr, "fork() failed: %s\n", strerror(errno));
		exit(1);
	}
	if (ret != 0) {
		exit(0);
	}

	if (setsid() == -1) {
		fprintf(stderr, "setsid() failed: %s\n", strerror(errno));
		exit(1);
	}

	ret = fork();
	if (ret == -1) {
		fprintf(stderr, "fork() failed: %s\n", strerror(errno));
		exit(1);
	}
	if (ret != 0) {
		exit(0);
	}

	close(0);
	if (open("/dev/null", O_RDONLY) == -1) {
		fprintf(stderr, "open() failed: %s\n", strerror(errno));
		exit(1);
	}
	close(1);
	if (open("/dev/null", O_WRONLY) == -1) {
		fprintf(stderr, "open() failed: %s\n", strerror(errno));
		exit(1);
	}
	close(2);
	if (open("/dev/null", O_WRONLY) == -1) {
		exit(1);
	}

	/* Main loop */
	while(1) {
		/* Run MUD server */
		time(&start);
		if ((child = fork()) == 0) {
			execl(path, path, NULL);

			/* execl() failed */
			exit(errno);
		}

		/* Wait for server to terminate */
		do {
			ret = waitpid(child, &status, 0);
		} while(ret == -1 && errno == EINTR);
		if (ret == -1) {
			fprintf(stderr, "waitpid() failed: %s\n", strerror(errno));
			exit(1);
		}

		/* Record run time */
		time(&end);
		snprintf(time_buf, sizeof(time_buf), "%u", end-start);

		/* Record shutdown manner */
		if (WIFEXITED(status)) {
			ret_type = "exit";
			snprintf(ret_buf, sizeof(ret_buf), "%d", WEXITSTATUS(status));
		} else if (WIFSIGNALED(status)) {
			ret_type = "signal";
			snprintf(ret_buf, sizeof(ret_buf), "%d", WTERMSIG(status));
		} else {
			ret_type = "unknown";
			snprintf(ret_buf, sizeof(ret_buf), "%d", status);
		}

		/* Invoke logic script */
		if ((child = fork()) == 0) {
			execl(script, script, ret_type, ret_buf, time_buf, path, NULL);

			/* execl() failed */
			exit(99);
		}

		/* Wait for script to terminate */
		do {
			ret = waitpid(child, &status, 0);
		} while(ret == -1 && errno == EINTR);
		if (ret == -1) {
			fprintf(stderr, "waitpid() failed: %s\n", strerror(errno));
			exit(1);
		}

		/* Ensure it was a normal exit */
		if (!WIFEXITED(status)) {
			fprintf(stderr, "%s terminated abnormally\n", script);
			exit(1);
		}

		/* If we have a non-zero return, finish */
		if (WEXITSTATUS(status) != 0) {
			exit(0);
		}
	}

	return 0;
}
