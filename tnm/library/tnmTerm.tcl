# tnmTerm.tcl --
#
#	This file contains the definition of an output or terminal
#	window, which is basically a text widget with scrollbars
#	and a menu. The menu contains a set of standard commands
#	to send email or to save/load the text to/from files.
#	This package is derived from the Tkined sources.
#
# Copyright (c) 1995-1996 Technical University of Braunschweig.
# Copyright (c) 1996-1997 University of Twente.
# Copyright (c) 1997-1998 Technical University of Braunschweig.
#
# See the file "license.terms" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# @(#) $Id: tnmTerm.tcl,v 1.1.1.1 2006/12/07 12:16:57 karl Exp $

package require tnm 3.1
package require tnmInet 3.1
package require tnmDialog 3.1
package provide tnmTerm 3.1.3

namespace eval tnmTerm {
    namespace export open close clear write setName setIcon
    namespace export toggleFreeze toggleWrap toggleXScroll toggleYScroll
    namespace export save load print email
}

# Definitions for virtual events that are used to define the 
# keyboard short-cuts in the menus.

event add <<TnmTermClear>>	<Alt-c> <Alt-C> <Meta-c> <Meta-C>
event add <<TnmTermOpen>>	<Alt-o> <Alt-O> <Meta-o> <Meta-O>
event add <<TnmTermSave>>	<Alt-a> <Alt-A> <Meta-a> <Meta-A>
event add <<TnmTermPrint>>	<Alt-p> <Alt-P> <Meta-p> <Meta-P>
event add <<TnmTermEmail>>	<Alt-e> <Alt-E> <Meta-e> <Meta-E>
event add <<TnmTermView>>	<Alt-n> <Alt-N> <Meta-n> <Meta-N>
event add <<TnmTermClose>>	<Alt-w> <Alt-W> <Meta-w> <Meta-W>
event add <<TnmTermFind>>	<Alt-f> <Alt-F> <Meta-f> <Meta-F>
event add <<TnmTermFilter>>	<Alt-t> <Alt-T> <Meta-t> <Meta-T>
event add <<TnmTermFreeze>>	<Alt-z> <Alt-Z> <Meta-z> <Meta-Z>
event add <<TnmTermWrap>>	<Alt-v> <Alt-V> <Meta-v> <Meta-V>
event add <<TnmTermScrollX>>	<Alt-x> <Alt-X> <Meta-x> <Meta-X>
event add <<TnmTermScrollY>>	<Alt-y> <Alt-Y> <Meta-y> <Meta-Y>

# tnmTerm::open --
#
#	Create an output window and define a proc named writeln 
#	to write to the text widget in this window. A hack, but 
#	a good one. :-)
#
# Arguments:
#	w 	The output window that should be created.
# Results:
#	None.

