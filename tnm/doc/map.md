# tnm::map - Network Map Commands

Create and manipulate network topology maps.

**Status:** ✅ 100% functional on Windows and Linux

---

## Synopsis

```tcl
tnm::map create [options]
tnm::map info subject [pattern]

$map configure [options]
$map cget option
$map create type [options]
$map find [options]
$map bind pattern script
$map raise tag [args]
$map message [options] tag text
$map attribute [name [value]]
$map save channel
$map load channel
$map dump
$map copy items
$map paste
$map clear
$map update
$map destroy
```

---

## Description

The `tnm::map` command creates and manipulates network maps. Network maps describe the topology of a network and are composed of items of different types (node, network, port, link, group). Maps support event-driven programming with bindings, messages, and attributes for storing management data.

---

## Map Commands

### tnm::map create [options]

Create a new network map.

```tcl
set m [tnm::map create -name "Corporate Network" -width 800 -height 600]
```

### tnm::map info subject [pattern]

Get information about maps.

```tcl
# List all maps
set maps [tnm::map info maps]

# List item types
set types [tnm::map info types]
```

---

## Map Instance Commands

### $map configure [options]

Query or change map configuration.

```tcl
$m configure -name "New Network" -tick 30
```

### $map cget option

Get a configuration option value.

```tcl
set name [$m cget -name]
set width [$m cget -width]
```

### $map create type [options]

Create a new item on the map.

```tcl
# Create a node
set router [$m create node -name "Router1" -address 192.168.1.1]

# Create a network
set lan [$m create network -name "LAN Segment" -address 192.168.1.0/24]

# Create a port on a node
set port1 [$m create port -name "eth0" -node $router]

# Create a link between port and network
set link [$m create link -src $port1 -dst $lan]

# Create a group
set servers [$m create group -name "Servers"]
```

### $map find [options]

Find items on the map.

```tcl
# Find all items
set items [$m find]

# Find by type
set nodes [$m find -type node]
set networks [$m find -type network]

# Find by name pattern
set routers [$m find -name "Router*"]

# Find by address
set device [$m find -address 192.168.1.1]

# Find by tags
set critical [$m find -tags "critical"]

# Sort by modification time (decreasing)
set recent [$m find -sort mtime -order decreasing]
```

### $map bind pattern script

Bind a script to map events.

```tcl
$m bind "error:*" {
    puts "Error event: %N"
    puts "Arguments: %A"
}

$m bind "status:change" {
    puts "Status changed on item %I"
}
```

### $map raise tag [args]

Raise an event on the map.

```tcl
set evt [$m raise "status:update" "All systems operational"]
```

### $map message [options] tag text

Create a message on the map.

```tcl
# Simple message
$m message "info" "Map initialized"

# Message with interval and health impact
$m message -interval 60 -health -5 "load:high" "CPU load above threshold"
```

### $map attribute [name [value]]

Get or set map attributes.

```tcl
# List all attributes
set attrs [$m attribute]

# Get attribute value
set contact [$m attribute :Contact]

# Set attribute
$m attribute :Contact "admin@example.com"
$m attribute Snmp:Community "public"
```

### $map save channel

Save map to a Tcl channel.

```tcl
set f [open "network.map" w]
$m save $f
close $f
```

### $map load channel

Load (merge) map from a Tcl channel.

```tcl
set f [open "network.map" r]
$m load $f
close $f
```

### $map dump

Return a Tcl script that recreates the map.

```tcl
set script [$m dump]
```

### $map copy items / $map paste

Copy and paste items.

```tcl
$m copy [list $node1 $node2]
$m paste  ;# Creates clones of copied items
```

### $map clear

Remove all items and data from the map.

```tcl
$m clear
```

### $map update

Process pending updates (expire events/messages).

```tcl
$m update
```

### $map destroy

Destroy the map.

```tcl
$m destroy
```

### $map info subject [pattern]

Get map-related handles.

```tcl
# Get all events
set events [$m info events]

# Get messages matching pattern
set loadMsgs [$m info messages "load:*"]

# Get bindings
set bindings [$m info bindings]
```

---

## Map Options

| Option | Default | Description |
|--------|---------|-------------|
| `-name name` | - | Map name |
| `-width pixels` | - | Map width in virtual pixels |
| `-height pixels` | - | Map height in virtual pixels |
| `-expire seconds` | - | Default lifetime for events/messages |
| `-tick seconds` | 60 | Update interval (0 = no updates) |
| `-path path` | - | Filesystem path for storing statistics |
| `-store patternList` | - | Message tag patterns to save to files |

