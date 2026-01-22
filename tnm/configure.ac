#!/bin/bash -norc
dnl	This file is an input file used by the GNU "autoconf" program to
dnl	generate the file "configure", which is run during Tcl installation
dnl	to configure the system for the local environment.

#-----------------------------------------------------------------------
# Package information
#-----------------------------------------------------------------------

AC_INIT([tnm],[3.1.3])dnl

#-----------------------------------------------------------------------
# TEA initialization
#-----------------------------------------------------------------------

TEA_INIT()

#-----------------------------------------------------------------------
# C++ support (C++11)
#-----------------------------------------------------------------------

TEA_INIT_CXX([11])

#-----------------------------------------------------------------------
# Set auxiliary directory
#-----------------------------------------------------------------------

AC_CONFIG_AUX_DIR(tclconfig)
AC_PROG_INSTALL

#-----------------------------------------------------------------------
# Scotty specific definitions and checks
#-----------------------------------------------------------------------

TNM_VERSION=3.1.3
TKI_VERSION=1.6.0

# disabled config.h generation, since this inhibits stub compilation...
# but we do kinda need it though?
AC_CHECK_HEADERS([config.h])
AS_IF([test "x${ac_cv_header_config_h}" != "xyes"],[
  AC_CONFIG_HEADERS([config.h])
  AH_TOP([
#ifndef __TNM_CONFIG_H__
# define __TNM_CONFIG_H__ 1
])
  AH_BOTTOM([
#endif /* !__TNM_CONFIG_H__ */
])
],[
  AC_MSG_NOTICE([using discovered <config.h> instead of generating one.])
])dnl

#--------------------------------------------------------------------
#       Check for various system specific things.
#--------------------------------------------------------------------

AC_TYPE_INT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([unsigned int])
AC_CHECK_SIZEOF([long])
AC_CHECK_SIZEOF([unsigned long])
AC_CHECK_SIZEOF([unsigned long long])dnl

#----------------------------------------------------------------------------
#	rpcgen check disabled - Sun RPC support excluded from build
#	(Original check for rpcgen and precompiled RPC stubs commented out)
#----------------------------------------------------------------------------

# AC_CHECK_PROG([RPCGEN],[rpcgen],[rpcgen],[echo])
# if test "x${RPCGEN}" = "xecho"; then
#     AC_MSG_WARN([Failed to find rpcgen. Will use precompiled files.])
#     cp ${srcdir}/../compat/ether* .
#     cp ${srcdir}/../compat/mount* .
#     cp ${srcdir}/../compat/pcnfsd* .
#     cp ${srcdir}/../compat/rstat* .
# fi

#----------------------------------------------------------------------------
#	Check for the existence of various libraries.
#       sin, ieee, inet, net/errno.h, socket, nsl 
#----------------------------------------------------------------------------

TEA_TCL_LINK_LIBS

#----------------------------------------------------------------------------
#	Check for the existence of various libraries.
#       zlib, rpc, resolv
#----------------------------------------------------------------------------

AX_CHECK_ZLIB([LIBS="${LIBS} -lz"],[no])dnl

# See Alex Chernyakhovsky's article:
# http://blog.achernya.com/2013/03/os-x-has-silly-libsystem.html
# We have taken his solution from here:
# https://github.com/achernya/hesiod/blob/master/configure.ac
AC_CHECK_LIB([resolv],[res_mkquery],[],[
    AC_MSG_CHECKING([if res_mkquery is provided by libresolv with mangled symbols])
    save_LIBS="${LIBS}"
    LIBS="-lresolv ${LIBS}"
    AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <resolv.h>]], [[res_mkquery(0,NULL,0,0,NULL,0,NULL,NULL,0);]])],[
                    AC_DEFINE([HAVE_LIBRESOLV],[1],[Define if your libresolv provides res_mkquery.])
		    AC_MSG_RESULT([yes])
		    NM_LIBS="${NM_LIBS} -lresolv"
		],[
		    LIBS="${save_LIBS}"
		    AC_MSG_RESULT([no])])])dnl

