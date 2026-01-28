# tnm::mib - MIB Management Commands

Load and query SNMP MIB (Management Information Base) definitions.

**Status:** ✅ 100% functional (33 subcommands)

---

## Synopsis

```tcl
tnm::mib load file
tnm::mib oid name
tnm::mib name oid
tnm::mib label oid
tnm::mib syntax node
tnm::mib type node
tnm::mib access node
tnm::mib status node
tnm::mib description node
tnm::mib children node
tnm::mib parent node
tnm::mib module node
tnm::mib exists nodeOrType
tnm::mib file module
tnm::mib info subject
tnm::mib format node value
tnm::mib scan node value
tnm::mib enums node
tnm::mib member node value
tnm::mib range node
tnm::mib size node
tnm::mib defval node
tnm::mib displayhint node
tnm::mib compare oid1 oid2
tnm::mib subtree oid1 oid2
tnm::mib length oid
tnm::mib split oid
tnm::mib index entry
tnm::mib variables entry
tnm::mib pack entry values
tnm::mib unpack oid
tnm::mib macro name
tnm::mib walk oidVar labelVar node body
```

---

## Description

The `tnm::mib` command provides comprehensive access to SNMP MIB definitions. It can load MIB files, convert between OID formats, query object properties, and navigate the MIB tree structure.

---

## Loading MIBs

### tnm::mib load file

Load a MIB definition file.

```tcl
tnm::mib load IF-MIB.txt
tnm::mib load /usr/share/snmp/mibs/SNMPv2-MIB.txt
```

**Note:** Core MIBs (SNMPv2-SMI, SNMPv2-TC, SNMPv2-MIB, IF-MIB) are typically loaded automatically.

### tnm::mib info loaded

List currently loaded MIB modules.

```tcl
set modules [tnm::mib info loaded]
puts $modules
# Returns: {SNMPv2-SMI SNMPv2-TC SNMPv2-MIB IF-MIB ...}
```

### tnm::mib file module

Get the file path for a loaded MIB module.

```tcl
set path [tnm::mib file IF-MIB]
```

---

## OID/Name Conversion

### tnm::mib oid name

Convert a symbolic name to numeric OID.

```tcl
set oid [tnm::mib oid sysDescr.0]
# Returns: 1.3.6.1.2.1.1.1.0

set oid [tnm::mib oid ifDescr]
# Returns: 1.3.6.1.2.1.2.2.1.2
```

### tnm::mib name oid

Convert a numeric OID to fully qualified name.

```tcl
set name [tnm::mib name 1.3.6.1.2.1.1.1.0]
# Returns: SNMPv2-MIB!sysDescr.0
```

### tnm::mib label oid

Get just the label (without module prefix) for an OID.

```tcl
set label [tnm::mib label 1.3.6.1.2.1.1.1.0]
# Returns: sysDescr.0
```

---

## Object Properties

### tnm::mib syntax node

Get the ASN.1 base syntax of an object.

```tcl
tnm::mib syntax sysDescr
# Returns: OCTET STRING

tnm::mib syntax sysUpTime
# Returns: TimeTicks

tnm::mib syntax ifOperStatus
# Returns: INTEGER
```

### tnm::mib type node

Get the derived (textual convention) type.

```tcl
tnm::mib type sysDescr
# Returns: DisplayString

tnm::mib type ifPhysAddress
# Returns: PhysAddress
```

### tnm::mib access node

Get the maximum access level.

```tcl
tnm::mib access sysDescr
# Returns: read-only

tnm::mib access sysContact
# Returns: read-write

tnm::mib access ifTable
# Returns: not-accessible
```

### tnm::mib status node

Get the status of an object.

```tcl
tnm::mib status sysDescr
# Returns: current

tnm::mib status atTable
# Returns: deprecated
```

Possible values: `current`, `deprecated`, `obsolete`

### tnm::mib description node

Get the DESCRIPTION clause text.

```tcl
set desc [tnm::mib description sysDescr]
puts $desc
```

### tnm::mib module node

Get the module name where the object is defined.

```tcl
tnm::mib module sysDescr
# Returns: SNMPv2-MIB

tnm::mib module ifDescr
# Returns: IF-MIB
```

### tnm::mib exists nodeOrType

Check if an object or type exists in loaded MIBs.

```tcl
tnm::mib exists sysDescr
# Returns: 1

tnm::mib exists nonExistent
# Returns: 0
```

---

## Tree Navigation

### tnm::mib children node

Get child nodes of an object.

```tcl
set children [tnm::mib children system]
# Returns: {sysDescr sysObjectID sysUpTime sysContact ...}

set children [tnm::mib children mib-2]
# Returns: {system interfaces at ip icmp tcp udp ...}
```

### tnm::mib parent node

Get the parent node.

```tcl
tnm::mib parent sysDescr
# Returns: system

tnm::mib parent system
# Returns: mib-2
```

### tnm::mib walk oidVar labelVar node body

Walk the MIB tree executing a script for each node.

```tcl
tnm::mib walk oid label system {
    puts "$label ($oid)"
}
```

---

## Value Operations

### tnm::mib format node value

Format a primitive value for human-readable display.

```tcl
tnm::mib format sysUpTime 12345678
# Returns: 1 day, 10:17:36.78

tnm::mib format ifOperStatus 1
# Returns: up
```

### tnm::mib scan node value

Parse a formatted value back to primitive form.