---

## Item Types

### Node Items

Represent network elements like routers, switches, and end-systems.

```tcl
set router [$m create node -name "Router1" -address 192.168.1.1 -icon "router"]
```

### Port Items

Represent interfaces on nodes. Must be associated with a node.

```tcl
set eth0 [$m create port -name "eth0" -node $router -address 192.168.1.1]
set eth1 [$m create port -name "eth1" -node $router -address 10.0.0.1]
```

Port-specific option:
- `-node node` - Associate port with a node item

### Network Items

Represent transmission networks (Ethernet segments, subnets, etc.).

```tcl
set lan [$m create network -name "Office LAN" -address 192.168.1.0/24]
```

### Link Items

Connect ports to other ports or networks.

```tcl
set link [$m create link -src $port -dst $network]
```

### Group Items

Container items that hold other items.

```tcl
set servers [$m create group -name "Database Servers"]
$dbServer configure -group $servers
```

---

## Common Item Commands

### $item configure [options]

Configure item options.

```tcl
$router configure -name "Main Router" -color "blue"
```

### $item cget option

Get item option value.

```tcl
set name [$router cget -name]
set addr [$router cget -address]
```

### $item attribute [name [value]]

Get or set item attributes.

```tcl
$router attribute Snmp:Community "public"
$router attribute :Contact "admin@example.com"
```

### $item bind pattern script

Bind event handler to item.

```tcl
$router bind "snmp:error" {
    puts "SNMP error on %I: %A"
}
```

### $item raise tag [args]

Raise an event on the item.

```tcl
$router raise "status:down" "No SNMP response"
```

### $item message [options] tag text

Create a message on the item.

```tcl
$router message -interval 300 "ifload:eth0" "85"
```

### $item move [x y]

Move item position.

```tcl
# Get current position
set pos [$item move]

# Move by offset
$item move 10 -5
```

### $item info subject [pattern]

Get item-related handles.

```tcl
set events [$item info events]
set messages [$item info messages]
set bindings [$item info bindings]
set members [$group info member]  ;# For groups only
```

### $item map

Get the map containing this item.

```tcl
set parentMap [$item map]
```

### $item type

Get the item type.

```tcl
set type [$item type]  ;# Returns: node, network, port, link, or group
```

### $item dump

Return Tcl script to recreate the item.

```tcl
set script [$item dump]
```

### $item destroy

Destroy the item.

```tcl
$item destroy
```

---

## Common Item Options

| Option | Description |
|--------|-------------|
| `-name name` | Item name |
| `-address address` | Item address (format depends on usage) |
| `-color color` | Item color (Tk color names suggested) |
| `-font font` | Item font (Tk font names suggested) |
| `-icon icon` | Item icon identifier |
| `-tags tagList` | Tags for grouping items |
| `-group group` | Parent group item |
| `-expire seconds` | Event/message lifetime |
| `-path path` | Statistics storage path |
| `-store patternList` | Message patterns to save |
| `-ctime time` | Creation time (read-only) |
| `-mtime time` | Modification time (read-only) |

---

## Events

Events are raised on maps or items and trigger bound scripts.

### Event Binding Escapes

| Escape | Description |
|--------|-------------|
| `%%` | Literal percent sign |
| `%A` | Event arguments |
| `%B` | Binding handle |
| `%E` | Event handle |
| `%I` | Item handle (empty for map events) |
| `%M` | Map handle |
| `%N` | Event tag/name |
| `%P` | Pattern that matched |

### Event Processing

Events propagate from items to parent groups to the map. Use `break` or `continue` return codes to control processing.

```tcl
$item bind "error:*" {
    if {[handleError %A]} {
        return -code break  ;# Stop processing
    }
    return -code continue  ;# Skip to parent
}
```

### Event Instance Commands

```tcl
$event args      ;# Get event arguments
$event tag       ;# Get event tag
$event time      ;# Get event timestamp
$event item      ;# Get associated item (or empty)
$event map       ;# Get associated map
$event type      ;# Returns "event"
$event destroy   ;# Destroy the event
```

---

## Messages

Messages store timestamped data with optional health impact.

### Message Instance Commands

```tcl
$msg text        ;# Get message text
$msg tag         ;# Get message tag
$msg time        ;# Get timestamp
$msg interval    ;# Get time interval
$msg health      ;# Get health impact (-100..100)
$msg item        ;# Get associated item
$msg map         ;# Get associated map
$msg type        ;# Returns "message"
$msg destroy     ;# Destroy the message
```

---

## Bindings

### Binding Instance Commands

