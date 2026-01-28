# tnm::icmp - ICMP Message Commands

Send ICMP packets for network diagnostics including ping and traceroute.

**Status:** ✅ 100% functional (mask/timestamp Linux only)

---

## Synopsis

```tcl
tnm::icmp [options] echo hosts
tnm::icmp [options] mask hosts
tnm::icmp [options] timestamp hosts
tnm::icmp [options] ttl num hosts
tnm::icmp [options] trace num hosts
```

---

## Description

The `tnm::icmp` command allows sending ICMP (Internet Control Message Protocol) messages to test host reachability, measure round-trip times, and trace routes through the network.

---

## Commands

### tnm::icmp echo hosts

Send ICMP echo requests (ping) to test host reachability.

**Syntax:**
```tcl
tnm::icmp [options] echo hosts
```

**Parameters:**
- `hosts` - Single host or list of hosts to ping

**Returns:** List of {host rtt} pairs where rtt is in milliseconds (empty if unreachable)

**Example:**
```tcl
# Ping single host
set result [tnm::icmp echo 192.168.1.1]
# Returns: {{192.168.1.1 5}}

# Ping multiple hosts
set results [tnm::icmp echo {192.168.1.1 192.168.1.2 8.8.8.8}]
# Returns: {{192.168.1.1 5} {192.168.1.2 3} {8.8.8.8 15}}

# Check results
foreach {host rtt} $results {
    if {$rtt eq ""} {
        puts "$host: unreachable"
    } else {
        puts "$host: ${rtt}ms"
    }
}
```

---

### tnm::icmp mask hosts

Request network masks from hosts using ICMP address mask requests.

**Syntax:**
```tcl
tnm::icmp [options] mask hosts
```

**Parameters:**
- `hosts` - Single host or list of hosts

**Returns:** List of {host netmask} pairs

**Example:**
```tcl
set masks [tnm::icmp mask 192.168.1.1]
# Returns: {{192.168.1.1 255.255.255.0}}
```

**Platform Note:** This command is only available on **Linux**. It has been removed from Windows due to ICMP.DLL API limitations.

---

### tnm::icmp timestamp hosts

Request timestamps from hosts to measure time differences.

**Syntax:**
```tcl
tnm::icmp [options] timestamp hosts
```

**Parameters:**
- `hosts` - Single host or list of hosts

**Returns:** List of {host timestamp_info} pairs

**Example:**
```tcl
set times [tnm::icmp timestamp 192.168.1.1]
```

**Platform Note:** This command is only available on **Linux**. It has been removed from Windows due to ICMP.DLL API limitations.

---

### tnm::icmp ttl num hosts

Send ICMP echo with a specific TTL (Time To Live) value to reach a specific hop.

**Syntax:**
```tcl
tnm::icmp [options] ttl num hosts
```

**Parameters:**
- `num` - TTL value (hop count)
- `hosts` - Target host(s)

**Returns:** List of {router rtt} pairs for the router at that hop

**Example:**
```tcl
# Get router at hop 5
set hop5 [tnm::icmp ttl 5 8.8.8.8]
puts "Router at hop 5: [lindex $hop5 0]"
```

---

### tnm::icmp trace num hosts

Trace route to hosts using the Van Jacobsen algorithm.

**Syntax:**
```tcl
tnm::icmp [options] trace num hosts
```

**Parameters:**
- `num` - Maximum number of hops
- `hosts` - Destination host(s)

**Returns:** List of {router rtt} pairs for each hop along the route

**Example:**
```tcl
# Trace route with max 30 hops
set route [tnm::icmp trace 30 8.8.8.8]

set hop 1
foreach {router rtt} $route {
    if {$router eq ""} {
        puts "$hop: * * *"
    } else {
        puts "$hop: $router (${rtt}ms)"
    }
    incr hop
}
```

---

## Options

