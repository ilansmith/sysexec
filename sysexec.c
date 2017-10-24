#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_CMD_LEN 512
#define ENV_SHELL "SHELL"

#define SYSEXEC_ERROR(e) do { \
	sysexec_error(__FUNCTION__, e); \
} while (0)

enum err_sysexec {
	ERR_SYSEXEC_PIPE,
	ERR_SYSEXEC_EINVAL,
	ERR_SYSEXEC_INTERNAL,
	ERR_SYSEXEC_LAST,
};

static void sysexec_error(const char *func, enum err_sysexec err)
{
	static char *err2str[ERR_SYSEXEC_LAST] = {
		[ ERR_SYSEXEC_PIPE ] = "Pipe creation error",
		[ ERR_SYSEXEC_EINVAL ] = "Invalid argument",
		[ ERR_SYSEXEC_INTERNAL ] = "Internal error",
	};

	if (err == ERR_SYSEXEC_LAST) {
		fprintf(stderr, "badd error value: %d\n", err);
		return;
	}

	fprintf(stderr, "%s(): %s\n", func, err2str[err]);
}

static int __sysexec(char **env, char *flags, char *fmt, va_list ap)
{
	int status = 0;
	int pipefd[2];
	int fd_parent = -1;
	int fd_child = -1;
	int fdio = -1;
	int is_write;

	if (flags) {
		if (pipe(pipefd)) {
			SYSEXEC_ERROR(ERR_SYSEXEC_PIPE);
			return -1;
		}

		if (!strncmp(flags, "r", strlen(flags))) {
			is_write = 0;
		} else if (!strncmp(flags, "w", strlen(flags))) {
			is_write = 1;
		} else {
			SYSEXEC_ERROR(ERR_SYSEXEC_EINVAL);
			return -1;
		}

		fd_parent = pipefd[is_write];
		fd_child = pipefd[!is_write];
		fdio = !is_write;
	}

	if (!fork()) { /* child process */
		char cmd[MAX_CMD_LEN];
		char *args[4];
		int ret;

		bzero(cmd, MAX_CMD_LEN);
		ret = vsnprintf(cmd, MAX_CMD_LEN, fmt, ap);

		if (ret < 0) {
			SYSEXEC_ERROR(ERR_SYSEXEC_INTERNAL);
			exit(-1);
		}

		args[0] = getenv(ENV_SHELL);
		args[1] = "-c";
		args[2] = cmd;
		args[3] = NULL;

		if (flags) {
			close(fd_parent);
			ret = dup2(fd_child, fdio);
			close(fd_child);

			if (ret != fdio) {
				SYSEXEC_ERROR(ERR_SYSEXEC_INTERNAL);
				exit(-1);
			}
		}

		if (!env)
			env = environ;
		execvpe(args[0], args, env);
	}

	/* parent process */

	close(fd_child);
	wait(&status);

	if (!WIFEXITED(status) || WEXITSTATUS(status) == -1) {
		/* the child process did not exit gracefully */
		close(fd_parent);
		return -1;
	}

	return flags ? fd_parent : 0;
}

int sysexece(char **env, char *fmt, ...)
{
	va_list ap;
	int ret;

	va_start(ap, fmt);
	ret = __sysexec(env, NULL, fmt, ap);
	va_end(ap);

	return ret;
}

int sysexec(char *fmt, ...)
{
	va_list ap;
	int ret;

	va_start(ap, fmt);
	ret = __sysexec(NULL, NULL, fmt, ap);
	va_end(ap);

	return ret;
}

int sysexecpe(char *buf, int count, char **env, char *fmt, ...)
{
	va_list ap;
	char *eol;
	int fd;

	va_start(ap, fmt);
	fd = __sysexec(env, "r", fmt, ap);
	va_end(ap);

	if (fd == -1)
		return -1;

	read(fd, buf, count);
	close(fd);

	if ((eol = strchr(buf, '\n')))
		*eol = 0;

	return 0;
}

int sysexecp(char *buf, int count, char *fmt, ...)
{
	va_list ap;
	int ret;

	va_start(ap, fmt);
	ret = sysexecpe(buf, count, NULL, fmt, ap);
	va_end(ap);

	return ret;
}

