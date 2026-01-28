# TNM 3.1.3 Quick Reference

## Installation

```tcl
package require tnm 3.1.3
namespace import tnm::*
```

---

## Command Status Summary

| Command | Windows | Linux | Notes |
|---------|---------|-------|-------|
| tnm::dns | ✅ 100% | ✅ 100% | `-server` option Linux only |
| tnm::icmp | ✅ 100% | ✅ 100% | mask/timestamp Linux only |
| tnm::snmp | ✅ 100% | ✅ 100% | Full v1/v2c/v3 |
| tnm::mib | ✅ 100% | ✅ 100% | 33 subcommands |
| tnm::udp | ✅ 100% | ✅ 100% | |
| tnm::job | ✅ 91% | ✅ 91% | Sub-interp limits |
| tnm::netdb | ✅ 100% | ✅ 100% | sunrpcs removed |
| tnm::ntp | ✅ 100% | ✅ 100% | time + status |
| tnm::syslog | ✅ 100% | ✅ 100% | |
| tnm::map | ✅ 100% | ✅ 100% | |

**Removed:** tnm::smx, tnm::ined, tnm::netdb sunrpcs

---

## tnm::dns - DNS Queries

```tcl
tnm::dns address host           ;# Get IP address(es)
tnm::dns name address           ;# Reverse lookup (PTR)
tnm::dns cname name             ;# Canonical name
tnm::dns hinfo name             ;# Host info
tnm::dns mx name                ;# Mail exchangers
tnm::dns ns name                ;# Name servers
tnm::dns soa name               ;# Start of authority
tnm::dns txt name               ;# Text records
```

**Options:** `-timeout secs` (def: 2), `-retries num` (def: 2), `-server addr` (Linux only)

---

## tnm::icmp - ICMP Operations

```tcl
tnm::icmp echo hosts            ;# Ping -> {{host rtt} ...}
tnm::icmp mask hosts            ;# Get netmasks (Linux only)
tnm::icmp timestamp hosts       ;# Time diff (Linux only)
tnm::icmp ttl num hosts         ;# Echo at specific hop
tnm::icmp trace num hosts       ;# Traceroute
```

**Options:** `-timeout ms` (def: 5000), `-retries num` (def: 2), `-delay ms` (def: 0, max 255), `-size bytes` (def: 64), `-window num` (def: 10)

**Note:** Timeout/delay in **milliseconds** on both platforms

---

## tnm::snmp - SNMP Protocol

### Session Types
```tcl
set s [tnm::snmp generator -address HOST -read COMMUNITY]
set l [tnm::snmp listener -port 162]
set r [tnm::snmp responder]
set n [tnm::snmp notifier -address HOST]
```

### Session Options
| Option | Default | Description |
|--------|---------|-------------|
| `-address` | 127.0.0.1 | Peer address |
| `-port` | 161/162 | UDP port |
| `-version` | SNMPv1 | SNMPv1/SNMPv2c/SNMPv3 |
| `-read` | public | Read community |
| `-write` | - | Write community |
| `-user` | - | SNMPv3 username |
| `-context` | - | SNMPv3 context |
| `-timeout` | 5000 | Timeout (ms) |
| `-retries` | 3 | Retry count |

### Generator Commands
```tcl
$s get vbl ?script?             ;# GET request
$s getnext vbl ?script?         ;# GETNEXT request
$s getbulk nr mr vbl ?script?   ;# GETBULK (v2c/v3)
$s set vbl ?script?             ;# SET request
$s walk varname vbl body        ;# Walk subtree
$s configure -option value      ;# Configure
$s cget -option                 ;# Get option
$s destroy                      ;# Cleanup
```

### Utility Commands
```tcl
tnm::snmp alias name options    ;# Create alias
tnm::snmp find ?-address? ?-tags?  ;# Find sessions
tnm::snmp info subject          ;# Get info
tnm::snmp wait                  ;# Wait for async
```