proc tnmTerm::open w {

    variable state

    if {![winfo exists $w]} {

	toplevel $w -menu $w.menu
	menu $w.menu -tearoff 0

	text $w.t -wrap none -highlightthickness 0 -setgrid true \
		-relief sunken -borderwidth 2 \
		-yscrollcommand "$w.yscroll set" \
		-xscrollcommand "$w.xscroll set"
	scrollbar $w.yscroll -orient vertical \
		-command "$w.t yview" -relief sunken
	scrollbar $w.xscroll -orient horizontal \
		-command "$w.t xview" -relief sunken

	grid rowconfig    $w 0 -weight 1 -minsize 0
	grid columnconfig $w 0 -weight 1 -minsize 0
	grid $w.t       -in $w -row 0 -column 0 -sticky news
	grid $w.yscroll -in $w -row 0 -column 1 \
		-rowspan 1 -columnspan 1 -sticky news
	grid $w.xscroll -in $w -row 1 -column 0 \
		-rowspan 1 -columnspan 1 -sticky news

	$w.menu add cascade -label "File" -menu $w.menu.file
	menu $w.menu.file -tearoff 0
	$w.menu.file add command -label "Clear" \
		-accelerator "  Alt+C" \
		-command "tnmTerm::clear $w"
	bind $w <<TnmTermClear>> "$w.menu.file invoke Clear"
	$w.menu.file add command -label "Open..." \
		-accelerator "  Alt+O" \
		-command "tnmTerm::load $w"
	bind $w <<TnmTermOpen>> "$w.menu.file invoke Open..."
	$w.menu.file add command -label "Save As..." \
		-accelerator "  Alt+A" \
		-command "tnmTerm::save $w"
	bind $w <<TnmTermSave>> "$w.menu.file invoke {Save As...}"
	$w.menu.file add sep
	$w.menu.file add command -label "Print..." \
		-accelerator "  Alt+P" \
		-command "tnmTerm::print $w"
	bind $w <<TnmTermPrint>> "$w.menu.file invoke Print..."
	$w.menu.file add command -label "Email..." \
		-accelerator "  Alt+E" \
		-command "tnmTerm::email $w"
	bind $w <<TnmTermEmail>> "$w.menu.file invoke Email..."
	$w.menu.file add sep
	$w.menu.file add command -label "New View" \
		-accelerator "  Alt+N" \
		-command "tnmTerm::newView $w"
	bind $w <<TnmTermView>> "$w.menu.file invoke {New View}"
	$w.menu.file add command -label "Close View" \
		-accelerator "  Alt+W" \
		-command "tnmTerm::close $w"
	bind $w <<TnmTermClose>> "$w.menu.file invoke {Close View}"

	$w.menu add cascade -label "Find" -menu $w.menu.find
	menu $w.menu.find -tearoff 0
	$w.menu.find add command -label "Find..." \
		-accelerator "  Alt+F" \
		-command "tnmTerm::find $w"
	bind $w <<TnmTermFind>> "$w.menu.find invoke {Find...}"
if {0} {
	$w.menu.find add separator
	$w.menu.find add command -label "Filter..." \
		-accelerator "  Alt+T" \
		-command "tnmTerm::editFilter $w"
	bind $w <<TnmTermFilter>> "$w.menu.find invoke {Filter...}"
}

	$w.menu add cascade -label "Options" -menu $w.menu.options
	menu $w.menu.options -tearoff 0
	$w.menu.options add checkbutton -label "Freeze" \
		-offvalue 0 -onvalue 1 -variable tnmTerm::state(freeze,$w) \
		-accelerator "  Alt+Z"
	set state(freeze,$w) 0
	bind $w <<TnmTermFreeze>> "$w.menu.options invoke Freeze"
	$w.menu.options add checkbutton -label "Wrap" \
		-offvalue none -onvalue word \
		-variable tnmTerm::state(wrap,$w) \
		-accelerator "  Alt+V" \
		-command "$w.t configure -wrap \[set tnmTerm::state(wrap,$w)\]"
	set state(wrap,$w) none
	bind $w <<TnmTermWrap>> "$w.menu.options invoke Wrap"
	$w.menu.options add separator
	$w.menu.options add checkbutton -label "X Scroll" \
		-offvalue 0 -onvalue 1 -variable tnmTerm::state(xscroll,$w) \
		-accelerator "  Alt+X" -command "tnmTerm::toggleXScroll $w"
        tnmTerm::toggleXScroll $w
	bind $w <<TnmTermScrollX>> "$w.menu.options invoke {X Scroll}"
	$w.menu.options add checkbutton -label "Y Scroll" \
		-offvalue 0 -onvalue 1 -variable tnmTerm::state(yscroll,$w) \
		-accelerator "  Alt+Y" -command "tnmTerm::toggleYScroll $w"
	$w.menu.options invoke "Y Scroll"
        tnmTerm::toggleYScroll $w
	bind $w <<TnmTermScrollY>> "$w.menu.options invoke {Y Scroll}"

	static offset
	if {![info exists offset]} {
	    set offset 80
	} else {
	    incr offset 10
	    if {$offset > 180} {set offset 80}
	}

	wm withdraw $w
	update idletasks
	set top [winfo toplevel [winfo parent $w]]
	
	set rx [expr {[winfo rootx $top]}]
	set ry [expr {[winfo rooty $top]}]
	
	set cx [expr $rx+[winfo width $top]/4]
	set cy [expr $ry+[winfo height $top]/4]
	
	set x  [expr $cx+$offset]
	set y  [expr $cy+$offset]
	
	if {$x < 0} { set x 0 }
	if {$y < 0} { set y 0 }
	
	wm geometry $w +$x+$y
	wm deiconify $w
	
	update
    }

    return
}


# tnmTerm::newView --
#
#	Create a new output window (aka a new view).
#
# Arguments:
#	w	The widget path of the current terminal window.
# Results:
#	None.

proc tnmTerm::newView w {
    set parent [winfo parent $w]
    if {[string compare $parent "."] == 0} {
	set parent ""
    }
    for {set i 1} 1 {incr i} {
        set view $parent.view$i
        if ![winfo exists $view] {
            break
        }
    }
    tnmTerm::open $view
    return
}


# tnmTerm::close --
#
#	Close a terminal window.
#
# Arguments:
#	w 	The widget path of the terminal window to close.
# Results:
#	None.

proc tnmTerm::close w {
    destroy $w
    return
}


