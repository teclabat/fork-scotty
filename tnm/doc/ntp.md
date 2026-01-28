# tnm::ntp - NTP Time Protocol Commands

Retrieve NTP status information from time servers.

**Status:** ✅ 100% functional on Windows and Linux

---

## Synopsis

```tcl
tnm::ntp [options]
tnm::ntp [options] host arrayName
```

---

## Description

The `tnm::ntp` command retrieves control variables from NTP (Network Time Protocol) peers by sending NTP version 3 mode 6 requests. This allows monitoring of time synchronization status across network devices.

NTP is defined in RFC 1119 and RFC 1305.

---

## Commands

### tnm::ntp [options]

Query or set default NTP options.

```tcl
# Get current timeout
set timeout [tnm::ntp -timeout]

# Set default timeout to 5 seconds
tnm::ntp -timeout 5
```

### tnm::ntp [options] host arrayName

Query NTP status from a host and store results in an array.

```tcl
tnm::ntp 192.168.1.1 ntpStatus
puts "Stratum: $ntpStatus(sys.stratum)"
puts "Offset: $ntpStatus(peer.offset)"
```

---

## Options

| Option | Default | Description |
|--------|---------|-------------|
| `-timeout seconds` | 2 | Time to wait for response |
| `-retries number` | 2 | Number of retransmission attempts |

---

## Result Array Elements

After successful completion, the array contains the following elements:

### Peer Information

| Element | Description |
|---------|-------------|
| `peer.delay` | Roundtrip delay to peer clock in seconds |
| `peer.dispersion` | Maximum error of peer clock in seconds |
| `peer.offset` | Offset of peer clock relative to local clock in seconds |
| `peer.precision` | Precision of clocks (power of two, in seconds) |
| `peer.reach` | Reachability shift register (peer reachable if any bit is set) |
| `peer.srcadr` | IP address of the peer |
| `peer.stratum` | Stratum level of the local clock |
| `peer.valid` | Valid samples remaining in filter register |

### System Information

| Element | Description |
|---------|-------------|
| `sys.peer` | Selector identifying current synchronization source |
| `sys.precision` | Precision of various clocks (power of two, in seconds) |
| `sys.refid` | 32-bit code identifying the reference clock |
| `sys.rootdelay` | Total roundtrip delay to primary reference source |
| `sys.rootdispersion` | Maximum error relative to primary reference source |
| `sys.stratum` | Stratum level of the local clock |
| `sys.system` | Textual description of the system type |

---

## Examples

### Basic NTP Query

```tcl
package require tnm 3.1.3

proc checkNtpServer {host} {
    if {[catch {tnm::ntp $host status} err]} {
        puts "Failed to query $host: $err"
        return
    }

    puts "NTP Status for $host:"
    puts "  System:     $status(sys.system)"
    puts "  Stratum:    $status(sys.stratum)"
    puts "  Reference:  $status(sys.refid)"
    puts "  Offset:     $status(peer.offset) seconds"
    puts "  Delay:      $status(peer.delay) seconds"
    puts "  Dispersion: $status(peer.dispersion) seconds"
}

checkNtpServer pool.ntp.org
```

### Monitor Multiple NTP Servers

```tcl
package require tnm 3.1.3

set servers {
    time.nist.gov
    pool.ntp.org
    time.google.com
}

puts [format "%-20s %-8s %-12s %-12s" "Server" "Stratum" "Offset" "Delay"]
puts [string repeat "-" 55]

foreach server $servers {
    if {[catch {tnm::ntp -timeout 5 $server status}]} {
        puts [format "%-20s %-8s" $server "TIMEOUT"]
        continue
    }

    puts [format "%-20s %-8s %-12.6f %-12.6f" \
        $server \
        $status(sys.stratum) \
        $status(peer.offset) \
        $status(peer.delay)]
}
```

### Check Time Synchronization Health

```tcl
package require tnm 3.1.3

proc checkTimeHealth {host {maxOffset 0.1} {maxStratum 4}} {
    if {[catch {tnm::ntp $host status} err]} {
        return [list status error message $err]
    }

    set warnings {}

    # Check stratum
    if {$status(sys.stratum) > $maxStratum} {
        lappend warnings "High stratum: $status(sys.stratum)"
    }

    # Check offset
    if {abs($status(peer.offset)) > $maxOffset} {
        lappend warnings "Large offset: $status(peer.offset)s"
    }

    # Check reachability
    if {$status(peer.reach) == 0} {
        lappend warnings "Peer not reachable"
    }

    if {[llength $warnings] == 0} {
        return [list status ok stratum $status(sys.stratum) \
                     offset $status(peer.offset)]
    } else {
        return [list status warning warnings $warnings \
                     stratum $status(sys.stratum) \
                     offset $status(peer.offset)]
    }
}

set result [checkTimeHealth 192.168.1.1]
puts "Time sync status: [dict get $result status]"
```

### NTP Monitoring with Job Scheduler

```tcl
package require tnm 3.1.3

set ntpServer "pool.ntp.org"

proc monitorNtp {} {
    global ntpServer

    if {[catch {tnm::ntp $ntpServer status} err]} {
        puts "[clock format [clock seconds]]: NTP query failed: $err"
        return
    }

    set offset $status(peer.offset)
    set stratum $status(sys.stratum)

    if {abs($offset) > 0.5} {
        puts "[clock format [clock seconds]]: WARNING - Large offset: ${offset}s"
    } else {
        puts "[clock format [clock seconds]]: OK - Stratum $stratum, offset ${offset}s"
    }
}

# Check every 5 minutes
set j [tnm::job create -command monitorNtp -interval 300000]

vwait forever
```

---

## Understanding NTP Values

### Stratum Levels

| Stratum | Description |
|---------|-------------|
| 0 | Unspecified or invalid |
| 1 | Primary reference (GPS, atomic clock, etc.) |
| 2 | Secondary reference (synced to stratum 1) |
| 3-15 | Subsequent levels of synchronization |
| 16 | Unsynchronized |

### Offset

The offset value indicates how far the local clock is from the reference. Positive values mean the local clock is ahead; negative values mean it's behind. Well-synchronized systems typically have offsets in milliseconds or less.

### Dispersion

Dispersion represents the maximum error bound. Lower values indicate better precision. Values increase over time when synchronization is lost.

### Reachability

The reach value is a shift register where each bit represents a polling interval. A value of 255 (all bits set) indicates the peer has been reachable for the last 8 polling intervals.

---

## Error Handling

```tcl
if {[catch {tnm::ntp $host status} err]} {
    switch -glob -- $err {
        "*timeout*" {
            puts "NTP server not responding"
        }
        "*refused*" {
            puts "Connection refused - NTP not running"
        }
        default {
            puts "NTP error: $err"
        }
    }
}
```

---

## Notes

- NTP uses UDP port 123
- The command sends NTPv3 mode 6 (control) messages
- Firewall rules may need to allow UDP 123 for queries to work
- Some NTP servers may not respond to control queries
- Timeout is specified in seconds (not milliseconds like some other commands)

---

## See Also

- [tnm.md](tnm.md) - Main TNM documentation
- RFC 1305 - Network Time Protocol (Version 3)
- RFC 5905 - Network Time Protocol Version 4
