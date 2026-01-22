# Tcl package index file, version 1.0
# This file is hand crafted so that the packages are loaded immediately.
#
# $Id: pkgIndex.tcl,v 1.1.1.1 2006/12/07 12:16:57 karl Exp $

foreach pkg {
    tnmDialog tnmTerm tnmInet tnmMap tnmIetf tnmEther tnmMonitor
    tnmSnmp tnmMib tnmScriptMib tnmSmxProfiles
} {
    package ifneeded $pkg 3.1.3 [list source [file join $dir $pkg.tcl]]
}

