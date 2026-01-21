/*
 * tnmWinPort.h --
 *
 *	This header file handles porting issues that occur because of
 *	differences between Windows and Unix. It should be the only
 *	file that contains #ifdefs to handle different flavors of OS.
 *
 * Copyright (c) 1993-1996 Technical University of Braunschweig.
 * Copyright (c) 1996-1997 University of Twente. 
 * Copyright (c) 1997-2001 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#ifndef _TNMWINPORT
#define _TNMWINPORT

/*
 *----------------------------------------------------------------
 * Configuration parameters for the Win32 plattform.
 *----------------------------------------------------------------
 */

/* #define WORDS_BIGENDIAN */  /* x86_64 is little-endian, not big-endian */
#define LITTLE_ENDIAN 1234
#define BIG_ENDIAN 4321
#define BYTE_ORDER LITTLE_ENDIAN
#define HAVE_RPCENT
#define SIZEOF_INT 4
#define SIZEOF_UNSIGNED_INT 4
#define SIZEOF_LONG 4
#define SIZEOF_UNSIGNED_LONG 4

/*
 * The default directory name where we will find the Tnm library
 * files. This is normally overwritten in the Makefile. Note that
 * the Windows version will use the information in the registry
 * is available. This is only a fall through definition here.
 */

/* #define TKI_VERSION "1.5.1" */  /* Use version from tnm.h instead (1.6.0) */

#ifndef TNMLIB
#define TNMLIB "c:/tcl/lib/tnm3.1.3"
#endif

#ifndef TKINEDLIB
#define TKINEDLIB "c:/tcl/lib/tkined1.6.0"
#endif

/*
 *----------------------------------------------------------------
 * Windows related includes and defines.
 *----------------------------------------------------------------
 */

#include <malloc.h>
#include <stdio.h>

#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

#include <time.h>

#include <process.h>
#include <io.h>
#include <windows.h>
#include <winsock.h>

/*
 * MinGW64 compatibility: Define struct __stat64 as alias for struct _stat64
 * Tcl uses __stat64 but MinGW64 only provides _stat64
 */
#if defined(__MINGW64__)
struct __stat64 {
    _dev_t st_dev;
    _ino_t st_ino;
    unsigned short st_mode;
    short st_nlink;
    short st_uid;
    short st_gid;
    _dev_t st_rdev;
    __int64 st_size;
    __time64_t st_atime;
    __time64_t st_mtime;
    __time64_t st_ctime;
};
#endif

/*
 * Winsock1 compatibility definitions for modern code
 */
#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif

/* Provide inet_ntop compatibility via inet_ntoa */
#ifndef inet_ntop
static inline const char* inet_ntop_compat(int af, const void *src, char *dst, int size) {
    if (af == AF_INET) {
        struct in_addr *addr = (struct in_addr *)src;
        char *result = inet_ntoa(*addr);
        if (result && size > 0) {
            strncpy(dst, result, size - 1);
            dst[size - 1] = '\0';
            return dst;
        }
    }
    return NULL;
}
#define inet_ntop inet_ntop_compat
#endif

#ifndef IN_CLASSD
#define	IN_CLASSD(i)		(((long)(i) & 0xf0000000) == 0xe0000000)
#endif

/*
 *----------------------------------------------------------------
 * Windows does not define the access modes - we add them here.
 *----------------------------------------------------------------
 */

#ifndef F_OK
#define F_OK 00
#endif
#ifndef X_OK
#define X_OK 01
#endif
#ifndef W_OK
#define W_OK 02
#endif
#ifndef R_OK
#define R_OK 04
#endif

#if 0
/*
 *----------------------------------------------------------------
 * Windows does not define all the usual macros to query file
 * type bits. We define them here if they are not defined yet.
 *----------------------------------------------------------------
 */

#ifndef S_ISREG
#   ifdef S_IFREG
#       define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#   else
#       define S_ISREG(m) 0
#   endif
# endif
#ifndef S_ISDIR
#   ifdef S_IFDIR
#       define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#   else
#       define S_ISDIR(m) 0
#   endif
# endif
#ifndef S_ISCHR
#   ifdef S_IFCHR
#       define S_ISCHR(m) (((m) & S_IFMT) == S_IFCHR)
#   else
#       define S_ISCHR(m) 0
#   endif
# endif
#ifndef S_ISBLK
#   ifdef S_IFBLK
#       define S_ISBLK(m) (((m) & S_IFMT) == S_IFBLK)
#   else
#       define S_ISBLK(m) 0
#   endif
# endif
#ifndef S_ISFIFO
#   ifdef S_IFIFO
#       define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#   else
#       define S_ISFIFO(m) 0
#   endif
# endif
#endif

/*
 *----------------------------------------------------------------
 * This is part of the Resolv lib, which we do not have header
 * files for. So we define the enty points here.
 *----------------------------------------------------------------
 */

extern struct netent *getnetbyaddr(long net, int type);
extern struct netent *getnetbyname(const char *name);

/*
 *----------------------------------------------------------------
 * A define that we use to create Tcl_File handles on sockets.
 *----------------------------------------------------------------
 */

#define TNM_SOCKET_FD	TCL_WIN_SOCKET
/* typedef unsigned int socklen_t; */  /* Use size_t from tnmInt.h instead */

/*
 *----------------------------------------------------------------
 * Windows has a stricmp function but not strcasecmp:
 *----------------------------------------------------------------
 */

#define strcasecmp     stricmp


#endif /* _TNMWINPORT */
