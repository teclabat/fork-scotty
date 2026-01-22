# tnmScriptMib.tcl --
#
#	This file implements a set of procedures to interact with
#	a remote scripting engine via the IETF Script MIB.
#
# Copyright (c) 1998-2000 Technical University of Braunschweig.
#
# See the file "license.terms" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# @(#) $Id: tnmScriptMib.tcl,v 1.1.1.1 2006/12/07 12:16:57 karl Exp $

package require tnm 3.1
package require tnmSnmp 3.1
package provide tnmScriptMib 3.1.3

namespace eval tnmScriptMib {
    namespace export getLanguages getExtensions

    namespace export installScript deleteScript
    namespace export setScriptStorage setScriptDescription setScriptSource
    namespace export setScriptLanguage enableScript disableScript

    namespace export installLaunch deleteLaunch enableLaunch disableLaunch

    namespace export suspendRun resumeRun abortRun deleteRun
}

proc tnmScriptMib::getLanguages {s} {

    set result {}
    lappend vbl [list smLangLanguage]
    $s walk vbl $vbl {
	set lang [tnm::mib unpack [tnm::snmp oid $vbl 0]]
	set langID [tnm::snmp value $vbl 0]
	lappend result [list $lang $langID]
    }

    return $result
}

proc tnmScriptMib::getExtensions {s language} {

    set result {}
    lappend vbl [list smExtsnExtension.$language]
    $s walk vbl $vbl {
	set extsn [lindex [tnm::mib unpack [tnm::snmp oid $vbl 0]] 1]
	set extsnID [tnm::snmp value $vbl 0]
	lappend result [list $extsn $extsnID]
    }

    return $result
}

proc tnmScriptMib::installScript {s owner name language source descr} {

    lappend vbl [list [tnm::mib pack smScriptRowStatus $owner $name] createAndGo]
    lappend vbl [list [tnm::mib pack smScriptLanguage $owner $name] $language]
    lappend vbl [list [tnm::mib pack smScriptSource $owner $name] $source]
    lappend vbl [list [tnm::mib pack smScriptDescr $owner $name] $descr]

    if {[catch {$s set $vbl} vbl]} {
	error "unable to create row: $vbl"
    }

    return
}

proc tnmScriptMib::deleteScript {s owner name} {
    
    tnmScriptMib::UnloadScript $s $owner $name

    lappend vbl [list [tnm::mib pack smScriptRowStatus $owner $name] destroy]
    if {[catch {$s set $vbl} vbl]} {
        error "unable to destroy row: $vbl"
    }

    return
}

proc tnmScriptMib::enableScript {s owner name {timeout 60}} {

    lappend vbl [list [tnm::mib pack smScriptAdminStatus $owner $name] enabled]
    if {[catch {$s set $vbl} vbl]} {
        error "unable to enable script: $vbl"
    }

    set transient {disabled retrieving compiling}
    set status "disabled"
    for {set n $timeout} {$n >= 0} {incr n -1} {
        if {[catch {$s get [tnm::mib pack smScriptOperStatus $owner $name]} vbl]} {
            error "unable to obtain script status: $vbl"
        }
        set status [tnm::snmp value $vbl 0]
	if {[lsearch $transient $status] < 0} break
        after 1000
    }
    if {$status != "enabled"} {
        error "unable to enable script: $vbl"
    }

    return
}

proc tnmScriptMib::disableScript {s owner name {timeout 60}} {

    lappend vbl [list [tnm::mib pack smScriptAdminStatus $owner $name] disabled]
    if {[catch {$s set $vbl} vbl]} {
        error "unable to enable script: $vbl"
    }

    set status "enabled"
    for {set n $timeout} {$n >= 0} {incr n -1} {
        if {[catch {$s get [tnm::mib pack smScriptOperStatus $owner $name]} vbl]} {
            error "unable to obtain script status: $vbl"
        }
        set status [tnm::snmp value $vbl 0]
	if {$status == "disabled"} break
        after 1000
    }
    if {$status != "disabled"} {
        error "unable to disable script: $vbl"
    }

    return
}

proc tnmScriptMib::setScriptStorage {s owner name storage} {

    lappend vbl [list [tnm::mib pack smScriptStorageType $owner $name] $storage]
    if {[catch {$s set $vbl} vbl]} {
        error "unable to set storage: $vbl"
    }

    return
}

proc tnmScriptMib::setScriptDescription {s owner name description} {

    lappend vbl [list [tnm::mib pack smScriptDescr $owner $name] $description]
    if {[catch {$s set $vbl} vbl]} {
        error "unable to set description: $vbl"
    }

    return
}

proc tnmScriptMib::setScriptSource {s owner name source} {
    tnmScriptMib::UnloadScript $s $owner $name

    lappend vbl [list [tnm::mib pack smScriptSource $owner $name] $source]
    if {[catch {$s set $vbl} vbl]} {
        error "unable to set script source: $vbl"
    }

    return
}

proc tnmScriptMib::setScriptLanguage {s owner name language} {
    tnmScriptMib::UnloadScript $s $owner $name

    lappend vbl [list [tnm::mib pack smScriptLanguage $owner $name] $language]
    if {[catch {$s set $vbl} vbl]} {
        error "unable to set script source: $vbl"
    }

    return
}