### Callback Escapes
| Escape | Description |
|--------|-------------|
| `%V` | Varbind list |
| `%E` | Error status |
| `%A` | Peer address |
| `%S` | Session name |
| `%I` | Error index |

---

## tnm::mib - MIB Operations (33 subcommands)

### OID/Name Conversion
```tcl
tnm::mib oid name               ;# Name -> OID
tnm::mib name oid               ;# OID -> Name
tnm::mib label oid              ;# Get label only
```

### MIB Queries
```tcl
tnm::mib load file              ;# Load MIB file
tnm::mib info loaded            ;# List loaded MIBs
tnm::mib syntax node            ;# ASN.1 syntax
tnm::mib type node              ;# Derived type
tnm::mib access node            ;# Access level
tnm::mib status node            ;# Status
tnm::mib description node       ;# Description
tnm::mib module node            ;# Module name
tnm::mib children node          ;# Child nodes
tnm::mib parent node            ;# Parent node
tnm::mib exists node            ;# Check existence
tnm::mib file module            ;# MIB file path
```

### Value Operations
```tcl
tnm::mib format node value      ;# Format for display
tnm::mib scan node value        ;# Parse value
tnm::mib enums node             ;# Enum values
tnm::mib member node value      ;# Enum member name
tnm::mib range node             ;# Value range
tnm::mib size node              ;# Size constraints
tnm::mib defval node            ;# Default value
tnm::mib displayhint node       ;# Display hint
```

### OID Operations
```tcl
tnm::mib compare oid1 oid2      ;# Compare (-1/0/1)
tnm::mib subtree oid1 oid2      ;# Check containment
tnm::mib length oid             ;# OID length
tnm::mib split oid              ;# Split {base instance}
```

### Table Operations
```tcl
tnm::mib index entry            ;# Index columns
tnm::mib variables entry        ;# Table columns
tnm::mib pack entry values      ;# Build instance OID
tnm::mib unpack oid             ;# Extract index
```

### Walking
```tcl
tnm::mib walk oidVar labelVar node body
```

---

## tnm::udp - UDP Sockets

```tcl
set u [tnm::udp create -myport PORT]
tnm::udp find -tags pattern

$u send host port message       ;# Send datagram
$u connect host port            ;# Connect
$u send message                 ;# Send (connected)
set data [$u receive]           ;# Receive -> {addr port msg}
$u configure -option value
$u cget -option
$u destroy
```

**Options:** `-address`, `-port`, `-myaddress`, `-myport`, `-read cmd`, `-write cmd`, `-tags`

---

## tnm::job - Job Scheduler

```tcl
set j [tnm::job create -command cmd -interval ms -iterations n]
tnm::job current                ;# Current job
tnm::job find ?-status? ?-tags? ;# Find jobs
tnm::job schedule ms script     ;# One-time
tnm::job wait                   ;# Wait all

$j configure -option value
$j cget -option
$j destroy
```

**Options:** `-command`, `-error`, `-exit`, `-interval` (ms), `-iterations`, `-status`, `-tags`

**Status:** `waiting`, `suspended`, `running`, `expired`

---

## tnm::netdb - Network Database

### Hosts
```tcl
tnm::netdb hosts address name   ;# Name -> IP
tnm::netdb hosts name addr      ;# IP -> Name
tnm::netdb hosts aliases arg    ;# Get aliases
```

### Services
```tcl
tnm::netdb services name port proto    ;# Port -> Name
tnm::netdb services number name proto  ;# Name -> Port
tnm::netdb services aliases arg proto
```

### Protocols
```tcl
tnm::netdb protocols name num   ;# Number -> Name
tnm::netdb protocols number name ;# Name -> Number
tnm::netdb protocols aliases arg
```

### Networks
```tcl
tnm::netdb networks name addr   ;# Addr -> Name
tnm::netdb networks address name ;# Name -> Addr
tnm::netdb networks aliases arg
```

