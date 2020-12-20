/*
 * dtxsys.h - D-Type's extra system utility functions
 * Copyright (C) 1996-2020 D-Type Solutions
 * Web Site: http://www.d-type.com
 * E-mail: info@d-type.com
 */

#ifndef DTYPE_XSYSTEM_H
#define DTYPE_XSYSTEM_H


#if (defined(linux) || defined(__linux) || defined(__linux__) || defined(__CYGWIN__) || defined(__APPLE__))
#define DEF_XSYSTEM_POSIX
#endif

#ifdef DEF_XSYSTEM_POSIX
#include <limits.h> /* PATH_MAX */
#include <unistd.h> /* chdir */
#include <sys/resource.h> /* getrlimit, setrlimit */
#endif


inline int xsys_set_work_dir(int main_argc, char* main_argv[])
{
	if (main_argc < 1 || main_argv == DV_NULL || main_argv[0] == DV_NULL) return 0;

#ifdef DEF_XSYSTEM_POSIX

	char resolved_path[PATH_MAX + 1]; realpath(main_argv[0], resolved_path);
	int i, j = 0;

	for (i = 0; i < PATH_MAX; i++)
	{
		if (resolved_path[i] == 0) break; else if (resolved_path[i] == '/') j = i;
	}

	if (j < 1) return 0;

	resolved_path[j] = 0; chdir(resolved_path);

#else

	(void)main_argc;
	(void)main_argv;

#endif

	return 1;
}


inline int xsys_set_max_files(int max_files)
{
#ifdef DEF_XSYSTEM_POSIX
	rlimit rlp_set; rlp_set.rlim_cur = rlp_set.rlim_max = max_files; setrlimit(RLIMIT_NOFILE, &rlp_set);
#else
	(void)max_files;
#endif

	return 1;
}


#endif /* DTYPE_XSYSTEM_H */