# tnmTerm::clear --
#
#	Clear the contents of the output window.
#
# Arguments:
#	w 	The widget path of the terminal window to clear.
# Results:
#	None.

proc tnmTerm::clear w {
    $w.t delete 0.0 end
    return
}


# tnmTerm::write --
#
#	Write a text string in the output terminal window.
#
# Arguments:
#	w	The widget path of the terminal window.
#	txt	The text to write into the window.
# Results:
#	None.

proc tnmTerm::write {w txt} {
    variable state
    $w.t insert end $txt
    if !$state(freeze,$w) {
	$w.t yview -pickplace end
    }
    return
}


# tnmTerm::setName --
#
#	Set the name of an output terminal window, which is displayed
#	in the window decorations as well as in the icon.
#
# Arguments:
#	w	The widget path of the terminal window.
#	name	The name of the terminal window to be assigned.
# Results:
#	None.

proc tnmTerm::setName { w name } {
    wm title $w $name
    wm iconname $w $name
    return
}


# tnmTerm::setIcon --
#
#	Set the icon bitmap of an output terminal window.
#
# Arguments:
#	w	The widget path of the terminal window.
#	bitmap	The bitmap to be assigned.
# Results:
#	None.

proc tnmTerm::setIcon { w bitmap } {
    wm iconbitmap $w $bitmap
    return
}


# tnmTerm::toggleFreeze --
#
#	Toggle the freeze state of an output terminal window.
#
# Arguments:
#	w	The widget path of the terminal window.
# Results:
#	None.

proc tnmTerm::toggleFreeze w {
    variable state
    $w.menu.options invoke Freeze
    return $state(freeze,$w)
}


# tnmTerm::toggleWrap --
#
#	Toggle the text wrap feature of an output terminal window.
#
# Arguments:
#	w	The widget path of the terminal window.
# Results:
#	None.

proc tnmTerm::toggleWrap w {
    variable state
    $w.menu.options invoke Wrap
    return [expr {$state(wrap,$w) == "word"}]
}


# tnmTerm::toggleXScroll --
#
#	Toggle the horizontal scrollbar of an output terminal window.
#
# Arguments:
#	w	The widget path of the terminal window.
# Results:
#	None.

proc tnmTerm::toggleXScroll w {
    variable state
    if $state(xscroll,$w) {
	grid $w.xscroll -in $w -row 1 -column 0 \
                -rowspan 1 -columnspan 1 -sticky news
    } else {
	grid forget $w.xscroll
    }
    return $state(xscroll,$w)
}


# tnmTerm::toggleYScroll --
#
#	Toggle the vertical scrollbar of an output terminal window.
#
# Arguments:
#	w	The widget path of the terminal window.
# Results:
#	None.

proc tnmTerm::toggleYScroll w {
    variable state
    if $state(yscroll,$w) {
	grid $w.yscroll -in $w -row 0 -column 1 \
                -rowspan 1 -columnspan 1 -sticky news
    } else {
	grid forget $w.yscroll
    }
    return $state(yscroll,$w)
}


# tnmTerm::save --
#
#	Save the contents of the output terminal window in a file.
#
# Arguments:
#	w	The widget path of the terminal window.
# Results:
#	None.

proc tnmTerm::save w {

    set types {
	{{Text Files}	{.txt}	}
	{{All Files}	*	}
    }

    set fname [tk_getSaveFile -defaultextension .txt -filetypes $types \
	    -parent $w -title "Write to file:"]
    if {$fname==""} return
    set mode "w"
    if {[file exists $fname]} {
	set result [tnm_dialogConfirm $w.r info \
		"File $fname already exists!" [list replace append cancel]]
	switch [lindex $result 0] {
	    "cancel" {
		return
	    }
	    "replace" {
		set mode "w"
	    }
	    "append" {
		set mode "a"
	    }
	}
    }
    if {[catch {::open $fname $mode} file]} {
	tnm_dialogConfirm $w.r error "Unable to open $fname." ok
	return
    }
    puts $file [$w.t get 1.0 end]
    ::close $file
    return
}


# tnmTerm::load --
#
#	Read the contents of the output terminal window from a file.
#
# Arguments:
#	w	The widget path of the terminal window.
# Results:
#	None.

proc tnmTerm::load w {

    set types {
	{{Text Files}	{.txt}	}
	{{All Files}	*	}
    }

    set fname [tk_getOpenFile -defaultextension .txt -filetypes $types \
	    -parent $w -title "Read from file:"]
    if {$fname == ""} return
    if {[catch {::open $fname r} file]} {
	tnm_dialogConfirm $w.r error "Unable to read from $fname" ok
	return
    }
    $w.t delete 0.0 end
    $w.t insert end [read $file]
    $w.t yview 1.0
    ::close $file
    return
}