```tcl
tnm::mib scan sysUpTime "1 day, 10:17:36.78"
# Returns: 12345678

tnm::mib scan ifOperStatus up
# Returns: 1
```

### tnm::mib enums node

Get enumeration values for an INTEGER object.

```tcl
tnm::mib enums ifOperStatus
# Returns: {up 1 down 2 testing 3 unknown 4 dormant 5 notPresent 6 lowerLayerDown 7}
```

### tnm::mib member node value

Get the enumeration label for a numeric value.

```tcl
tnm::mib member ifOperStatus 1
# Returns: up

tnm::mib member ifOperStatus 2
# Returns: down
```

### tnm::mib range node

Get the valid value range for a numeric object.

```tcl
tnm::mib range ifMtu
# Returns: {68 65535}
```

### tnm::mib size node

Get size constraints for an OCTET STRING object.

```tcl
tnm::mib size sysDescr
# Returns: {0 255}
```

### tnm::mib defval node

Get the DEFVAL (default value) for an object.

```tcl
tnm::mib defval someObject
```

### tnm::mib displayhint node

Get the DISPLAY-HINT for a textual convention.

```tcl
tnm::mib displayhint ifPhysAddress
# Returns: 1x:
```

---

## OID Operations

### tnm::mib compare oid1 oid2

Compare two OIDs lexicographically.

```tcl
tnm::mib compare 1.3.6.1.2.1 1.3.6.1.4.1
# Returns: -1 (first is less)

tnm::mib compare 1.3.6.1.2.1 1.3.6.1.2.1
# Returns: 0 (equal)

tnm::mib compare 1.3.6.1.4.1 1.3.6.1.2.1
# Returns: 1 (first is greater)
```

### tnm::mib subtree oid1 oid2

Check if oid2 is within the subtree rooted at oid1.

```tcl
tnm::mib subtree system sysDescr
# Returns: 1

tnm::mib subtree system ifDescr
# Returns: 0
```

### tnm::mib length oid

Get the number of components in an OID.

```tcl
tnm::mib length 1.3.6.1.2.1.1.1.0
# Returns: 9
```

### tnm::mib split oid

Split an OID into base object and instance parts.

```tcl
tnm::mib split sysDescr.0
# Returns: {1.3.6.1.2.1.1.1 0}

tnm::mib split ifDescr.5
# Returns: {1.3.6.1.2.1.2.2.1.2 5}
```

---

## Table Operations

### tnm::mib index entry

Get the INDEX clause columns for a table entry.

```tcl
tnm::mib index ifEntry
# Returns: {ifIndex}

tnm::mib index tcpConnEntry
# Returns: {tcpConnLocalAddress tcpConnLocalPort tcpConnRemAddress tcpConnRemPort}
```

### tnm::mib variables entry

Get all columnar objects in a table entry.

```tcl
tnm::mib variables ifEntry
# Returns: {ifIndex ifDescr ifType ifMtu ifSpeed ...}
```

### tnm::mib pack entry values

Build an instance OID by combining entry and index values.

```tcl
tnm::mib pack ifEntry 5
# Returns: 1.3.6.1.2.1.2.2.1.5
```

### tnm::mib unpack oid

Extract index values from an instance OID.

```tcl
tnm::mib unpack ifDescr.5
# Returns: {5}

tnm::mib unpack tcpConnState.192.168.1.1.80.10.0.0.1.12345
# Returns: {192.168.1.1 80 10.0.0.1 12345}
```

---

## Other Commands

### tnm::mib macro name

Get information about SMI macros.

```tcl
tnm::mib macro MODULE-IDENTITY
tnm::mib macro OBJECT-TYPE
```

### tnm::mib info subject

Get various MIB subsystem information.

| Subject | Description |
|---------|-------------|
| `loaded` | List of loaded MIB modules |
| `files` | List of loaded MIB files |
| `path` | MIB search path |

---

## Examples

### MIB Browser

```tcl
package require tnm 3.1.3

proc browseNode {node {indent 0}} {
    set prefix [string repeat "  " $indent]

    if {[tnm::mib exists $node]} {
        set oid [tnm::mib oid $node]
        set syntax [tnm::mib syntax $node]
        set access [tnm::mib access $node]

        puts "${prefix}$node ($oid)"
        puts "${prefix}  Syntax: $syntax"
        puts "${prefix}  Access: $access"

        foreach child [tnm::mib children $node] {
            browseNode $child [expr {$indent + 1}]
        }
    }
}

browseNode system
```

### Format SNMP Values

```tcl
package require tnm 3.1.3

# Format various value types
puts "Uptime: [tnm::mib format sysUpTime 12345678]"
puts "Status: [tnm::mib format ifOperStatus 1]"
puts "Speed: [tnm::mib format ifSpeed 100000000]"
```

### Explore Table Structure

```tcl
package require tnm 3.1.3

proc describeTable {table} {
    puts "Table: $table"
    puts "Entry: ${table}Entry"

    set entry "${table}Entry"
    puts "\nIndex columns:"
    foreach idx [tnm::mib index $entry] {
        puts "  $idx"
    }

    puts "\nAll columns:"
    foreach col [tnm::mib variables $entry] {
        set syntax [tnm::mib syntax $col]
        set access [tnm::mib access $col]
        puts "  $col ($syntax, $access)"
    }
}

describeTable ifTable
```

---

## See Also

- [tnm.md](tnm.md) - Main TNM documentation
- [tnm::snmp](snmp.md) - SNMP protocol operations
