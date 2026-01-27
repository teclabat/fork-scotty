/*
 * tnmNtp.c --
 *
 *	Extend a tcl command interpreter with a command to query NTP
 *	server for timestat. This implementation is supposed to be
 *	thread-safe.
 *
 * Copyright (c) 1994-1996 Technical University of Braunschweig.
 * Copyright (c) 1996-1997 University of Twente.
 * Copyright (c) 1997-1999 Technical University of Braunschweig.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "tnmInt.h"
#include "tnmPort.h"

/*
 * ToDo:	* check about `more' flag.
 *		* make better error return strings.
 */

/*
 * NTP Control Mode packet structure (mode 6) - for server monitoring
 */
struct ntp_control {
    unsigned char mode;			/* version and mode */
    unsigned char op;			/* opcode */
    unsigned short sequence;		/* sequence # */
    unsigned short status;		/* status */
    unsigned short associd;		/* association id */
    unsigned short offset;		/* data offset */
    unsigned short len;			/* data len */
    unsigned char data[(480 + 20)];	/* data + auth */
};

/*
 * NTP Client Mode packet structure (mode 3) - for time synchronization
 * This is the standard 48-byte NTP packet (RFC 5905)
 */
struct ntp_time_pkt {
    unsigned char li_vn_mode;		/* LI(2) | VN(3) | Mode(3) */
    unsigned char stratum;		/* stratum level */
    unsigned char poll;			/* poll interval */
    signed char precision;		/* precision */
    unsigned int root_delay;		/* root delay */
    unsigned int root_dispersion;	/* root dispersion */
    unsigned int ref_id;		/* reference ID */
    unsigned int ref_ts_sec;		/* reference timestamp seconds */
    unsigned int ref_ts_frac;		/* reference timestamp fraction */
    unsigned int orig_ts_sec;		/* origin timestamp seconds */
    unsigned int orig_ts_frac;		/* origin timestamp fraction */
    unsigned int recv_ts_sec;		/* receive timestamp seconds */
    unsigned int recv_ts_frac;		/* receive timestamp fraction */
    unsigned int xmit_ts_sec;		/* transmit timestamp seconds */
    unsigned int xmit_ts_frac;		/* transmit timestamp fraction */
};

/* NTP epoch offset: seconds from 1900-01-01 to 1970-01-01 */
#define NTP_EPOCH_OFFSET 2208988800UL

/*
 * The options for the ntp command.
 */

enum options { optTimeout, optRetries };

static TnmTable ntpOptionTable[] = {
    { optTimeout,	"-timeout" },
    { optRetries,	"-retries" },
    { 0, NULL }
};

/*
 * The subcommands for the ntp command.
 */

enum subcmds { cmdTime, cmdStatus };

static TnmTable ntpSubCmdTable[] = {
    { cmdTime,		"time" },
    { cmdStatus,	"status" },
    { 0, NULL }
};

/*
 * The socket used to send and receive NTP datagrams.
 */

static int sock = -1;

/*
 * Every Tcl interpreter has an associated NtpControl record. It
 * keeps track of the default settings for this interpreter.
 */

static char tnmNtpControl[] = "tnmNtpControl";

typedef struct NtpControl {
    int retries;		/* Default number of retries. */
    int timeout;		/* Default timeout in seconds. */
} NtpControl;

/*
 * Mutex used to serialize access to static variables in this module.
 */

TCL_DECLARE_MUTEX(ntpMutex)

/*
 * Forward declarations for procedures defined later in this file:
 */

static void
AssocDeleteProc	(ClientData clientData, Tcl_Interp *interp);

static int
NtpSocket	(Tcl_Interp *interp);

static int
NtpReady	(int s, int timeout);

static void
NtpMakePkt	(struct ntp_control *pkt, int op,
			     unsigned short assoc, unsigned short seq);
static int
NtpFetch	(Tcl_Interp *interp, struct sockaddr_in *daddr, 
			     int op, int retries, int timeo,
			     char *buf, unsigned short assoc);
static int
NtpSplitToDict	(Tcl_Interp *interp, Tcl_Obj *dictPtr,
			     char *pfix, char *buf);
static int 
NtpGetPeer	(char *data, int *assoc);

/*
 *----------------------------------------------------------------------
 *
 * AssocDeleteProc --
 *
 *	This procedure is called when a Tcl interpreter gets destroyed
 *	so that we can clean up the data associated with this interpreter.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static void
AssocDeleteProc(ClientData clientData, Tcl_Interp *interp)
{
    NtpControl *control = (NtpControl *) clientData;

    if (control) {
	ckfree((char *) control);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * NtpSocket --
 *
 *	This procedure opens a socket that is used to send NTP requests.
 *	An error message is left in interp if we can't open the socket.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	A socket is opened.
 *
 *----------------------------------------------------------------------
 */