### IP Operations
```tcl
tnm::netdb ip class addr        ;# IP class (A/B/C/D)
tnm::netdb ip apply addr mask   ;# Apply netmask
tnm::netdb ip broadcast addr mask  ;# Broadcast addr
tnm::netdb ip compare m1 m2     ;# Compare masks
tnm::netdb ip range addr mask   ;# IP range list
```

---

## tnm::ntp - NTP Queries

```tcl
# Time query (works with all public NTP servers)
set r [tnm::ntp time pool.ntp.org]
dict get $r time      ;# Unix timestamp
dict get $r offset    ;# Offset (seconds)
dict get $r delay     ;# Delay (seconds)
dict get $r stratum   ;# Stratum level
dict get $r precision ;# Precision
dict get $r refid     ;# Reference ID

# Status query (control mode - may be disabled)
set r [tnm::ntp status ntpserver]
```

**Options:** `-timeout secs` (def: 2), `-retries num` (def: 2)

---

## tnm::syslog - System Logging

```tcl
tnm::syslog emergency msg
tnm::syslog alert msg
tnm::syslog critical msg
tnm::syslog error msg
tnm::syslog warning msg
tnm::syslog notice msg
tnm::syslog info msg
tnm::syslog debug msg
```

**Options:** `-ident string` (def: scotty), `-facility name` (def: local0)

---

## tnm::map - Network Maps

```tcl
set m [tnm::map create -name "Name"]
tnm::map find pattern
tnm::map info option

$m configure -option value
$m cget -option
$m destroy
```

**Options:** `-name`, `-tags`, `-command`

---

## Common Patterns

### Ping Multiple Hosts
```tcl
set results [tnm::icmp echo {192.168.1.1 192.168.1.2}]
foreach {host rtt} $results {
    puts "$host: ${rtt}ms"
}
```

### SNMP Table Walk
```tcl
set s [tnm::snmp generator -address 192.168.1.1 -read public]
$s walk row "ifDescr ifOperStatus" {
    puts "[lindex $row 0]: [lindex $row 2]"
}
$s destroy
```

### Async SNMP Get
```tcl
set s [tnm::snmp generator -address 192.168.1.1]
$s get {sysDescr.0} {
    if {"%E" == "noError"} {
        puts "Result: %V"
    }
}
tnm::snmp wait
$s destroy
```

### Periodic Monitoring
```tcl
set j [tnm::job create -command {
    puts "[clock format [clock seconds]]: tick"
} -interval 10000]
vwait forever
```

### NTP Time Check
```tcl
set r [tnm::ntp time pool.ntp.org]
puts "Time: [clock format [dict get $r time]]"
puts "Offset: [dict get $r offset]s"
```

---

## Environment Variables

| Variable | Description |
|----------|-------------|
| `TNM_LIBRARY` | Library path |
| `TNM_NMICMPD` | nmicmpd path (Linux) |
| `TNM_NMTRAPD` | nmtrapd path |
| `TNM_RCFILE` | RC file path |

---

## Tcl Variables

| Variable | Description |
|----------|-------------|
| `tnm(version)` | Version number |
| `tnm(host)` | Local hostname |
| `tnm(domain)` | DNS domain |
| `tnm(user)` | Username |
| `tnm(library)` | Library path |
| `tnm(mibs)` | MIBs to load |
| `tnm(tmp)` | Temp directory |

---

## Error Handling

```tcl
# Synchronous
if {[catch {$s get $vbl} result]} {
    puts "Error: $result"
}

# Async (callback)
$s get $vbl {
    if {"%E" != "noError"} {
        puts "SNMP Error: %E"
    }
}
```

---

## See Also

- Full documentation: `doc/tnm.md`
- Platform status: `TNMOVERVIEW.md`
- GitHub: https://github.com/flightaware/scotty
