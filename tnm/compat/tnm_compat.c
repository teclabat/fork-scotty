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

/* DNS Resolver stub */
typedef struct __res_state {
    int retrans;
    int retry;
    unsigned long options;
    int nscount;
    struct sockaddr_in nsaddr_list[3];
} *res_state;

struct __res_state _res;

int res_init(void) {
    /* Stub: DNS resolver not available on MinGW */
    errno = ENOSYS;
    return -1;
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