static int
NtpSocket(Tcl_Interp *interp)
{
    struct sockaddr_in maddr;
    int code;
    
    if (sock != -1) {
	TnmSocketClose(sock);
    }

    sock = TnmSocket(AF_INET, SOCK_DGRAM, 0);
    if (sock == TNM_SOCKET_ERROR) {
	Tcl_AppendResult(interp, "could not create socket: ", 
			 Tcl_PosixError(interp), (char *) NULL);
        return TCL_ERROR;
    }

    maddr.sin_family = AF_INET;
    maddr.sin_addr.s_addr = htonl(INADDR_ANY);
    maddr.sin_port = htons(0);

    code = TnmSocketBind(sock, (struct sockaddr *) &maddr, sizeof(maddr));
    if (code == TNM_SOCKET_ERROR) {
	Tcl_AppendResult(interp, "can not bind socket: ",
			 Tcl_PosixError(interp), (char *) NULL);
	TnmSocketClose(sock);
	sock = -1;
        return TCL_ERROR;
    }

    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * NtpReady --
 *
 *	This procedure used to wait for an answer from an NTP server.
 *
 * Results:
 *	Return 1 if we got an answer, 0 otherwise.
 *
 * Side effects:
 *	Time passes.
 *
 *----------------------------------------------------------------------
 */

static int
NtpReady(s, timeout)
    int s, timeout;
{
    fd_set rfd;
    struct timeval tv;
    int rc;

    FD_ZERO(&rfd);
    FD_SET(s, &rfd);
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;

    do {
	rc = select(s + 1, &rfd, (fd_set *) 0, (fd_set *) 0, &tv);
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
	if (rc == SOCKET_ERROR) {
	    int wsaErr = WSAGetLastError();
	    if (wsaErr == WSAEINTR) {
		continue;
	    }
	    /* On Windows, select() errors on UDP are not fatal - just timeout */
	    return 0;
	}
#else
	if (rc == -1 && errno == EINTR) {
	    continue;
	}
	if (rc == -1) {
	    return 0;
	}
#endif
    } while (rc < 0);

    return rc > 0;
}

/*
 *----------------------------------------------------------------------
 *
 * NtpMakePkt --
 *
 *	This procedure creates an NTP packet.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static void
NtpMakePkt(struct ntp_control *pkt, int op, unsigned short assoc, unsigned short seq)
{
    pkt->mode = 0x18 | 6;			/* version 3 | MODE_CONTROL */
    pkt->op = op;				/* CTL_OP_... */
    pkt->sequence = htons(seq);
    pkt->status = 0;
    pkt->associd = htons(assoc);
    pkt->offset = htons(0);

    if (! assoc) {
	snprintf((char *) pkt->data, sizeof(pkt->data),
	      "precision,peer,system,stratum,rootdelay,rootdispersion,refid");
    } else  {
	snprintf((char *) pkt->data, sizeof(pkt->data),
	      "srcadr,stratum,precision,reach,valid,delay,offset,dispersion");
    }
    pkt->len = htons((unsigned short) (strlen((char *) pkt->data)));
}

/*
 *----------------------------------------------------------------------
 *
 * NtpFetch --
 *
 *	This procedure fetches the result of an op from an NTP server
 *	and appends the result to the buffer buf. An error message is
 *	left in the Tcl interpreter pointed to by interp.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	Ntp packets are send and received.
 *
 *----------------------------------------------------------------------
 */