proc tnmScriptMib::installLaunch {s owner name sowner sname argument lifetime expiretime maxrunning maxcompleted} {

    lappend vbl [list [tnm::mib pack smLaunchRowStatus $owner $name] createAndGo]
    lappend vbl [list [tnm::mib pack smLaunchScriptOwner $owner $name] $sowner]
    lappend vbl [list [tnm::mib pack smLaunchScriptName $owner $name] $sname]
    lappend vbl [list [tnm::mib pack smLaunchArgument $owner $name] $argument]
    lappend vbl [list [tnm::mib pack smLaunchLifeTime $owner $name] $lifetime]
    lappend vbl [list [tnm::mib pack smLaunchExpireTime $owner $name] $expiretime]
    lappend vbl [list [tnm::mib pack smLaunchMaxRunning $owner $name] $maxrunning]
    lappend vbl [list [tnm::mib pack smLaunchMaxCompleted $owner $name] $maxcompleted]

    if {[catch {$s set $vbl} vbl]} {
	error "unable to create row: $vbl"
    }

    return
}

proc tnmScriptMib::deleteLaunch {s owner name} {
    
    lappend vbl [list [tnm::mib pack smLaunchRowStatus $owner $name] destroy]
    if {[catch {$s set $vbl} vbl]} {
        error "unable to destroy row: $vbl"
    }

    return
}

proc tnmScriptMib::enableLaunch {s owner name {timeout 60}} {

    lappend vbl [list [tnm::mib pack smLaunchAdminStatus $owner $name] enabled]
    if {[catch {$s set $vbl} vbl]} {
        error "unable to enable launch: $vbl"
    }

    set transient {disabled}
    set status "disabled"
    for {set n $timeout} {$n >= 0} {incr n -1} {
        if {[catch {$s get [tnm::mib pack smLaunchOperStatus $owner $name]} vbl]} {
            error "unable to obtain launch status: $vbl"
        }
        set status [tnm::snmp value $vbl 0]
	if {[lsearch $transient $status] < 0} break
        after 1000
    }
    if {$status != "enabled"} {
        error "unable to enable launch: $vbl"
    }

    return
}

proc tnmScriptMib::disableLaunch {s owner name {timeout 60}} {

    lappend vbl [list [tnm::mib pack smLaunchAdminStatus $owner $name] disabled]
    if {[catch {$s set $vbl} vbl]} {
        error "unable to enable launch: $vbl"
    }

    set status "enabled"
    for {set n $timeout} {$n >= 0} {incr n -1} {
        if {[catch {$s get [tnm::mib pack smLaunchOperStatus $owner $name]} vbl]} {
            error "unable to obtain launch status: $vbl"
        }
        set status [tnm::snmp value $vbl 0]
	if {$status == "disabled"} break
        after 1000
    }
    if {$status != "disabled"} {
        error "unable to disable launch: $vbl"
    }

    return
}

proc tnmScriptMib::suspendRun {s owner name index {timeout 60}} {

    lappend vbl [list [tnm::mib pack smRunControl $owner $name $index] suspend]
    if {[catch {$s set $vbl} vbl]} {
        error "unable to suspend running script: $vbl"
    }

    set transient {suspending executing}
    set status "suspending"
    for {set n $timeout} {$n >= 0} {incr n -1} {
        if {[catch {$s get [tnm::mib pack smRunState $owner $name $index]} vbl]} {
            error "unable to obtain running script status: $vbl"
        }
        set status [tnm::snmp value $vbl 0]
	if {[lsearch $transient $status] < 0} break
        after 1000
    }
    if {$status != "suspended"} {
        error "unable to suspend running script: $vbl"
    }

    return
}

proc tnmScriptMib::resumeRun {s owner name index {timeout 60}} {

    lappend vbl [list [tnm::mib pack smRunControl $owner $name $index] resume]
    if {[catch {$s set $vbl} vbl]} {
        error "unable to resume running script: $vbl"
    }

    set transient {resuming suspended}
    set status "resuming"
    for {set n $timeout} {$n >= 0} {incr n -1} {
        if {[catch {$s get [tnm::mib pack smRunState $owner $name $index]} vbl]} {
            error "unable to obtain running script status: $vbl"
        }
        set status [tnm::snmp value $vbl 0]
	if {[lsearch $transient $status] < 0} break
        after 1000
    }
    if {$status != "executing"} {
        error "unable to resume running script: $vbl"
    }

    return
}

proc tnmScriptMib::abortRun {s owner name index {timeout 60}} {

    lappend vbl [list [tnm::mib pack smRunControl $owner $name $index] abort]
    if {[catch {$s set $vbl} vbl]} {
        error "unable to abort running script: $vbl"
    }

    set transient {aborting executing suspended suspending resuming initializing}
    set status "aborting"
    for {set n $timeout} {$n >= 0} {incr n -1} {
        if {[catch {$s get [tnm::mib pack smRunState $owner $name $index]} vbl]} {
            error "unable to obtain running script status: $vbl"
        }
        set status [tnm::snmp value $vbl 0]
	if {[lsearch $transient $status] < 0} break
        after 1000
    }
    if {$status != "terminated"} {
        error "unable to abort running script: $vbl"
    }

    return
}

proc tnmScriptMib::deleteRun {s owner name index} {
    
    lappend vbl [list [tnm::mib pack smRunExpireTime $owner $name $index] 0]
    if {[catch {$s set $vbl} vbl]} {
        error "unable to expire row: $vbl"
    }

    return
}