```tcl
$binding pattern  ;# Get the match pattern
$binding script   ;# Get the bound script
$binding item     ;# Get associated item
$binding map      ;# Get associated map
$binding type     ;# Returns "binding"
$binding destroy  ;# Destroy the binding
```

---

## Attributes

Attributes follow a naming convention:

**Persistent attributes** (saved with map):
```
[Scope]:Name[:Qualifier]*
```

Examples:
- `:Contact` - Global contact attribute
- `Snmp:Community` - SNMP community string
- `tnm:Monitor:ifSpeed:1` - Interface speed override

**Non-persistent attributes** (not saved):
- Must not start with capital letter or colon
- Example: `temp:lastCheck`

---

## Examples

### Create a Simple Network Map

```tcl
package require tnm 3.1.3

# Create map
set m [tnm::map create -name "Office Network" -width 800 -height 600]

# Create nodes
set router [$m create node -name "Gateway" -address 192.168.1.1]
set server [$m create node -name "FileServer" -address 192.168.1.10]
set pc1 [$m create node -name "Workstation1" -address 192.168.1.100]

# Create network segment
set lan [$m create network -name "Office LAN" -address 192.168.1.0/24]

# Create ports and links
set rport [$m create port -name "eth0" -node $router]
set sport [$m create port -name "eth0" -node $server]
set pport [$m create port -name "eth0" -node $pc1]

$m create link -src $rport -dst $lan
$m create link -src $sport -dst $lan
$m create link -src $pport -dst $lan

# Add attributes
$m attribute :Contact "it@company.com"
$router attribute Snmp:Community "public"
```

### Monitor Network with Events

```tcl
package require tnm 3.1.3

set m [tnm::map create -name "Monitored Network" -tick 60]

# Create monitored device
set router [$m create node -name "Router" -address 192.168.1.1 -tags "critical"]

# Bind event handlers
$router bind "ping:failed" {
    puts "ALERT: %I is unreachable!"
    tnm::syslog alert "Device down: [%I cget -name]"
}

$router bind "ping:success" {
    puts "OK: %I is reachable"
}

# Monitoring procedure
proc checkDevice {item} {
    set addr [$item cget -address]
    set result [tnm::icmp -timeout 2000 echo $addr]
    lassign [lindex $result 0] ip rtt

    if {$rtt eq ""} {
        $item raise "ping:failed" "No response"
    } else {
        $item raise "ping:success" "RTT: ${rtt}ms"
    }
}

# Periodic monitoring job
set j [tnm::job create -command {
    foreach item [$::m find -tags "critical"] {
        checkDevice $item
    }
} -interval 30000]

vwait forever
```

### Save and Load Maps

```tcl
package require tnm 3.1.3

proc saveMap {map filename} {
    set f [open $filename w]
    $map save $f
    close $f
    puts "Map saved to $filename"
}

proc loadMap {filename} {
    set m [tnm::map create]
    set f [open $filename r]
    $m load $f
    close $f
    puts "Map loaded from $filename"
    return $m
}

# Create and save
set m [tnm::map create -name "Test Map"]
$m create node -name "Server1" -address 10.0.0.1
saveMap $m "network.map"

# Load later
set loaded [loadMap "network.map"]
puts "Loaded map: [$loaded cget -name]"
```

### Statistics Collection

```tcl
package require tnm 3.1.3

set m [tnm::map create -name "Stats Map" \
    -path "/var/log/network" \
    -store {"^load:*" "^traffic:*"} \
    -tick 60]

set router [$m create node -name "Router" -address 192.168.1.1 \
    -path "/var/log/network/router" \
    -store {"^if:*"}]

# Log interface statistics (saved to file)
proc logInterfaceStats {item ifIndex inOctets outOctets} {
    $item message -interval 300 "if:traffic:$ifIndex" "$inOctets $outOctets"
}

# Log with health impact
proc logLoad {item load} {
    set health 0
    if {$load > 90} {
        set health -20
    } elseif {$load > 70} {
        set health -5
    }
    $item message -interval 60 -health $health "load:cpu" $load
}
```

---

## Notes

- Maps maintain modification timestamps for all items
- Setting tick to 0 disables automatic updates (may cause memory leaks)
- Events and messages expire based on the `-expire` option
- Bindings use Tcl string match pattern rules
- Items can belong to multiple groups
- Link items require port items as sources

---

## See Also

- [tnm.md](tnm.md) - Main TNM documentation
- [tnm::job](job.md) - Job scheduler for periodic monitoring
- [tnm::snmp](snmp.md) - SNMP for device management