static int
NtpFetch(Tcl_Interp *interp, struct sockaddr_in *daddr, int op, int retries, int timeo, char *buf, unsigned short assoc)
{
    struct ntp_control qpkt, pkt;
    struct sockaddr_in saddr;
    int i, rc;
    socklen_t slen = sizeof(saddr);
    int timeout = (timeo * 1000) / (retries + 1);

    static unsigned short seq = 1;			/* sequence number */

    /* 
     * increment to a new sequence number: 
     */

    seq++;
    
    for (i = 0; i < retries + 1; i++) {
	NtpMakePkt(&qpkt, op, assoc, seq);		/* CTL_OP_READVAR */
	memset((char *) &pkt, 0, sizeof(pkt));
	
	rc = TnmSocketSendTo(sock, (unsigned char *)&qpkt, sizeof(qpkt), 0, 
			     (struct sockaddr *) daddr, sizeof(*daddr));
	if (rc == TNM_SOCKET_ERROR) {
	    Tcl_AppendResult(interp, "udp sendto failed: ",
			     Tcl_PosixError(interp), (char *) NULL);
	    return TCL_ERROR;
	}
	
	while (NtpReady(sock, timeout)) {
	    memset((char *) &pkt, 0, sizeof(pkt));
	    rc = TnmSocketRecvFrom(sock, (unsigned char *) &pkt, sizeof(pkt), 0, 
				   (struct sockaddr *) &saddr, &slen);
	    if (rc == TNM_SOCKET_ERROR) {
		Tcl_AppendResult(interp, "recvfrom failed: ",
				 Tcl_PosixError(interp), (char *) NULL);
		return TCL_ERROR;
	    }

	    /*
	     * Ignore short packets < (ntp_control + 1 data byte)
	     */
	    
	    if (rc < 12 + 1) {
		continue;
	    }
	    
	    if ((pkt.op & 0x80) 
		&& saddr.sin_addr.s_addr == daddr->sin_addr.s_addr
		&& saddr.sin_port == daddr->sin_port
		&& pkt.sequence == qpkt.sequence)
	    {
		strcat(buf, (char *) pkt.data);
		return TCL_OK;
	    }
	}
    }
    
    Tcl_SetResult(interp, "no ntp response", TCL_STATIC);
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * NtpSplitToDict --
 *
 *	This procedure splits the result of an NTP query into pieces
 *	and adds them to a Tcl dictionary object with a prefix.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	Modifies the dictionary object.
 *
 *----------------------------------------------------------------------
 */

static int
NtpSplitToDict(Tcl_Interp *interp, Tcl_Obj *dictPtr, char *pfix, char *buf)
{
    char *d, *s, *g;
    char key[256];

    for (s = buf, d = buf; *s; s++) {
	if (*s == ',') {
	    *s = '\0';
	    for (g = d; *g && (*g != '='); g++) ;
	    if (*g) {
		*g++ = '\0';
		snprintf(key, sizeof(key), "%s.%s", pfix, d);
		Tcl_DictObjPut(interp, dictPtr,
		               Tcl_NewStringObj(key, -1),
		               Tcl_NewStringObj(g, -1));
	    }
	    for (d = s+1; *d && isspace(*d); d++) ;
	}
    }

    if (d != s) {
	if (isspace(*--s)) *s = '\0';
	if (isspace(*--s)) *s = '\0';
	for (g = d; *g && (*g != '='); g++) ;
	if (*g) {
	    *g++ = '\0';
	    snprintf(key, sizeof(key), "%s.%s", pfix, d);
	    Tcl_DictObjPut(interp, dictPtr,
	                   Tcl_NewStringObj(key, -1),
	                   Tcl_NewStringObj(g, -1));
	}
    }

    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * NtpGetPeer --
 *
 *	This procedure scans data for a peer=... entry.
 *
 * Results:
 *	Returns 1 if the peer entry is found, 0 otherwise. The
 *	peer is returned in assoc.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static int 
NtpGetPeer(char *data, int *assoc)
{
    int i;

    for (i = 0; i < (int) strlen(data); i++) {
        if (1 == sscanf(data + i, "peer=%d,", assoc)) {
	    return 1;
	}
    }

    return 0;
}

/*
 *----------------------------------------------------------------------
 *
 * NtpTimeQuery --
 *
 *	This procedure performs a simple NTP time query using mode 3
 *	(client mode). This works with public NTP pool servers that
 *	don't respond to mode 6 control queries.
 *
 * Results:
 *	A standard Tcl result. On success, sets array variables with
 *	time information.
 *
 * Side effects:
 *	NTP packets are sent and received.
 *
 *----------------------------------------------------------------------
 */

static int
NtpTimeQuery(Tcl_Interp *interp, struct sockaddr_in *daddr,
             int retries, int timeo)
{
    struct ntp_time_pkt qpkt, pkt;
    struct sockaddr_in saddr;
    int i, rc;
    socklen_t slen = sizeof(saddr);
    int timeout = (timeo * 1000) / (retries + 1);
    char buf[64];
    double offset, delay;
    unsigned int t1_sec, t2_sec, t3_sec, t4_sec;
    time_t server_time;
    Tcl_Obj *dictPtr;

    for (i = 0; i < retries + 1; i++) {
        /* Build NTP client request packet */
        memset(&qpkt, 0, sizeof(qpkt));
        qpkt.li_vn_mode = 0x1B;  /* LI=0, VN=3, Mode=3 (client) */

        /* Set transmit timestamp (T1) to current time */
        t1_sec = (unsigned int)(time(NULL) + NTP_EPOCH_OFFSET);
        qpkt.xmit_ts_sec = htonl(t1_sec);
        qpkt.xmit_ts_frac = 0;

        rc = TnmSocketSendTo(sock, (unsigned char *)&qpkt, sizeof(qpkt), 0,
                             (struct sockaddr *) daddr, sizeof(*daddr));
        if (rc == TNM_SOCKET_ERROR) {
            Tcl_AppendResult(interp, "udp sendto failed: ",
                             Tcl_PosixError(interp), (char *) NULL);
            return TCL_ERROR;
        }

        while (NtpReady(sock, timeout)) {
            memset(&pkt, 0, sizeof(pkt));
            rc = TnmSocketRecvFrom(sock, (unsigned char *) &pkt, sizeof(pkt), 0,
                                   (struct sockaddr *) &saddr, &slen);
            if (rc == TNM_SOCKET_ERROR) {
                Tcl_AppendResult(interp, "recvfrom failed: ",
                                 Tcl_PosixError(interp), (char *) NULL);
                return TCL_ERROR;
            }

            /* Check for valid NTP response (mode 4 = server) */
            if (rc < 48) {
                continue;  /* Too short */
            }
            if ((pkt.li_vn_mode & 0x07) != 4) {
                continue;  /* Not server mode */
            }
            if (saddr.sin_addr.s_addr != daddr->sin_addr.s_addr) {
                continue;  /* Wrong source */
            }

            /* Record receive time (T4) */
            t4_sec = (unsigned int)(time(NULL) + NTP_EPOCH_OFFSET);

            /* Extract timestamps from response */
            t2_sec = ntohl(pkt.recv_ts_sec);
            t3_sec = ntohl(pkt.xmit_ts_sec);

            /* Calculate offset and delay (simplified, ignoring fractions) */
            /* offset = ((T2 - T1) + (T3 - T4)) / 2 */
            /* delay = (T4 - T1) - (T3 - T2) */
            offset = (((double)t2_sec - t1_sec) + ((double)t3_sec - t4_sec)) / 2.0;
            delay = ((double)t4_sec - t1_sec) - ((double)t3_sec - t2_sec);

            /* Server time is T3 (transmit timestamp) */
            server_time = t3_sec - NTP_EPOCH_OFFSET;

            /* Build result dictionary */
            dictPtr = Tcl_NewDictObj();

            snprintf(buf, sizeof(buf), "%u", (unsigned int)server_time);
            Tcl_DictObjPut(interp, dictPtr,
                           Tcl_NewStringObj("time", -1),
                           Tcl_NewStringObj(buf, -1));

            snprintf(buf, sizeof(buf), "%.6f", offset);
            Tcl_DictObjPut(interp, dictPtr,
                           Tcl_NewStringObj("offset", -1),
                           Tcl_NewStringObj(buf, -1));

            snprintf(buf, sizeof(buf), "%.6f", delay);
            Tcl_DictObjPut(interp, dictPtr,
                           Tcl_NewStringObj("delay", -1),
                           Tcl_NewStringObj(buf, -1));

            Tcl_DictObjPut(interp, dictPtr,
                           Tcl_NewStringObj("stratum", -1),
                           Tcl_NewIntObj(pkt.stratum));

            Tcl_DictObjPut(interp, dictPtr,
                           Tcl_NewStringObj("precision", -1),
                           Tcl_NewIntObj(pkt.precision));

            /* Reference ID - format depends on stratum */
            if (pkt.stratum <= 1) {
                /* Stratum 0-1: ASCII reference clock identifier */
                snprintf(buf, sizeof(buf), "%.4s", (char *)&pkt.ref_id);
            } else {
                /* Stratum 2+: IP address of reference server */
                struct in_addr refaddr;
                refaddr.s_addr = pkt.ref_id;
                snprintf(buf, sizeof(buf), "%s", inet_ntoa(refaddr));
            }
            Tcl_DictObjPut(interp, dictPtr,
                           Tcl_NewStringObj("refid", -1),
                           Tcl_NewStringObj(buf, -1));

            Tcl_SetObjResult(interp, dictPtr);
            return TCL_OK;
        }
    }

    Tcl_SetResult(interp, "no ntp response", TCL_STATIC);
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * Tnm_NtpObjCmd --
 *
 *	This procedure is invoked to process the "ntp" command.
 *	See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

int
Tnm_NtpObjCmd(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
    struct sockaddr_in daddr;
    int x, code, assoc;
    char data1 [1024], data2 [1024];

    int actRetries = -1;	/* actually used retries */
    int actTimeout = -1;	/* actually used timeout */

    NtpControl *control = (NtpControl *) 
	Tcl_GetAssocData(interp, tnmNtpControl, NULL);

    if (! control) {
	control = (NtpControl *) ckalloc(sizeof(NtpControl));
	control->retries = 2;
	control->timeout = 2;
	Tcl_SetAssocData(interp, tnmNtpControl, AssocDeleteProc, 
			 (ClientData) control);
    }

    if (objc < 2) {
    wrongArgs:
	Tcl_WrongNumArgs(interp, 1, objv,
			 "?-timeout t? ?-retries r? (time|status) host");
	return TCL_ERROR;
    }

    /* 
     * Parse the options:
     */

    for (x = 1; x < objc; x++) {
	code = TnmGetTableKeyFromObj(interp, ntpOptionTable,
				     objv[x], "option");
	if (code == -1) {
	    char *option = Tcl_GetStringFromObj(objv[x], NULL);
	    if (*option == '-') {
		return TCL_ERROR;
	    } else {
		Tcl_ResetResult(interp);
		break;
	    }
	}
	switch ((enum options) code) {
	case optTimeout:
	    if (x == objc-1) {
		Tcl_SetIntObj(Tcl_GetObjResult(interp), control->timeout);
		return TCL_OK;
	    }
	    code = TnmGetPositiveFromObj(interp, objv[++x], &actTimeout);
	    if (code != TCL_OK) {
	        return TCL_ERROR;
	    }
	    break;
	case optRetries:
	    if (x == objc-1) {
		Tcl_SetIntObj(Tcl_GetObjResult(interp), control->retries);
		return TCL_OK;
	    }
	    code = TnmGetUnsignedFromObj(interp, objv[++x], &actRetries);
	    if (code != TCL_OK) {
	        return TCL_ERROR;
	    }
	    break;
	}
    }

    /*
     * No arguments left? Set the default values and return.
     */

    if (x == objc) {
	if (actRetries >= 0) {
	    control->retries = actRetries;
	}
	if (actTimeout > 0) {
	    control->timeout = actTimeout;
	}
        return TCL_OK;
    }

    /*
     * Parse subcommand (time or status) - required.
     */

    int subcmd;
    if (x >= objc) {
        goto wrongArgs;
    }
    subcmd = TnmGetTableKeyFromObj(interp, ntpSubCmdTable, objv[x], "subcommand");
    if (subcmd < 0) {
        return TCL_ERROR;
    }
    x++;

    /*
     * Now we should have one argument left: host.
     */

    if (x != objc-1) {
	goto wrongArgs;
    }

    actRetries = actRetries < 0 ? control->retries : actRetries;
    actTimeout = actTimeout < 0 ? control->timeout : actTimeout;

    Tcl_MutexLock(&ntpMutex);

    if (sock < 0) {
	if (NtpSocket(interp) != TCL_OK) {
	    Tcl_MutexUnlock(&ntpMutex);
	    return TCL_ERROR;
	}
    }

    if (TnmSetIPAddress(interp, Tcl_GetStringFromObj(objv[x], NULL),
			&daddr) != TCL_OK) {
	Tcl_MutexUnlock(&ntpMutex);
        return TCL_ERROR;
    }
    daddr.sin_port = htons(123);

    /*
     * Handle "time" subcommand - uses NTP client mode (mode 3)
     */

    if (subcmd == cmdTime) {
        code = NtpTimeQuery(interp, &daddr, actRetries, actTimeout);
        Tcl_MutexUnlock(&ntpMutex);
        return code;
    }

    /*
     * Handle "status" subcommand - uses NTP control mode (mode 6)
     * CTL_OP_READVAR
     */

    {
        Tcl_Obj *dictPtr = Tcl_NewDictObj();

        data1[0] = data2[0] = 0;
        code = NtpFetch(interp, &daddr, 2, actRetries, actTimeout, data1, 0);
        if (code != TCL_OK) {
            Tcl_MutexUnlock(&ntpMutex);
            return TCL_ERROR;
        }

        /*
         * Try to get additional peer info:
         */

        if (NtpGetPeer(data1, &assoc)) {
            NtpFetch(interp, &daddr, 2, actRetries, actTimeout, data2,
                     (unsigned short) assoc);
            /* Ignore errors on peer fetch - sys data is enough */
        }

        /*
         * Split the response into a dictionary.
         */

        NtpSplitToDict(interp, dictPtr, "sys", data1);
        if (data2[0]) {
            NtpSplitToDict(interp, dictPtr, "peer", data2);
        }

        Tcl_SetObjResult(interp, dictPtr);
        Tcl_MutexUnlock(&ntpMutex);
        return TCL_OK;
    }
}