#----------------------------------------------------------------------------
#	Check for new socklen_t type which is not yet universally available.
#	Check for sin_len field in struct sockaddr (BSD-ish).
#----------------------------------------------------------------------------

AC_CHECK_HEADERS([sys/socket.h])dnl

AC_CHECK_TYPE([socklen_t],
              [AC_DEFINE([HAVE_SOCKLEN_T],[1],[socklen_t available])],[],
[
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif /* HAVE_SYS_TYPES_H */
#ifdef HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif /* HAVE_SYS_SOCKET_H */
])dnl

AC_CACHE_CHECK([for 'sa_len' in 'struct sockaddr'],[ac_cv_struct_sa_len], 
[AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <sys/types.h> 
#include <sys/socket.h>]], [[struct sockaddr sa; sa.sa_len = 0; ]])],[ac_cv_struct_sa_len=yes],[ac_cv_struct_sa_len=no])]) 
if test "x${ac_cv_struct_sa_len}" = "xyes"; then 
  AC_DEFINE([HAVE_SA_LEN],[1],[sa_len present]) 
fi 

#----------------------------------------------------------------------------
#	Checks for various IPv6 related functions (and other functions).
#----------------------------------------------------------------------------

AC_CHECK_FUNCS([alarm bzero dup2 floor getaddrinfo gethostbyaddr gethostbyname \
                getnameinfo inet_ntoa inet_ntop inet_pton isascii memset select \
                socket strcasecmp strchr strrchr strstr strtol strtoul])dnl

#----------------------------------------------------------------------------
#	Checks for various include files missing on some machines.
#----------------------------------------------------------------------------

AC_HEADER_RESOLV
AC_CHECK_HEADERS([arpa/nameser.h fcntl.h malloc.h malloc/malloc.h netdb.h \
                  netinet/in.h sys/ioctl.h sys/select.h sys/time.h syslog.h \
                  zlib.h])dnl

if test "x${ac_cv_header_arpa_nameser_h}" != "xyes"; then
  if test -e compat/arpa/nameser.h; then
    if test "x${CPPFLAGS}" != "x"; then
      test -n "${CPPFLAGS}" && export CPPFLAGS="${CPPFLAGS} -Icompat"
    else
      test -z "${CPPFLAGS}" && export CPPFLAGS="-Icompat"
    fi
  elif test -x "`which locate`"; then
    locate arpa/nameser.h
  else
    AC_MSG_WARN([unsure how to find <arpa/nameser.h>])
  fi
else
  AC_MSG_NOTICE([assuming <arpa/nameser.h> is fine])
fi

#----------------------------------------------------------------------------
#       Check for various Unix library functions that can be used.
#----------------------------------------------------------------------------

AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_CHECK_FUNC([strerror],
              [AC_DEFINE([HAVE_STRERROR],[1],[strerror function available])])
# TODO gethostent is deprecated by POSIX, instead use getaddrinfo et.al.
AC_CHECK_FUNC([gethostent],
              [AC_DEFINE([HAVE_GETHOSTENT],[1],[gethostent function available])])
AC_CHECK_FUNC([getnetent],
              [AC_DEFINE([HAVE_GETNETENT],[1],[getnetent function available])])
AC_CHECK_FUNC([getprotoent],
              [AC_DEFINE([HAVE_GETPROTOENT],[1],[getprotoent function available])])
AC_CHECK_FUNC([getservent],
              [AC_DEFINE([HAVE_GETSERVENT],[1],[getservent function available])])
AC_CHECK_FUNC([getrpcent],
              [AC_DEFINE([HAVE_GETRPCENT],[1],[getrpcent function available])])

#----------------------------------------------------------------------------
#       Check if we want/need to use the libtirpc alternative rpc
#       implementation.
#----------------------------------------------------------------------------

AC_ARG_WITH([tirpc],
            [AS_HELP_STRING([--with-tirpc],
                            [Build with the tirpc rpc implementation])])dnl

AS_IF([test "x${with_tirpc}" = "xyes"],[
	  if pkg-config libtirpc; then
	      TIRPC_CFLAGS=`pkg-config --cflags libtirpc`
	      TIRPC_LIBS=`pkg-config --libs libtirpc`
	      CPPFLAGS="${TIRPC_CFLAGS} ${CPPFLAGS}"
	  else
	      AC_MSG_ERROR([could not determine how to configure tirpc])
	  fi
      ])dnl

#----------------------------------------------------------------------------
#       Some older SUN RPC implementations are really ugly.
#----------------------------------------------------------------------------

AC_MSG_CHECKING([for struct rpcent])
AC_EGREP_CPP([^struct * rpcent],[
#include <netdb.h>
#include <rpc/rpc.h>
],[AC_MSG_RESULT([yes]); AC_DEFINE([HAVE_RPCENT],[1],[rpcent structure defined in system libraries])],[AC_MSG_RESULT([no])])dnl


#----------------------------------------------------------------------------
#       Checking for multicast options.
#----------------------------------------------------------------------------

AC_ARG_ENABLE([multicast], 
	[AS_HELP_STRING([--enable-multicast],[enable support for multicasts])],
	[mcast_ok=$enableval], [mcast_ok=no])
if test "x${mcast_ok}" = "xyes"; then
    AC_MSG_CHECKING([for multicast support])
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>]], [[int i=IP_MULTICAST_TTL, j=IP_ADD_MEMBERSHIP;]])],[AC_MSG_RESULT([yes]); AC_DEFINE([HAVE_MULTICAST],[1],[multicast enabled])],[AC_MSG_RESULT([no])])
fi

