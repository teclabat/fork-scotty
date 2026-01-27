/*
 * tnm_compat.c --
 *
 *	Compatibility shims for MinGW64 compilation.
 *	Provides stub implementations for DNS resolver and network database
 *	functions that are not available in MinGW.
 *
 * Copyright (c) 2026 teclab.
 */

#include <windows.h>
#include <winsock.h>
#include <errno.h>
#include <string.h>

/* DNS Resolver constants (must match tnmDns.c expectations) */
#ifndef MAXDNSRCH
#define MAXDNSRCH 6     /* Max search domains */
#endif
#ifndef MAXNS
#define MAXNS 3         /* Max name servers */
#endif

/* DNS Resolver stub - must include all fields accessed by tnmDns.c */
typedef struct __res_state {
    int retrans;                          /* retransmission timeout */
    int retry;                            /* number of retries */
    unsigned long options;                /* option flags */
    int nscount;                          /* number of name servers */
    struct sockaddr_in nsaddr_list[MAXNS];/* name server addresses */
    char *dnsrch[MAXDNSRCH + 1];          /* search list (NULL-terminated) */
} *res_state;

/* Zero-initialize to prevent garbage values and NULL-terminate dnsrch */
struct __res_state _res = {0};

int res_init(void) {
    /* Initialize with safe defaults for Windows */
    memset(&_res, 0, sizeof(_res));
    _res.retrans = 2;      /* 2 second timeout */
    _res.retry = 2;        /* 2 retries */
    _res.nscount = 0;      /* No servers configured */
    /* dnsrch[] is NULL from memset - will terminate search loop immediately */
    return 0;  /* Return success to allow graceful error handling */
}

int res_mkquery(int op, const char *dname, int class, int type,
                const unsigned char *data, int datalen,
                const unsigned char *newrr, unsigned char *buf, int buflen) {
    /* Stub: DNS resolver not available on MinGW */
    errno = ENOSYS;
    return -1;
}

int res_send(const unsigned char *msg, int msglen,
             unsigned char *answer, int anslen) {
    /* Stub: DNS resolver not available on MinGW */
    errno = ENOSYS;
    return -1;
}

int dn_expand(const unsigned char *msg, const unsigned char *eomorig,
              const unsigned char *comp_dn, char *exp_dn, int length) {
    /* Stub: DNS resolver not available on MinGW */
    errno = ENOSYS;
    return -1;
}

int __dn_skipname(const unsigned char *comp_dn, const unsigned char *eom) {
    /* Stub: DNS resolver not available on MinGW */
    errno = ENOSYS;
    return -1;
}

/* Network database stubs */
struct netent *getnetbyname(const char *name) {
    /* Stub: Network database not available on MinGW */
    return NULL;
}

struct netent *getnetbyaddr(long net, int type) {
    /* Stub: Network database not available on MinGW */
    return NULL;
}
