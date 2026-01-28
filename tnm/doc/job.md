# tnm::job - Job Scheduler Commands

Execute Tcl procedures at regular intervals.

**Status:** ✅ 91% functional (sub-interpreter limitations)

---

## Synopsis

```tcl
tnm::job create [options]
tnm::job find [options]
tnm::job current
tnm::job schedule ms script
tnm::job wait

$job configure [options]
$job cget option
$job destroy
```

---

## Description

The `tnm::job` command provides a mechanism to invoke Tcl procedures at regular intervals. Jobs are activated from the Tcl event loop and are useful for periodic monitoring, polling, and scheduled tasks.

Jobs are never interrupted due to the event-driven nature of Tcl. Long computations or blocking commands can affect scheduling accuracy.

---

## Job Commands

### tnm::job create [options]

Create a new job.

```tcl
# Basic job running every second
set j [tnm::job create -command {puts "tick"} -interval 1000]

# Job with limited iterations
set j [tnm::job create \
    -command {puts "count"} \
    -interval 500 \
    -iterations 10]

# Job with error handling
set j [tnm::job create \
    -command {doWork} \
    -error {puts "Error: $errorInfo"} \
    -exit {puts "Job finished"} \
    -interval 5000]
```

### tnm::job find [options]

Find existing jobs.

```tcl
# Find all jobs
set jobs [tnm::job find]

# Find by status
set running [tnm::job find -status running]
set waiting [tnm::job find -status waiting]

# Find by tags
set monitors [tnm::job find -tags "monitor"]
```

### tnm::job current

Get the currently executing job.

```tcl
set j [tnm::job current]
if {$j ne ""} {
    puts "Current job: $j"
}
```

### tnm::job schedule ms script

Schedule a one-time execution after a delay.

```tcl
tnm::job schedule 5000 {puts "Delayed message"}
```

### tnm::job wait

Wait for all jobs to complete.

```tcl
# Wait for all jobs to finish (blocks until all expire)
tnm::job wait
```

---

## Job Instance Commands

### $job configure [options]

Configure job options.

```tcl
$j configure -interval 2000
$j configure -command {newCommand}
$j configure -status suspended
```

### $job cget option

Get a job option value.

```tcl
set interval [$j cget -interval]
set status [$j cget -status]
set iterations [$j cget -iterations]
```

### $job destroy

Destroy the job.

```tcl
$j destroy
```

---

## Job Options

| Option | Description |
|--------|-------------|
| `-command cmd` | Tcl command to execute on each activation |
| `-error cmd` | Command to execute on errors (prevents bgerror) |
| `-exit cmd` | Command to execute when job ends |
| `-interval ms` | Interval between activations in milliseconds |
| `-iterations n` | Total number of activations (0 = unlimited) |
| `-status state` | Job state: waiting, suspended, running, expired |
| `-tags tagList` | Tags for grouping/finding jobs |

### Status Values

| Status | Description |
|--------|-------------|
| `waiting` | Waiting for next scheduled activation |
| `suspended` | Temporarily paused (resume by setting to waiting) |
| `running` | Currently executing |
| `expired` | Completed all iterations (will be cleaned up) |

---

## Examples

### Basic Periodic Task

```tcl
package require tnm 3.1.3

set count 0
set j [tnm::job create -command {
    incr ::count
    puts "[clock format [clock seconds]]: Tick $::count"
} -interval 1000]

# Run for 10 seconds then stop
after 10000 {
    $::j destroy
    set ::done 1
}

vwait done
puts "Completed $count iterations"
```

### Monitoring Job with Error Handling

```tcl
package require tnm 3.1.3

proc checkHost {host} {
    set result [tnm::icmp -timeout 2000 echo $host]
    lassign [lindex $result 0] addr rtt

    if {$rtt eq ""} {
        error "Host $host is unreachable"
    }
    puts "[clock format [clock seconds]]: $host is up (${rtt}ms)"
}

set j [tnm::job create \
    -command {checkHost 192.168.1.1} \
    -error {
        puts "ERROR: $::errorInfo"
        tnm::syslog warning "Host check failed"
    } \
    -interval 10000]

vwait forever
```

### Limited Iterations

```tcl
package require tnm 3.1.3

# Run exactly 5 times
set j [tnm::job create \
    -command {puts "Iteration [incr ::i]"} \
    -exit {puts "All done!"} \
    -interval 1000 \
    -iterations 5]

set i 0
tnm::job wait
```

### Suspend and Resume

```tcl
package require tnm 3.1.3

set j [tnm::job create -command {puts "working..."} -interval 1000]

# Suspend after 5 seconds
after 5000 {
    puts "Suspending..."
    $::j configure -status suspended
}

# Resume after 10 seconds
after 10000 {
    puts "Resuming..."
    $::j configure -status waiting
}

# Stop after 15 seconds
after 15000 {
    $::j destroy
    set ::done 1
}

vwait done
```

### Tagged Job Groups

```tcl
package require tnm 3.1.3

# Create multiple monitoring jobs
foreach host {192.168.1.1 192.168.1.2 192.168.1.3} {
    tnm::job create \
        -command [list checkHost $host] \
        -interval 30000 \
        -tags "monitor ping"
}

# Find and manage all monitor jobs
proc stopAllMonitors {} {
    foreach j [tnm::job find -tags "monitor"] {
        $j destroy
    }
}
```

### Job with State

```tcl
package require tnm 3.1.3

# Using namespace for job state
namespace eval monitor {
    variable count 0
    variable host "192.168.1.1"

    proc tick {} {
        variable count
        variable host
        incr count

        set result [tnm::icmp echo $host]
        lassign [lindex $result 0] addr rtt

        if {$rtt ne ""} {
            puts "Check $count: $host OK (${rtt}ms)"
        } else {
            puts "Check $count: $host FAILED"
        }
    }
}

set j [tnm::job create \
    -command {monitor::tick} \
    -interval 5000]

vwait forever
```

---

## Notes

### Event Loop Required

Jobs only execute when the Tcl event loop is running:

```tcl
# This is required for jobs to run
vwait forever

# Or use update to process events
while {1} {
    update
    after 100
}
```

### System Clock Sensitivity

The job scheduler depends on the system clock. Moving the clock backwards may delay job activations. Moving the clock forward may cause jobs to activate earlier than expected.

### Sub-Interpreter Limitation

Creating jobs in child interpreters may have limitations. This is a known issue affecting approximately 9% of functionality.

---

## See Also

- [tnm.md](tnm.md) - Main TNM documentation
- Tcl `after` command - For simpler timing needs