#----------------------------------------------------------------------------
#       Checking for the libsmi library.
#----------------------------------------------------------------------------

AC_CHECK_HEADERS([smi.h])
AC_CHECK_LIB([smi],[smiInit],[LIBS="${LIBS} -lsmi"])
AC_MSG_CHECKING([whether to enable libsmi])
        AC_RUN_IFELSE([AC_LANG_SOURCE([[ /* libsmi available check */
#include <smi.h>
int main(void)
{
  int current, revision, age, n;
  const int required = 2;
  if (smiInit("")) 
    exit(1);
  if (strcmp(SMI_LIBRARY_VERSION, smi_library_version))
    exit(2);
  n = sscanf(smi_library_version, "%d:%d:%d", &current, &revision, &age);
  if (n != 3)
    exit(3);
  if (required < current - age || required > current)
    exit(4);
  exit(0);
}
]])],[ AC_MSG_RESULT([yes])
  AC_DEFINE([HAVE_SMI_H])
  AC_DEFINE([LIBSMI],[1],[libsmi available])
  libsmi=yes],[ AC_MSG_RESULT([no])
  libsmi=no],[ AC_MSG_RESULT([not when cross-compiling])
  libsmi=no
])dnl

#----------------------------------------------------------------------------
#       Some more libraries for OSes that want your attention.
#----------------------------------------------------------------------------

AC_FUNC_GETMNTENT

#----------------------------------------------------------------------------
#       Required programs
#----------------------------------------------------------------------------

#--------------------------------------------------------------------
# Load the tclConfig.sh file
#--------------------------------------------------------------------

TEA_PATH_TCLCONFIG
TEA_LOAD_TCLCONFIG

#-----------------------------------------------------------------------
# Handle the --prefix=... option by defaulting to what Tcl gave.
# Must be called after TEA_LOAD_TCLCONFIG and before TEA_SETUP_COMPILER.
#-----------------------------------------------------------------------

TEA_PREFIX

#-----------------------------------------------------------------------
# Standard compiler checks.
# This sets up CC by using the CC env var, or looks for gcc otherwise.
# This also calls AC_PROG_CC, AC_PROG_INSTALL and a few others to create
# the basic setup necessary to compile executables.
#-----------------------------------------------------------------------

