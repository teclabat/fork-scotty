# TNM - Tcl Network Management Extension

A comprehensive Tcl extension for network management applications, providing string-based APIs to access network protocols and services including SNMP, ICMP, DNS, UDP, and more.

**Version:** 3.1.3 \
**Package:** `tnm` \
**Namespace:** `tnm::` \
**Platforms:** Windows (MinGW64) & Linux \
**Authors:** Juergen Schoenwaelder, Erik Schoenfelder, and contributors \
**Maintainer:** FlightAware (https://github.com/flightaware/scotty)

---

## Table of Contents

1. [Overview](#overview)
2. [Key Features](#key-features)
3. [Installation](#installation)
4. [Command Reference](#command-reference)
   - 4.1 [tnm::dns - DNS Queries](#tnmdns---dns-queries)
   - 4.2 [tnm::icmp - ICMP Messages](#tnmicmp---icmp-messages)
   - 4.3 [tnm::snmp - SNMP Protocol](#tnmsnmp---snmp-protocol)
   - 4.4 [tnm::mib - MIB Management](#tnmmib---mib-management)
   - 4.5 [tnm::udp - UDP Datagrams](#tnmudp---udp-datagrams)
   - 4.6 [tnm::job - Job Scheduler](#tnmjob---job-scheduler)
   - 4.7 [tnm::netdb - Network Database](#tnmnetdb---network-database)
   - 4.8 [tnm::ntp - NTP Status](#tnmntp---ntp-status)
   - 4.9 [tnm::syslog - System Logging](#tnmsyslog---system-logging)
   - 4.10 [tnm::map - Network Maps](#tnmmap---network-maps)
5. [Utility Packages](#utility-packages)
6. [Environment Variables](#environment-variables)
7. [Tcl Variables](#tcl-variables)
8. [Examples](#examples)
9. [Platform-Specific Notes](#platform-specific-notes)
10. [See Also](#see-also)
11. [License](#license)

---

## Overview

The **TNM** (Tcl Network Management) extension simplifies the development of network management applications. It provides a string-based API to access network protocols and services relevant for network management applications, including:

- SNMP (v1, v2c, v3) for device monitoring and control
- ICMP for reachability testing and route tracing
- DNS for hostname resolution
- UDP for custom datagram communication
- NTP for time synchronization status
- MIB database operations

The use of Tcl allows integration with the Tk toolkit, enabling cost-effective and platform-independent prototyping of management applications.

---

## Key Features

- **SNMP Support**: Full SNMPv1, SNMPv2c, and SNMPv3 implementation
- **ICMP Operations**: Ping, traceroute (mask/timestamp on Linux only)
- **DNS Queries**: Forward/reverse lookups, MX, NS, SOA, TXT, CNAME, HINFO records
- **MIB Management**: Load, parse, and query SNMP MIB definitions (33 subcommands)
- **UDP Communication**: Send and receive UDP datagrams
- **Job Scheduling**: Execute Tcl procedures at regular intervals
- **Network Database**: Access local host, network, and service information
- **NTP Integration**: Retrieve time and status from NTP servers
- **System Logging**: Write messages to syslog (Unix) or Event Log (Windows)
- **Network Mapping**: Create and manipulate network topology maps

---

## Installation

```tcl
package require tnm 3.1.3
```

### Importing Commands

All TNM commands live in the `::tnm` namespace:

```tcl
# Use full namespace prefix
tnm::icmp echo 192.168.1.1

# Or import specific commands
namespace import tnm::icmp

# Or import all commands
namespace import tnm::*
```

### Requirements

**Build-time:**
- Tcl 8.6 or later
- C compiler (MinGW64 on Windows, GCC on Linux)

**Runtime:**
- Tcl 8.6+
- On Linux: nmicmpd daemon (setuid root) for ICMP operations

---

## Command Reference

### tnm::dns - DNS Queries

Query the Domain Name System (DNS) for host information.

**Status:** ✅ 100% functional on Windows and Linux

#### Commands

**`tnm::dns [options] address host`**

Get IP addresses for a hostname.

```tcl
set ips [tnm::dns address www.example.com]
# Returns: {93.184.216.34}
```

**`tnm::dns [options] name address`**

Reverse lookup - get hostname from IP address.

```tcl
set name [tnm::dns name 8.8.8.8]
# Returns: dns.google
```

**`tnm::dns [options] cname name`**

Get canonical name (CNAME) record.

```tcl
set canonical [tnm::dns cname www.example.com]
```

**`tnm::dns [options] hinfo name`**

Get host information record (hardware type and OS).

```tcl
set info [tnm::dns hinfo host.example.com]
```

**`tnm::dns [options] mx name`**

Get mail exchanger (MX) records.

```tcl
set mxlist [tnm::dns mx example.com]
# Returns: {{mail1.example.com 10} {mail2.example.com 20}}
```

**`tnm::dns [options] ns name`**

Get name server (NS) records for a domain.

```tcl
set servers [tnm::dns ns example.com]
```

**`tnm::dns [options] soa name`**

Get start of authority (SOA) record.

```tcl
set auth [tnm::dns soa example.com]
```

**`tnm::dns [options] txt name`**

Get text (TXT) records.

```tcl
set txt [tnm::dns txt example.com]
```

#### DNS Options

| Option | Default | Description |
|--------|---------|-------------|
| `-server server` | System default | DNS server(s) to query (Linux only) |
| `-timeout time` | 2 | Timeout in seconds |
| `-retries number` | 2 | Number of retries |

**Note:** The `-server` option is not supported on Windows (uses system DNS).

---

### tnm::icmp - ICMP Messages

Send ICMP packets for network diagnostics.

**Status:** ✅ 100% functional (mask/timestamp removed on Windows)

#### Commands

**`tnm::icmp [options] echo hosts`**

Test host reachability (ping).

```tcl
set result [tnm::icmp echo {192.168.1.1 192.168.1.2}]
# Returns: {{192.168.1.1 5} {192.168.1.2 3}}
# Format: {{host1 rtt1} {host2 rtt2} ...} (rtt in milliseconds)
```

**`tnm::icmp [options] mask hosts`** *(Linux only)*

Get network masks from hosts.

```tcl
set masks [tnm::icmp mask 192.168.1.1]
# Returns: {192.168.1.1 255.255.255.0}
```

**`tnm::icmp [options] timestamp hosts`** *(Linux only)*

Retrieve time differences from hosts.

```tcl
set times [tnm::icmp timestamp 192.168.1.1]
```

**`tnm::icmp [options] ttl num hosts`**

Send ICMP echo with specific TTL (hop count).

```tcl
set hop [tnm::icmp ttl 5 192.168.1.1]
```

**`tnm::icmp [options] trace num hosts`**

Trace route to hosts (Van Jacobsen algorithm).

```tcl
set route [tnm::icmp trace 30 192.168.1.1]
```

#### ICMP Options

| Option | Default | Description |
|--------|---------|-------------|
| `-timeout time` | 5000 | Timeout in milliseconds |
| `-retries number` | 2 | Number of retries |
| `-delay time` | 0 | Delay between packets (ms, max 255) |
| `-size number` | 64 | Packet size in bytes (64-65535) |
| `-window size` | 10 | Max concurrent requests |

**Notes:**
- **Breaking Change:** `-timeout` and `-delay` are now in **milliseconds** on both platforms
- On Windows: `mask` and `timestamp` commands are not available (ICMP.DLL limitation)
- On Linux: Requires nmicmpd(8) setuid root daemon for raw socket access

---

### tnm::snmp - SNMP Protocol

Send and receive SNMP messages (SNMPv1, SNMPv2c, SNMPv3).

**Status:** ✅ 100% functional on Windows and Linux

#### Session Types

- **generator** - Initiates get/getnext/getbulk/set requests
- **listener** - Receives traps and informs
- **responder** - Processes incoming SNMP commands (agent mode)
- **notifier** - Sends traps and informs

#### Creating Sessions

```tcl
# Create a generator session (SNMP manager)
set s [tnm::snmp generator -address 192.168.1.1 -read public]

# Create a listener for traps
set l [tnm::snmp listener -port 162]

# Create a notifier to send traps
set n [tnm::snmp notifier -address 192.168.1.100]
```

#### Generator Commands

**`snmp# get vbl [script]`**

Retrieve SNMP variables.

```tcl
# Synchronous get
set result [$s get {sysDescr.0 sysUpTime.0}]

# Asynchronous get
$s get {sysDescr.0} {
    if {"%E" == "noError"} {
        puts "Result: %V"
    }
}
```

**`snmp# getnext vbl [script]`**

Get lexicographical successor.

```tcl
set next [$s getnext {sysDescr}]
```

**`snmp# getbulk nr mr vbl [script]`**

Bulk retrieval for efficient table walks (SNMPv2c/v3 only).

```tcl
# nr = non-repeaters, mr = max-repetitions
set bulk [$s getbulk 0 10 {ifDescr ifType}]
```

**`snmp# set vbl [script]`**

Modify SNMP variables.

```tcl
set result [$s set {{sysContact.0 "admin@example.com"}}]
```

**`snmp# walk varname vbl body`**

Walk a MIB subtree.

```tcl
$s walk x "ifDescr ifType" {
    puts "[lindex $x 0]: [lindex $x 2]"
}
```

#### Session Management

```tcl
$s configure -timeout 5000      ;# Configure session
$s cget -version                ;# Get option value
$s destroy                      ;# Destroy session
```

#### Utility Commands

```tcl
tnm::snmp alias name options    ;# Create configuration alias
tnm::snmp find ?-address? ?-tags? ;# Find sessions
tnm::snmp info subject          ;# Get SNMP info
tnm::snmp wait                  ;# Wait for async operations
```

#### SNMP Session Options

| Option | Default | Description |
|--------|---------|-------------|
| `-address addr` | 127.0.0.1 | Peer IP address |
| `-port port` | 161/162 | UDP port |
| `-version ver` | SNMPv1 | SNMPv1, SNMPv2c, SNMPv3 |
| `-read community` | public | Read community string |
| `-write community` | - | Write community string |
| `-user name` | - | SNMPv3 username |
| `-context name` | - | SNMPv3 context |
| `-timeout ms` | 5000 | Response timeout in milliseconds |
| `-retries num` | 3 | Retry count |
| `-window size` | - | Max async requests |
| `-tags tagList` | - | Session tags |

#### Callback Script % Escapes

| Escape | Description |
|--------|-------------|
| `%%` | Single percent |
| `%V` | Varbind list |
| `%R` | Request ID |
| `%S` | Session name |
| `%E` | Error status |
| `%I` | Error index |
| `%A` | Peer IP address |
| `%P` | Peer port |
| `%T` | PDU type |
| `%C` | Context/community |

---

### tnm::mib - MIB Management

Load and query SNMP MIB definitions.

**Status:** ✅ 100% functional (33 subcommands)

#### Loading MIBs

```tcl
# Load a MIB file
tnm::mib load IF-MIB.txt

# Check loaded MIBs
tnm::mib info loaded
```

#### OID/Name Conversion

```tcl
# Name to OID
set oid [tnm::mib oid sysDescr.0]
# Returns: 1.3.6.1.2.1.1.1.0

# OID to name
set name [tnm::mib name 1.3.6.1.2.1.1.1.0]
# Returns: SNMPv2-MIB!sysDescr.0

# Get label only
set label [tnm::mib label 1.3.6.1.2.1.1.1.0]
# Returns: sysDescr.0
```

#### MIB Queries

```tcl
tnm::mib syntax sysDescr        ;# Get ASN.1 syntax (OCTET STRING)
tnm::mib type sysDescr          ;# Get derived type
tnm::mib access sysDescr        ;# Get access level (read-only)
tnm::mib status sysDescr        ;# Get status (current/deprecated/obsolete)
tnm::mib description sysDescr   ;# Get description text
tnm::mib module sysDescr        ;# Get module name (SNMPv2-MIB)
tnm::mib children system        ;# Get child nodes
tnm::mib parent sysDescr        ;# Get parent node
tnm::mib exists sysDescr        ;# Check existence (1 or 0)
tnm::mib file SNMPv2-MIB        ;# Get MIB file path
```

#### Value Operations

```tcl
tnm::mib format sysUpTime 12345678  ;# Format value for display
tnm::mib scan sysUpTime "1:10:17:36.78"  ;# Parse formatted value
tnm::mib enums ifAdminStatus    ;# Get enumeration values
tnm::mib member ifAdminStatus 1 ;# Get enum member name
tnm::mib range ifMtu            ;# Get value range
tnm::mib size sysDescr          ;# Get object size constraints
tnm::mib defval sysContact      ;# Get default value
tnm::mib displayhint ifPhysAddress  ;# Get display hint
```

#### OID Operations

```tcl
tnm::mib compare oid1 oid2      ;# Compare OIDs (-1/0/1)
tnm::mib subtree oid1 oid2      ;# Check subtree containment
tnm::mib length 1.3.6.1.2.1.1.1 ;# Get OID length
tnm::mib split sysDescr.0       ;# Split to {base instance}
```

#### Table Operations

```tcl
tnm::mib index ifEntry          ;# Get index columns
tnm::mib variables ifEntry      ;# Get table columns
tnm::mib pack ifEntry 1         ;# Build table instance OID
tnm::mib unpack ifDescr.1       ;# Extract index values
```

#### Walking MIB Tree

```tcl
tnm::mib walk oidVar labelVar system {
    puts "$labelVar ($oidVar)"
}
```

#### All MIB Subcommands

`access`, `children`, `compare`, `defval`, `description`, `displayhint`, `enums`, `exists`, `file`, `format`, `index`, `info`, `label`, `length`, `load`, `macro`, `member`, `module`, `name`, `oid`, `pack`, `parent`, `range`, `scan`, `size`, `split`, `status`, `subtree`, `syntax`, `type`, `unpack`, `variables`, `walk`

---

### tnm::udp - UDP Datagrams

Send and receive UDP datagrams.

**Status:** ✅ 100% functional

#### Creating Endpoints

```tcl
# Create UDP endpoint
set u [tnm::udp create -myport 5000]

# Find existing endpoints
set endpoints [tnm::udp find -tags "mytag"]
```

#### Sending Data

```tcl
# Send to specific host:port
$u send 192.168.1.1 5001 "Hello, World!"

# Connect and send
$u connect 192.168.1.1 5001
$u send "Hello, World!"
```

#### Receiving Data

```tcl
# Blocking receive
set data [$u receive]
# Returns: {address port message}

# Event-driven receive
$u configure -read {
    set data [%S receive]
    puts "Received from [lindex $data 0]: [lindex $data 2]"
}
```

#### UDP Options

| Option | Description |
|--------|-------------|
| `-address addr` | Remote IP address |
| `-port port` | Remote port |
| `-myaddress addr` | Local IP address |
| `-myport port` | Local port |
| `-read command` | Read callback |
| `-write command` | Write callback |
| `-tags tagList` | Endpoint tags |

---

### tnm::job - Job Scheduler

Execute Tcl procedures at regular intervals.

**Status:** ✅ 91% functional (sub-interpreter limitations)

#### Creating Jobs

```tcl
set j [tnm::job create \
    -command {puts "tick"} \
    -interval 1000 \
    -iterations 10]
```

#### Job Commands

```tcl
tnm::job current                ;# Get current running job
tnm::job find -status running   ;# Find jobs by status/tags
tnm::job schedule 1000 {puts "once"}  ;# One-time scheduled execution
tnm::job wait                   ;# Wait for all jobs to complete
```

#### Job Instance Commands

```tcl
$j configure -interval 2000     ;# Configure job
$j cget -interval               ;# Get option value
$j destroy                      ;# Destroy job
```

#### Job Options

| Option | Description |
|--------|-------------|
| `-command cmd` | Tcl command to execute |
| `-error cmd` | Error handling command |
| `-exit cmd` | Cleanup command |
| `-interval ms` | Interval in milliseconds |
| `-iterations n` | Number of activations (0 = unlimited) |
| `-status state` | waiting, suspended, running, expired |
| `-tags tagList` | Job tags |

---

### tnm::netdb - Network Database

Access local network information.

**Status:** ✅ 100% functional (sunrpcs removed)

#### Host Lookups

```tcl
tnm::netdb hosts address localhost  ;# Name to address
tnm::netdb hosts name 127.0.0.1     ;# Address to name
tnm::netdb hosts aliases localhost  ;# Get aliases
```

#### Service Information

```tcl
tnm::netdb services name 80 tcp     ;# Port to name (http)
tnm::netdb services number http tcp ;# Name to port (80)
tnm::netdb services aliases http tcp
```

#### Protocol Information

```tcl
tnm::netdb protocols name 6         ;# Number to name (tcp)
tnm::netdb protocols number tcp     ;# Name to number (6)
tnm::netdb protocols aliases tcp
```

#### Network Information

```tcl
tnm::netdb networks name 10.0.0.0   ;# Address to name
tnm::netdb networks address loopback ;# Name to address
tnm::netdb networks aliases loopback
```

#### IP Address Operations

```tcl
tnm::netdb ip class 192.168.1.1     ;# Get IP class (C)
tnm::netdb ip apply 192.168.1.100 255.255.255.0  ;# Apply netmask
tnm::netdb ip broadcast 192.168.1.0 255.255.255.0  ;# Broadcast address
tnm::netdb ip compare mask1 mask2   ;# Compare masks (-1/0/1)
tnm::netdb ip range 192.168.1.0 255.255.255.248  ;# Get IP range
```

---

### tnm::ntp - NTP Status

Retrieve NTP (Network Time Protocol) time and status.

**Status:** ✅ 100% functional

#### Commands

**`tnm::ntp [options] time server`**

Get time from NTP server using client mode (mode 3). Works with all public NTP servers.

```tcl
set r [tnm::ntp time pool.ntp.org]
puts "Server time: [clock format [dict get $r time]]"
puts "Stratum: [dict get $r stratum]"
puts "Offset: [dict get $r offset] seconds"
```

**Returns dict with:** `time` (Unix timestamp), `offset` (seconds), `delay` (seconds), `stratum`, `precision`, `refid`

**`tnm::ntp [options] status server`**

Query NTP server status using control mode (mode 6). Only works with servers that allow control queries.

```tcl
set status [tnm::ntp status ntpserver]
```

**Returns dict with:** `sys.*` and `peer.*` variables

#### NTP Options

| Option | Default | Description |
|--------|---------|-------------|
| `-timeout time` | 2 | Timeout in seconds |
| `-retries number` | 2 | Number of retries |

**Note:** Most public NTP servers disable control mode (mode 6) for security. Use `time` subcommand for public servers.

---

### tnm::syslog - System Logging

Write messages to the system logging subsystem.

**Status:** ✅ 100% functional

#### Commands

```tcl
tnm::syslog emergency "System panic"
tnm::syslog alert "Immediate action required"
tnm::syslog critical "Critical condition"
tnm::syslog error "Error occurred"
tnm::syslog warning "Warning message"
tnm::syslog notice "Normal but significant"
tnm::syslog info "Informational"
tnm::syslog debug "Debug message"
```

#### Syslog Options

| Option | Default | Description |
|--------|---------|-------------|
| `-ident string` | scotty | Program identification |
| `-facility facility` | local0 | Message facility |

```tcl
# Configure defaults
tnm::syslog -ident myapp -facility daemon
tnm::syslog info "Application started"
```

**Notes:** Windows uses internal logging, Linux uses native syslog.

---

### tnm::map - Network Maps

Create and manipulate network topology maps.

**Status:** ✅ 100% functional

#### Creating Maps

```tcl
set m [tnm::map create -name "My Network"]
```

#### Map Commands

```tcl
tnm::map find *                 ;# Find maps matching pattern
tnm::map info class             ;# Get map information
```

#### Map Instance Commands

```tcl
$m configure -name "Network1"   ;# Configure attributes
$m cget -name                   ;# Get attribute
$m destroy                      ;# Destroy map
```

#### Map Options

| Option | Description |
|--------|-------------|
| `-name string` | Map name |
| `-tags list` | Map tags |
| `-command script` | Command callback |

---

## Utility Packages

### tnmInet - TCP/IP Services

```tcl
package require tnmInet $tnm(version)

tnmInet::getIpAddress host      ;# Get IP address
tnmInet::getIpName host         ;# Get hostname
tnmInet::dayTime host           ;# Get remote time
tnmInet::finger host ?user?     ;# Finger protocol
tnmInet::traceRoute host        ;# Trace route
tnmInet::tcpServices ?host?     ;# Probe TCP services
tnmInet::sendMail recipients message ?subject?
```

### tnmSnmp - SNMP Utilities

```tcl
package require tnmSnmp $tnm(version)

tnmSnmp::Walk session subtree
tnmSnmp::Scalars session scalars varName
tnmSnmp::ShowScalars session scalars
tnmSnmp::Table session table varName
tnmSnmp::ShowTable session table
```

### tnmDialog - Tk Dialogs

```tcl
package require tnmDialog $tnm(version)

tnmDialog::confirm path bitmap text buttons
tnmDialog::browse path title text buttons
```

### tnmMap - Map Utilities

```tcl
package require tnmMap $tnm(version)

tnmMap::getIpAddress node
tnmMap::getIpName node
tnmMap::getSnmpSession node
tnmMap::traceRoute node ?maxlength? ?retries?
```

### tnmMib - MIB Utilities

```tcl
package require tnmMib $tnm(version)

tnmMib::describeNode node
tnmMib::describeType type
```

### tnmEther - Ethernet Utilities

```tcl
package require tnmEther $tnm(version)

tnmEther::getVendor etherAddr   ;# Get vendor from MAC address
tnmEther::getEthers pattern     ;# Get ethernet prefixes for vendor
```

### tnmTerm - Output Terminals

```tcl
package require tnmTerm $tnm(version)

tnmTerm::open path              ;# Create terminal window
tnmTerm::write path text        ;# Write to terminal
tnmTerm::clear path             ;# Clear terminal
tnmTerm::close path             ;# Close terminal
```

---

## Environment Variables

| Variable | Description |
|----------|-------------|
| `TNM_LIBRARY` | Path to tnm library directory |
| `TNM_NMICMPD` | Path to nmicmpd executable (Linux) |
| `TNM_NMTRAPD` | Path to nmtrapd executable |
| `TNM_RCFILE` | Path to initialization file |

---

## Tcl Variables

| Variable | Description |
|----------|-------------|
| `tnm(library)` | Path to tnm library directory |
| `tnm(version)` | TNM version number |
| `tnm(arch)` | Architecture/OS identifier |
| `tnm(host)` | Local hostname |
| `tnm(domain)` | DNS domain name |
| `tnm(user)` | Current username |
| `tnm(mibs)` | List of MIB files to auto-load |
| `tnm(mibs:core)` | Core MIB files (always loaded) |
| `tnm(tmp)` | Temporary directory |
| `tnm(cache)` | Cache directory |
| `tnm(start)` | Start time (seconds since epoch) |

---

## Examples

### Example 1: SNMP Device Query

```tcl
package require tnm 3.1.3

# Create SNMP session
set s [tnm::snmp generator -address 192.168.1.1 -read public]

# Get system information
set vbl [$s get {sysDescr.0 sysName.0 sysUpTime.0 sysContact.0}]

# Display results
foreach vb $vbl {
    puts "[lindex $vb 0]: [lindex $vb 2]"
}

# Cleanup
$s destroy
```

### Example 2: ICMP Network Sweep

```tcl
package require tnm 3.1.3

# Ping multiple hosts
set hosts {192.168.1.1 192.168.1.2 192.168.1.3}
set results [tnm::icmp echo $hosts]

foreach result $results {
    set host [lindex $result 0]
    set rtt [lindex $result 1]
    if {$rtt eq ""} {
        puts "$host: unreachable"
    } else {
        puts "$host: ${rtt}ms"
    }
}
```

### Example 3: Periodic Monitoring Job

```tcl
package require tnm 3.1.3

# Monitor every 10 seconds
set job [tnm::job create -command {
    puts "Checking status: [clock format [clock seconds]]"
    # Add monitoring logic here
} -interval 10000]

# Run for 60 seconds then stop
after 60000 [list $job destroy]
vwait forever
```

### Example 4: MIB Tree Walk

```tcl
package require tnm 3.1.3

# Walk system subtree
tnm::mib walk oidVar labelVar system {
    puts "$labelVar ($oidVar)"
}
```

### Example 5: NTP Time Query

```tcl
package require tnm 3.1.3

# Get time from NTP server
set r [tnm::ntp time pool.ntp.org]
puts "Server time: [clock format [dict get $r time]]"
puts "Stratum: [dict get $r stratum]"
puts "Offset: [dict get $r offset] seconds"
```

---

## Platform-Specific Notes

### Windows (MinGW64)

**Fully Working (10 commands):**
- tnm::syslog, tnm::map, tnm::udp, tnm::job, tnm::snmp, tnm::mib, tnm::icmp, tnm::dns, tnm::ntp, tnm::netdb

**Limitations:**
- tnm::icmp: `mask` and `timestamp` commands not available (ICMP.DLL limitation)
- tnm::dns: `-server` option not supported (uses system DNS)
- tnm::netdb: `networks` queries limited

### Linux

**All commands fully functional.**

**Requirements:**
- nmicmpd daemon with setuid root for ICMP operations

### Removed Commands

The following commands have been removed from all platforms:
- **tnm::smx** - Unused Script MIB Executive feature
- **tnm::ined** - Tkined GUI integration not needed
- **tnm::netdb sunrpcs** - RPC support disabled

---

## See Also

- [SNMP RFCs](https://www.rfc-editor.org/): RFC 1157, RFC 3411-3418
- [ICMP RFC](https://www.rfc-editor.org/rfc/rfc792): RFC 792
- [DNS RFCs](https://www.rfc-editor.org/): RFC 1034, RFC 1035
- [NTP RFC](https://www.rfc-editor.org/rfc/rfc5905): RFC 5905
- [FlightAware Scotty](https://github.com/flightaware/scotty)
- TNMOVERVIEW.md for detailed platform status

---

## License

This software is copyrighted by Juergen Schoenwaelder, the Technical University of Braunschweig, the University of Twente and other parties.

Permission is granted to use, copy, modify, distribute, and license this software and its documentation for any purpose, provided that existing copyright notices are retained in all copies and that this notice is included verbatim in any distributions.

**Disclaimer:** This software is provided "as is" without warranty of any kind.