| Option | Default | Description |
|--------|---------|-------------|
| `-timeout time` | 5000 | Timeout in milliseconds |
| `-retries number` | 2 | Number of retries per target |
| `-delay time` | 0 | Delay between packets in milliseconds (max 255) |
| `-size number` | 64 | ICMP packet size in bytes (64-65535) |
| `-window size` | 10 | Maximum concurrent asynchronous requests |

### Setting Options

Options can be set for individual commands:

```tcl
tnm::icmp -timeout 10000 -retries 5 echo 192.168.1.1
```

Or set as defaults for the interpreter:

```tcl
tnm::icmp -timeout 10000 -retries 5
# Now all subsequent ICMP commands use these defaults
```

---

## Important Notes

### Breaking Change: Timeout Units

**The `-timeout` and `-delay` options are now in milliseconds on both platforms.**

Previous versions may have used seconds on some platforms. Ensure your code uses millisecond values:

```tcl
# Correct: 5 second timeout
tnm::icmp -timeout 5000 echo 192.168.1.1

# Wrong: This is only 5 milliseconds!
tnm::icmp -timeout 5 echo 192.168.1.1
```

### Platform Requirements

**Linux:**
- Requires the `nmicmpd` daemon with setuid root privileges
- The daemon is needed because raw ICMP sockets require root access
- Set the `TNM_NMICMPD` environment variable if not in default location

**Windows:**
- Uses the Windows ICMP.DLL API
- No special privileges required
- `mask` and `timestamp` commands are not available

---

## Examples

### Basic Ping

```tcl
package require tnm 3.1.3

set host "8.8.8.8"
set result [tnm::icmp echo $host]

lassign [lindex $result 0] addr rtt
if {$rtt ne ""} {
    puts "Reply from $addr: time=${rtt}ms"
} else {
    puts "Request timed out"
}
```

### Network Sweep

```tcl
package require tnm 3.1.3

# Build list of hosts in subnet
set hosts {}
for {set i 1} {$i <= 254} {incr i} {
    lappend hosts "192.168.1.$i"
}

# Ping all hosts (uses windowing for efficiency)
set results [tnm::icmp -timeout 1000 echo $hosts]

# Report live hosts
puts "Live hosts:"
foreach {host rtt} $results {
    if {$rtt ne ""} {
        puts "  $host (${rtt}ms)"
    }
}
```

### Traceroute

```tcl
package require tnm 3.1.3

proc traceroute {dest {maxhops 30}} {
    puts "Tracing route to $dest (max $maxhops hops):"
    puts ""

    set route [tnm::icmp -timeout 3000 trace $maxhops $dest]

    set hop 1
    foreach {router rtt} $route {
        if {$router eq ""} {
            puts [format "%2d  * * * Request timed out" $hop]
        } else {
            # Try reverse DNS lookup
            if {[catch {tnm::dns name $router} name]} {
                set name $router
            }
            puts [format "%2d  %s (%s) %s ms" $hop $name $router $rtt]
        }
        incr hop
    }
}

traceroute 8.8.8.8
```

### Monitor Host Availability

```tcl
package require tnm 3.1.3

proc monitor {host interval} {
    set j [tnm::job create -interval $interval -command [list checkHost $host]]
}

proc checkHost {host} {
    set result [tnm::icmp -timeout 2000 echo $host]
    lassign [lindex $result 0] addr rtt

    set time [clock format [clock seconds] -format "%H:%M:%S"]
    if {$rtt ne ""} {
        puts "$time $host: UP (${rtt}ms)"
    } else {
        puts "$time $host: DOWN"
        tnm::syslog warning "Host $host is unreachable"
    }
}

# Monitor every 10 seconds
monitor 192.168.1.1 10000
vwait forever
```

---

## Error Handling

```tcl
if {[catch {tnm::icmp echo 192.168.1.1} result]} {
    puts "ICMP error: $result"
} else {
    # Process results
}
```

---

## See Also

- [tnm.md](tnm.md) - Main TNM documentation
- [tnm::dns](dns.md) - DNS queries for hostname resolution
- RFC 792 - Internet Control Message Protocol