TEA_SETUP_COMPILER

#-----------------------------------------------------------------------
# Specify the C/C++ source files and any other subsidiary files
# that are involved in the build.
#-----------------------------------------------------------------------

# generic tnm
TEA_ADD_SOURCES([
		   generic/tnmInit.c
		   generic/tnmUtil.c
		   generic/tnmJob.c
		   generic/tnmSyslog.c
		   generic/tnmIcmp.c
		   generic/tnmDns.c
		   generic/tnmUdp.c
		   generic/tnmNtp.c
		   generic/tnmNetdb.c
		   generic/tnmMap.c
		   generic/tnmMapUtil.c
		   generic/tnmMapEvent.c
		   generic/tnmMapNode.c
		   generic/tnmMapNet.c
		   generic/tnmMapLink.c
		   generic/tnmMapPort.c
		   generic/tnmMapGroup.c
		   generic/tnmObj.c
	       ])
# snmp
TEA_ADD_SOURCES([
		   snmp/tnmAsn1.c 
		   snmp/tnmOidObj.c 
		   snmp/tnmMD5.c 
		   snmp/tnmSHA.c 
		   snmp/tnmSnmpNet.c 
		   snmp/tnmSnmpUtil.c 
		   snmp/tnmSnmpUsm.c 
		   snmp/tnmSnmpInst.c 
		   snmp/tnmSnmpTcl.c 
		   snmp/tnmSnmpSend.c 
		   snmp/tnmSnmpRecv.c 
		   snmp/tnmSnmpAgent.c 
		   snmp/tnmMibUtil.c 
		   snmp/tnmMibParser.c 
		   snmp/tnmMibTree.c 
		   snmp/tnmMibFrozen.c 
		   snmp/tnmMibTcl.c 
		   snmp/tnmSmiTcl.c 
	       ])
TEA_ADD_HEADERS([])
TEA_ADD_INCLUDES([${TIRPC_CFLAGS} -I. -Igeneric -Isnmp])
TEA_ADD_LIBS([])
TEA_ADD_CFLAGS([])
TEA_ADD_STUB_SOURCES([])
# This would be nice to use, but the current setup hardcodes related
# Tcl files to reside under $(pkglibdir)/library.
TEA_ADD_TCL_SOURCES([])

#--------------------------------------------------------------------
# __CHANGE__
#
# You can add more files to clean if your extension creates any extra
# files by extending CLEANFILES.
# Add pkgIndex.tcl if it is generated in the Makefile instead of ./configure
# and change Makefile.in to move it from CONFIG_CLEAN_FILES to BINARIES var.
#
# A few miscellaneous platform-specific items:
# TEA_ADD_* any platform specific compiler/build info here.
#--------------------------------------------------------------------

CLEANFILES="${CLEANFILES} nmicmpd nmtrapd"
# RPC stub files not generated (Sun RPC support disabled)
# CLEANFILES="${CLEANFILES} ether_* ether.h"
# CLEANFILES="${CLEANFILES} mount_* mount.h"
# CLEANFILES="${CLEANFILES} pcnfsd_* pcnfsd.h"
# CLEANFILES="${CLEANFILES} rstat_* rstat.h"
CLEANFILES="${CLEANFILES} autoscan.log"

