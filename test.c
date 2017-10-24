#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "sysexec.h"

static void test_sysexec_generic(const char *func, int is_env)
{
	int ret;
	char *env[] = {
		"ILAN=",
		NULL
	};

	printf("testing: %s()\n", func + strlen("test_"));

	setenv("ILAN", "nice", 0);
	printf("parent: ILAN=%s\n", getenv("ILAN"));

	ret = is_env ? sysexece(env, "echo child: ILAN=$ILAN") :
		sysexec("echo child: ILAN=$ILAN");

	if (ret) {
		printf("%s() failed\n", __FUNCTION__);
		return;
	}

	printf("parent: ILAN=%s\n", getenv("ILAN"));
	printf("\n");
}

static void test_sysexec(void)
{
	test_sysexec_generic(__FUNCTION__, 0);
}

static void test_sysexece(void)
{
	test_sysexec_generic(__FUNCTION__, 1);
}

static void test_sysexecp_generic(const char *func, int is_env)
{
	int ret;
	char buf[128];
	char *env[] = {
		"ILAN=",
		NULL
	};

	printf("testing: %s()\n", func + strlen("test_"));

	setenv("ILAN", "nice", 0);
	printf("parent: ILAN=%s\n", getenv("ILAN"));

	ret = is_env ? sysexecpe(buf, sizeof(buf), env, "echo ILAN=$ILAN") :
		sysexecp(buf, sizeof(buf), "echo ILAN=$ILAN");

	if (ret) {
		printf("%s() failed\n", __FUNCTION__);
		return;
	}

	printf("child returned: %s\n", buf);
	printf("parent: ILAN=%s\n", getenv("ILAN"));
	printf("\n");
}

static void test_sysexecp(void)
{
	test_sysexecp_generic(__FUNCTION__, 0);
}

static void test_sysexecpe(void)
{
	test_sysexecp_generic(__FUNCTION__, 1);
}

int main(int argc, char **argv)
{
	test_sysexec();
	test_sysexece();
	test_sysexecp();
	test_sysexecpe();

	return 0;
}

