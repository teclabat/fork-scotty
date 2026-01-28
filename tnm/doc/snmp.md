# tnm::snmp - SNMP Protocol Commands

Send and receive SNMP messages using SNMPv1, SNMPv2c, and SNMPv3.

**Status:** ✅ 100% functional on Windows and Linux

---

## Synopsis

```tcl
tnm::snmp generator [options]
tnm::snmp listener [options]
tnm::snmp responder [options]
tnm::snmp notifier [options]
tnm::snmp alias name [options]
tnm::snmp find [options]
tnm::snmp info subject
tnm::snmp wait
```

---

## Description

The `tnm::snmp` command provides a complete implementation of the Simple Network Management Protocol (SNMP) including SNMPv1, SNMPv2c, and SNMPv3. It supports all standard SNMP operations: GET, GETNEXT, GETBULK, SET, as well as trap/inform notifications.

---

## Session Types

### tnm::snmp generator

Create an SNMP manager session for sending GET/SET requests.

```tcl
set s [tnm::snmp generator -address 192.168.1.1 -read public]
```

### tnm::snmp listener

Create a session for receiving SNMP traps and informs.

```tcl
set l [tnm::snmp listener -port 162]
```

### tnm::snmp responder

Create an SNMP agent session for responding to requests.

```tcl
set r [tnm::snmp responder -read public -write private]
```

### tnm::snmp notifier

Create a session for sending traps and informs.

```tcl
set n [tnm::snmp notifier -address 192.168.1.100]
```

---

## Session Options

| Option | Default | Description |
|--------|---------|-------------|
| `-address addr` | 127.0.0.1 | Peer IP address |
| `-port port` | 161/162 | UDP port (161 for requests, 162 for traps) |
| `-version ver` | SNMPv1 | Protocol version: SNMPv1, SNMPv2c, SNMPv3 |
| `-read community` | public | Read community string (v1/v2c) |
| `-write community` | - | Write community string (v1/v2c) |
| `-user name` | - | SNMPv3 username |
| `-context name` | - | SNMPv3 context name |
| `-timeout ms` | 5000 | Response timeout in milliseconds |
| `-retries num` | 3 | Number of retries |
| `-window size` | - | Max concurrent async requests |
| `-tags tagList` | - | Session tags for grouping |

---

## Generator Commands

### $session get vbl [script]

Retrieve SNMP variable values.

**Synchronous:**
```tcl
set result [$s get {sysDescr.0 sysUpTime.0}]
# Returns: {{sysDescr.0 OCTET STRING "Linux router..."} {sysUpTime.0 TimeTicks 123456}}
```

**Asynchronous:**
```tcl
$s get {sysDescr.0} {
    if {"%E" == "noError"} {
        puts "Result: %V"
    } else {
        puts "Error: %E at index %I"
    }
}
tnm::snmp wait
```

### $session getnext vbl [script]

Get the lexicographically next OID.

```tcl
set next [$s getnext {system}]
# Returns next OID after 'system' subtree
```

### $session getbulk nr mr vbl [script]

Efficient bulk retrieval (SNMPv2c/v3 only).

**Parameters:**
- `nr` - Number of non-repeaters
- `mr` - Max repetitions

```tcl
# Get 10 rows of interface table
set bulk [$s getbulk 0 10 {ifDescr ifType ifOperStatus}]
```

### $session set vbl [script]

Modify SNMP variable values.

```tcl
# Set single value
$s set {{sysContact.0 "admin@example.com"}}

# Set multiple values
$s set {
    {sysContact.0 "admin@example.com"}
    {sysLocation.0 "Server Room A"}
}
```

### $session walk varname vbl body

Walk a MIB subtree synchronously.

```tcl
$s walk row "ifDescr ifType ifOperStatus" {
    set ifDescr [lindex $row 0 2]
    set ifType [lindex $row 1 2]
    set ifStatus [lindex $row 2 2]
    puts "$ifDescr: $ifType ($ifStatus)"
}
```

### $session configure [options]

Configure session options.

```tcl
$s configure -timeout 10000 -retries 5
```

### $session cget option

Get session option value.

```tcl
set timeout [$s cget -timeout]
set version [$s cget -version]
```

### $session destroy

Destroy the session and free resources.

```tcl
$s destroy
```

---

## Listener Commands

### $listener bind event script

Bind a script to trap/inform events.

```tcl
$l bind trap {
    puts "Trap received from %A:%P"
    puts "Varbinds: %V"
}

$l bind inform {
    puts "Inform received from %A"
    puts "Varbinds: %V"
}
```

---

## Notifier Commands

### $notifier trap oid vbl

Send an SNMPv1/v2c trap.

```tcl
$n trap coldStart {}
$n trap linkDown {{ifIndex.1 INTEGER 1}}
```

### $notifier inform oid vbl [script]

Send an SNMPv2c/v3 inform (acknowledged notification).

```tcl
$n inform linkDown {{ifIndex.1 INTEGER 1}} {
    if {"%E" == "noError"} {
        puts "Inform acknowledged"
    }
}
```

---

## Utility Commands

### tnm::snmp alias name [options]

Create a named configuration alias for reuse.

```tcl
tnm::snmp alias router1 -address 192.168.1.1 -read public -version SNMPv2c
set s [tnm::snmp generator -alias router1]
```