if test "x${TEA_PLATFORM}" = "xwindows"; then
    # Ensure no empty if clauses
    :
    if test -e win/winFile.c; then
        TEA_ADD_SOURCES([win/winFile.c])
    else
        AC_MSG_NOTICE([missing wanted sourcefile win/winFile.c])
    fi
    TEA_ADD_SOURCES([
                    win/tnmWinInit.c
                    win/tnmWinLog.c
                    win/tnmWinSocket.c
                    win/tnmWinIcmp.c
                    compat/tnm_compat.c
                   ])
    TEA_ADD_LIBS([-lws2_32 -ltcl86])
    SHLIB_LD="${SHLIB_LD} -Wl,--export-all-symbols"
    if test -d "$(${CYGPATH} ${srcdir}/win)"; then
        TEA_ADD_INCLUDES([-I\"$(${CYGPATH} ${srcdir}/win)\"])
    else
        AC_MSG_NOTICE([missing wanted include directory ${srcdir}/win])
    fi
else
    # Ensure no empty else clauses
    :
    TEA_ADD_SOURCES([
		       unix/tnmUnixInit.c
		       unix/tnmUnixLog.c
		       unix/tnmUnixSocket.c
		       unix/tnmUnixIcmp.c
		       unix/tnmUnixSnmp.c
		   ])
    TEA_ADD_LIBS([${TIRPC_LIBS}])
fi

#--------------------------------------------------------------------
# __CHANGE__
# Choose which headers you need.  Extension authors should try very
# hard to only rely on the Tcl public header files.  Internal headers
# contain private data structures and are subject to change without
# notice.
# This MUST be called after TEA_LOAD_TCLCONFIG / TEA_LOAD_TKCONFIG
#--------------------------------------------------------------------

TEA_PUBLIC_TCL_HEADERS

if test "x${CPPFLAGS}" = "x"; then
  if test -d compat; then
    echo "making sure to include the compat directory..."
    test -z "${CPPFLAGS}" && export CPPFLAGS="-Icompat"
  else
    AC_MSG_WARN([compat directory missing])
  fi
else
  echo "CPPFLAGS are ${CPPFLAGS}"
fi
AC_SUBST([CPPFLAGS])dnl

#--------------------------------------------------------------------
# Check whether --enable-threads or --disable-threads was given.
# This auto-enables if Tcl was compiled threaded.
#--------------------------------------------------------------------

TEA_ENABLE_THREADS

#-----------------------------------------------------------------------
# The statement below defines a collection of symbols related to
# building as a shared library instead of a static library.
#-----------------------------------------------------------------------

TEA_ENABLE_SHARED

#-----------------------------------------------------------------------
# This macro figures out what flags to use with the compiler/linker
# when building shared/static debug/optimized objects.  This information
# can be taken from the tclConfig.sh file, but this figures it all out.
#-----------------------------------------------------------------------

TEA_CONFIG_CFLAGS

#-----------------------------------------------------------------------
# Set the default compiler switches based on the --enable-symbols option.
#-----------------------------------------------------------------------

TEA_ENABLE_SYMBOLS

#-----------------------------------------------------------------------
# This macro generates a line to use when building a library.  It
# depends on values set by the TEA_ENABLE_SHARED, TEA_ENABLE_SYMBOLS,
# and TEA_LOAD_TCLCONFIG macros above.
#-----------------------------------------------------------------------

TEA_MAKE_LIB

#-----------------------------------------------------------------------
# Find tclsh so that we can run pkg_mkIndex to create the pkgIndex.tcl
# file during the install process. Don't run the TCLSH_PROG through
# ${CYGPATH} because it's being used directly by make.
# Require that we use a tclsh shell version 8.2 or later since earlier
# versions have bugs in the pkg_mkIndex routine.
# Add WISH as well if this is a Tk extension.
#-----------------------------------------------------------------------

TEA_PROG_TCLSH

#----------------------------------------------------------------------------
#       The list of symbols to substitute.
#----------------------------------------------------------------------------

AC_SUBST([TNM_VERSION])dnl
AC_SUBST([TKI_VERSION])dnl

# AC_SUBST([RPCGEN])dnl  # Not needed - Sun RPC support disabled
AC_SUBST([NM_LIBS])dnl

AC_SUBST([THREADS_LIBS])dnl
AC_SUBST([LD_SEARCH_FLAGS])dnl

#--------------------------------------------------------------------
# Finally, substitute all of the various values into the Makefile.
# You may alternatively have a special pkgIndex.tcl.in or other files
# which require substituting the AC variables in. Include these here.
#-----------------------------------------------------------------------

AC_CONFIG_FILES([Makefile pkgIndex.tcl])
AC_OUTPUT
