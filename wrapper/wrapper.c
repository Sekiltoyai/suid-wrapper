/**
 * Copyright (c) 2013, Emmanuel Thierry <emmanuel.thierry@sekil.fr>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the Emmanuel Thierry nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */


#include <errno.h>
#include <stdlib.h>

#ifdef DEBUG
#include <stdio.h>
#endif

#ifdef CHK_ARGS_REGEXP
#include <string.h>
#include <regex.h>
#include <sys/types.h>
#endif

#ifdef DO_CAPABILITIES
#include <sys/capability.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <linux/securebits.h>
#endif



#ifdef DEBUG
#define EXIT_CHK(error) \
	{fprintf(stderr, "Failed to pass %s test with errno %d\n", error, errno); exit(EPERM);}
#define EXIT_DO(error) \
	{fprintf(stderr, "Failed to grant %s privilege with errno %d\n", error, errno); exit(EPERM);}
#else
#define EXIT_CHK(error) \
	exit(EPERM)
#define EXIT_DO(error) \
	exit(EPERM)
#endif

#ifdef CHK_ARGS_REGEXP
const char const *args_regexp[] = { CHK_ARGS_REGEXP };
#define _ARG_MAX (sizeof(args_regexp) / sizeof(char *))
#endif

#ifdef DO_CAPABILITIES
cap_value_t capabilities[] = { DO_CAPABILITIES };
#define _CAP_MAX (sizeof(capabilities) / sizeof(cap_value_t))
#endif

char * const exec_args[] = { EXEC_ARGS };
#define _EXEC_MAX (sizeof(exec_args) / sizeof(char *))


int main(int argc, char * const argv[], char * const envp[])
{
	char *new_argv[_EXEC_MAX + argc + 1];
	int i;
#ifdef DO_CAPABILITIES
	cap_t caps, empty;
#endif


#ifdef CHK_WRAPPER_PATH
	if (strcmp(argv[0], CHK_WRAPPER_PATH))
		EXIT_CHK("CHK_WRAPPER_PATH");
#endif

#ifdef CHK_ARGC_MIN
	if (argc < CHK_ARGC_MIN)
		EXIT_CHK("CHK_ARGC_MIN");
#endif

#ifdef CHK_ARGC_MAX
	if (argc > CHK_ARGC_MAX)
		EXIT_CHK("CHK_ARGC_MAX");
#endif

#ifdef CHK_REAL_UID
	if (getuid() != CHK_REAL_UID)
		EXIT_CHK("CHK_REAL_UID");
#endif

#ifdef CHK_REAL_GID
	if (getgid() != CHK_REAL_GID)
		EXIT_CHK("CHK_REAL_GID");
#endif

	new_argv[0] = EXEC_PATH;
	for (i=1; i<_EXEC_MAX+1; i++) {
		new_argv[i] = exec_args[i-1];
	}
	for (; i<_EXEC_MAX+argc; i++) {
#ifdef CHK_ARGS_REGEXP
		if ((i < _EXEC_MAX + _ARG_MAX + 1) && args_regexp[i-_EXEC_MAX-1]) {
			regex_t preg;
			memset(&preg, 0, sizeof(preg));
			if (regcomp(&preg, args_regexp[i-_EXEC_MAX-1], CHK_ARGS_REGEXP_FLAGS))
				EXIT_CHK("CHK_ARGS_REGEXP(regcomp)");
			if (regexec(&preg, argv[i-_EXEC_MAX], 0, NULL, 0))
				EXIT_CHK("CHK_ARGS_REGEXP(regexec)");
			regfree(&preg);
		}
#endif
		new_argv[i] = argv[i-_EXEC_MAX];
	}
	new_argv[i] = NULL;

#ifdef DO_CAPABILITIES
	if (setuid(0))
		EXIT_DO("DO_CAPABILITIES(setuid)");
	if (prctl(PR_SET_SECUREBITS, SECBIT_KEEP_CAPS, 0, 0, 0))
		EXIT_DO("DO_CAPABILITIES(prctl)");
	caps = cap_init();
	if (cap_set_flag(caps, CAP_EFFECTIVE, _CAP_MAX, capabilities, CAP_SET) ||
	    cap_set_flag(caps, CAP_PERMITTED, _CAP_MAX, capabilities, CAP_SET) ||
	    cap_set_flag(caps, CAP_INHERITABLE, _CAP_MAX, capabilities, CAP_SET) ||
	    cap_set_proc(caps))
		EXIT_DO("DO_CAPABILITIES(cap_set_proc");
	cap_free(caps);
#endif

#ifdef DO_SETGID
	if (setgid(DO_SETGID))
		EXIT_DO("DO_SETGID");
#endif

#ifdef DO_SETUID
	if (setuid(DO_SETUID))
		EXIT_DO("DO_SETUID");
#endif

#ifdef DEBUG
	fprintf(stderr, "All checks passed\n");
	fprintf(stderr, "%s", EXEC_PATH);
	for (i=0; new_argv[i]; i++)
		fprintf(stderr, " \"%s\"", new_argv[i]);
	fprintf(stderr, "\n");
#endif
	execv(EXEC_PATH, new_argv);
}
