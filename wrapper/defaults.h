

#ifdef CHK_REAL_UID
#if !((CHK_REAL_UID >= 0) && (CHK_REAL_UID <= 65535))
#error "CHK_REAL_UID must be an integer between 0 and 65535"
#endif
#endif

#ifdef CHK_REAL_GID
#if !((CHK_REAL_GID >= 0) && (CHK_REAL_GID <= 65535))
#error "CHK_REAL_GID must be an integer between 0 and 65535"
#endif
#endif

#ifdef CHK_ARGC_MIN
#if !((CHK_ARGC_MIN >= 0) && (CHK_ARGC_MIN <= 255))
#error "CHK_ARGC_MIN must be an integer between 0 and 255"
#endif
#endif

#ifdef CHK_ARGC_MAX
#if !((CHK_ARGC_MAX >= 0) && (CHK_ARGC_MAX <= 255))
#error "CHK_ARGC_MAX must be an integer between 0 and 255"
#endif
#endif

#if defined(CHK_ARGS_REGEXP) && !defined(CHK_ARGS_REGEXP_FLAGS)
#define CHK_ARGS_REGEXP_FLAGS 0
#endif


#ifdef DO_SETGID
#if !((DO_SETGID >= 0) && (DO_SETGID <= 65535))
#error "DO_SETGID must be an integer between 0 and 65535"
#endif
#endif

#ifdef DO_SETUID
#if !((DO_SETUID >= 0) && (DO_SETUID <= 65535))
#error "DO_SETUID must be an integer between 0 and 65535"
#endif
#endif


#if !defined(EXEC_PATH)
#error "EXEC_PATH must be set"
#endif

#if !defined(EXEC_ARGS)
#define EXEC_ARGS
#endif