# tnmTerm::print --
#
#	Send the contents of the output terminal window to the
#	printer.
# 
# Arguments:
#	w	The widget path of the terminal window.
# Results:
#	None.

proc tnmTerm::print { w } {
    global env
    variable tnm

    set fname "$tnm(tmp)/output-[pid]"
    catch {file delete -force $fname}
    if {[file exists $fname] && ![file writable $fname]} {
	tnm_dialogConfirm $w.r error "Can not write temporary file $fname." ok
	return
    }
    if {[catch {::open $fname w} file]} {
	tnm_dialogConfirm $w.r error "Can not open $fname: $file" ok
	return
    }

    if {[catch {puts $file [$w.t get 1.0 end]} err]} {
	tnm_dialogConfirm $w.r error "Failed to write $fname: $err" ok
    }
    ::close $file
    foreach dir [split $env(PATH) ":"] {
	if {[file executable $dir/lpr]} {
	    set lpr $dir/lpr
	    break
	}
    }
    set res [tnm_dialogRequest $w.r questhead \
	    "Saved to temporary file $fname.\n\nEnter print command:" \
	    $lpr "print cancel" ]
    if {[lindex $res 0] == "print"} {
	set cmd [lindex $res 1]
	if {[catch {eval exec $cmd $fname} err]} {
	    tnm_dialogConfirm $w.r error "$lpr $fname failed:\n$err" ok
	}
    }
    catch {file delete -force $fname}
    return
}


# tnmTerm::email --
#
#	Send the contents of the output terminal window as an 
#	email message.
#
# Arguments:
#	w	The widget path of the terminal window.
# Results:
#	None.

proc tnmTerm::email w {
    global env
    variable tnm

    if ![info exists tnm(email)] {
	if [catch {set tnm(email) "$tnm(user)@$tnm(domain)"}] {
	    if [info exists env(USER)] {
		set tnm(email) $env(USER)
	    } else {
		set tnm(email) ""
	    }
	}
    }

    set res [tnm_dialogRequest $w.r questhead \
	    "Please enter the email address:" $tnm(email) "ok cancel"]
    if {[lindex $res 0] == "cancel"} return
    set to [lindex $res 1]
    if {$to == ""} {
	tnm_dialogConfirm $w.r warning "Ignoring empty email address." ok
        return
    }
    set res [tnm_dialogRequest $w.r questhead \
	    "Please enter the subject of this email:" \
	    "\[tnmTerm Output\]" "ok none cancel"]
    switch [lindex $res 0] {
	cancel return
	none {
	    set subject ""
	}
	ok {
	    set subject [lindex $res 1]
	}
    }

    if {[catch {tnmInet::sendMail $to [$w.t get 1.0 end] $subject} msg]} {
        tnm_dialogConfirm $w.r error "Unable send mail: $msg" ok
    }
    return
}


# tnmTerm::find --
#
#	Find a regular expression in the text of the output
#	terminal window.
#
# Arguments:
#	w	The widget path of the terminal window.
# Results:
#	None.

proc tnmTerm::find w {
    variable state
    if {! [info exists state(find,$w)]} {
	set state(find,$w) ""
    }
    set result [tnm_dialogRequest $w.find questhead \
	    "Find the following regular expression:" \
	    $state(find,$w) "find clear cancel" ]
    if {[lindex $result 0] == "cancel"} return
    $w.t tag delete found
    if {[lindex $result 0] == "clear"} return
    set regexp [lindex $result 1]
    set state(find,$w) $regexp
    set index "0.0"
    while {[set index [$w.t search -nocase -regexp $regexp $index end]] != ""} {
	$w.t tag add found "$index linestart" "$index lineend"
	set index [$w.t index "$index + 1 line linestart"]
    }
    $w.t tag configure found -relief raised -underline true
    return
}


# tnmTerm::editFilter --
#
#	Edit the filter expression that defines which text is 
#	displayed in the output terminal window.
#
# Arguments
#	w	The widget path of the terminal window.
# Results:
#	None.

proc tnmTerm::editFilter w {
    variable state
    if {! [info exists state(filter,$w)]} {
	set state(filter,$w) .
    }
    set result [tnm_dialogRequest $w.filter questhead \
	    "Edit the regular filter expression:" \
	    $state(filter,$w) "accept cancel" ]
    if {[lindex $result 0] == "cancel"} return
    set state(filter,$w) [lindex $result 1]
    return
}