### tnm::snmp find [options]

Find existing SNMP sessions.

```tcl
set sessions [tnm::snmp find]
set sessions [tnm::snmp find -address 192.168.1.*]
set sessions [tnm::snmp find -tags "router"]
```

### tnm::snmp info subject

Get SNMP subsystem information.

```tcl
tnm::snmp info version    ;# Supported versions
tnm::snmp info pdu        ;# PDU types
```

### tnm::snmp wait

Wait for all asynchronous operations to complete.

```tcl
# Start async operations
$s1 get {sysDescr.0} {puts "S1: %V"}
$s2 get {sysDescr.0} {puts "S2: %V"}

# Wait for all to complete
tnm::snmp wait
```

---

## Callback Script Escapes

When using asynchronous operations, the callback script supports these % escapes:

| Escape | Description |
|--------|-------------|
| `%%` | Literal percent sign |
| `%V` | Varbind list |
| `%R` | Request ID |
| `%S` | Session name |
| `%E` | Error status (noError, tooBig, noSuchName, etc.) |
| `%I` | Error index |
| `%A` | Peer IP address |
| `%P` | Peer port number |
| `%T` | PDU type |
| `%C` | Context (v3) or community string |

---

## Varbind Format

Varbinds are represented as lists: `{oid type value}`

**Example:**
```tcl
{sysDescr.0 "OCTET STRING" "Linux router 2.6.32"}
{sysUpTime.0 TimeTicks 12345678}
{ifIndex.1 INTEGER 1}
```

**SNMP Data Types:**
- `INTEGER` / `Integer32`
- `OCTET STRING`
- `OBJECT IDENTIFIER`
- `IpAddress`
- `Counter32` / `Counter64`
- `Gauge32` / `Unsigned32`
- `TimeTicks`
- `Opaque`
- `NULL`

---

## Examples

### Basic System Query

```tcl
package require tnm 3.1.3

set s [tnm::snmp generator -address 192.168.1.1 -read public]

set result [$s get {sysDescr.0 sysName.0 sysUpTime.0 sysContact.0}]

foreach vb $result {
    lassign $vb oid type value
    puts "[tnm::mib label $oid]: $value"
}

$s destroy
```

### Interface Table Walk

```tcl
package require tnm 3.1.3

set s [tnm::snmp generator -address 192.168.1.1 -read public -version SNMPv2c]

puts "Interface Table:"
puts [format "%-5s %-20s %-10s %-10s" "Idx" "Description" "Type" "Status"]
puts [string repeat "-" 50]

$s walk row "ifIndex ifDescr ifType ifOperStatus" {
    set idx [lindex $row 0 2]
    set descr [lindex $row 1 2]
    set type [lindex $row 2 2]
    set status [lindex $row 3 2]
    puts [format "%-5s %-20s %-10s %-10s" $idx $descr $type $status]
}

$s destroy
```

### SNMPv3 Session

```tcl
package require tnm 3.1.3

set s [tnm::snmp generator \
    -address 192.168.1.1 \
    -version SNMPv3 \
    -user admin \
    -context ""]

set result [$s get {sysDescr.0}]
puts "Description: [lindex $result 0 2]"

$s destroy
```

### Trap Receiver

```tcl
package require tnm 3.1.3

set l [tnm::snmp listener -port 162]

$l bind trap {
    puts "=== TRAP RECEIVED ==="
    puts "From: %A:%P"
    puts "Time: [clock format [clock seconds]]"
    puts "Varbinds:"
    foreach vb {%V} {
        lassign $vb oid type value
        puts "  $oid = $value"
    }
    puts ""
}

puts "Listening for SNMP traps on port 162..."
vwait forever
```

### Async Polling Multiple Devices

```tcl
package require tnm 3.1.3

set devices {192.168.1.1 192.168.1.2 192.168.1.3}

foreach ip $devices {
    set s($ip) [tnm::snmp generator -address $ip -read public]
    $s($ip) get {sysDescr.0 sysUpTime.0} [list handleResponse $ip]
}

proc handleResponse {ip} {
    if {"%E" == "noError"} {
        puts "$ip: [lindex {%V} 0 2]"
    } else {
        puts "$ip: Error - %E"
    }
}

tnm::snmp wait

foreach ip $devices {
    $s($ip) destroy
}
```

---

## Error Handling

```tcl
if {[catch {$s get {sysDescr.0}} result]} {
    puts "SNMP error: $result"
} else {
    puts "Success: $result"
}
```

Common SNMP errors:
- `noError` - Success
- `tooBig` - Response too large
- `noSuchName` - OID not found (v1)
- `noSuchObject` - Object not found (v2c/v3)
- `noSuchInstance` - Instance not found (v2c/v3)
- `genErr` - General error
- `noAccess` - Access denied
- `wrongType` - Wrong value type
- `wrongValue` - Wrong value
- `noCreation` - Cannot create
- `inconsistentValue` - Inconsistent value
- `resourceUnavailable` - Resource unavailable
- `commitFailed` - Commit failed
- `undoFailed` - Undo failed

---

## See Also

- [tnm.md](tnm.md) - Main TNM documentation
- [tnm::mib](mib.md) - MIB operations
- RFC 1157 - SNMPv1
- RFC 3411-3418 - SNMPv3
