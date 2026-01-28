# tnm::netdb - Network Database Commands

Access local network information databases for hosts, services, protocols, and networks.

**Status:** ✅ 100% functional (sunrpcs removed)

---

## Synopsis

```tcl
tnm::netdb hosts [subcommand] [args]
tnm::netdb services [subcommand] [args]
tnm::netdb protocols [subcommand] [args]
tnm::netdb networks [subcommand] [args]
tnm::netdb ip [subcommand] [args]
```

---

## Description

The `tnm::netdb` command provides access to network information stored in system configuration files, the Network Information Service (NIS), or the Domain Name System (DNS). Results depend on local system configuration.

---

## Hosts Database

### tnm::netdb hosts

List all locally known hosts.

```tcl
set hosts [tnm::netdb hosts]
# Returns list of {name address} pairs
```

**Note:** This may return an empty list on systems configured to use DNS only.

### tnm::netdb hosts address name

Get IP address from hostname.

```tcl
set ip [tnm::netdb hosts address localhost]
# Returns: 127.0.0.1

set ip [tnm::netdb hosts address myserver]
```

### tnm::netdb hosts name address

Get hostname from IP address.

```tcl
set name [tnm::netdb hosts name 127.0.0.1]
# Returns: localhost
```

### tnm::netdb hosts aliases address

Get all aliases for an IP address.

```tcl
set aliases [tnm::netdb hosts aliases 127.0.0.1]
# Returns: {localhost localhost.localdomain}
```

---

## Services Database

### tnm::netdb services

List all locally known services.

```tcl
set services [tnm::netdb services]
# Returns list of {name port protocol} triples
```

### tnm::netdb services name port protocol

Get service name from port number.

```tcl
set name [tnm::netdb services name 80 tcp]
# Returns: http

set name [tnm::netdb services name 53 udp]
# Returns: domain

set name [tnm::netdb services name 22 tcp]
# Returns: ssh
```

### tnm::netdb services number name protocol

Get port number from service name.

```tcl
set port [tnm::netdb services number http tcp]
# Returns: 80

set port [tnm::netdb services number ssh tcp]
# Returns: 22

set port [tnm::netdb services number domain udp]
# Returns: 53
```

### tnm::netdb services aliases port protocol

Get service aliases.

```tcl
set aliases [tnm::netdb services aliases 80 tcp]
# Returns: {http www www-http}
```

---

## Protocols Database

### tnm::netdb protocols

List all locally known Internet protocols.

```tcl
set protocols [tnm::netdb protocols]
# Returns list of {name number} pairs
```

### tnm::netdb protocols name number

Get protocol name from number.

```tcl
set name [tnm::netdb protocols name 6]
# Returns: tcp

set name [tnm::netdb protocols name 17]
# Returns: udp

set name [tnm::netdb protocols name 1]
# Returns: icmp
```

### tnm::netdb protocols number name

Get protocol number from name.

```tcl
set num [tnm::netdb protocols number tcp]
# Returns: 6

set num [tnm::netdb protocols number udp]
# Returns: 17

set num [tnm::netdb protocols number icmp]
# Returns: 1
```

### tnm::netdb protocols aliases number

Get protocol aliases.

```tcl
set aliases [tnm::netdb protocols aliases 6]
# Returns: {tcp TCP}
```

---

## Networks Database

### tnm::netdb networks

List all locally known networks.

```tcl
set networks [tnm::netdb networks]
# Returns list of {name address} pairs
```

### tnm::netdb networks name address

Get network name from address.

```tcl
set name [tnm::netdb networks name 127.0.0.0]
# Returns: loopback
```

### tnm::netdb networks address name

Get network address from name.

```tcl
set addr [tnm::netdb networks address loopback]
# Returns: 127.0.0.0
```

### tnm::netdb networks aliases address

Get network aliases.

```tcl
set aliases [tnm::netdb networks aliases 127.0.0.0]
```

**Note:** Network lookups may be limited on Windows.

---

## IP Address Operations

### tnm::netdb ip class address

Get the IP address class.

```tcl
tnm::netdb ip class 10.0.0.1
# Returns: A

tnm::netdb ip class 172.16.0.1
# Returns: B

tnm::netdb ip class 192.168.1.1
# Returns: C

tnm::netdb ip class 224.0.0.1
# Returns: D

tnm::netdb ip class 127.0.0.1
# Returns: loopback
```

### tnm::netdb ip apply address mask

Apply a netmask to an address (get network address).

```tcl
tnm::netdb ip apply 192.168.1.100 255.255.255.0
# Returns: 192.168.1.0

tnm::netdb ip apply 10.1.2.3 255.0.0.0
# Returns: 10.0.0.0
```

### tnm::netdb ip broadcast address mask

Get the broadcast address for a network.

```tcl
tnm::netdb ip broadcast 192.168.1.0 255.255.255.0
# Returns: 192.168.1.255

tnm::netdb ip broadcast 10.0.0.0 255.0.0.0
# Returns: 10.255.255.255
```

### tnm::netdb ip compare mask1 mask2

Compare two netmasks.

```tcl
tnm::netdb ip compare 255.255.255.0 255.255.0.0
# Returns: 1 (first is more specific)

tnm::netdb ip compare 255.255.0.0 255.255.255.0
# Returns: -1 (first is less specific)

tnm::netdb ip compare 255.255.255.0 255.255.255.0
# Returns: 0 (equal)
```

### tnm::netdb ip range address mask

Get all IP addresses in a network range.

```tcl
set ips [tnm::netdb ip range 192.168.1.0 255.255.255.248]
# Returns: {192.168.1.0 192.168.1.1 ... 192.168.1.7}
```

**Caution:** Large ranges can generate many addresses!

---

## Examples

### List Common Services

```tcl
package require tnm 3.1.3

set common_ports {21 22 23 25 53 80 110 143 443 993 995}

puts "Common Services:"
puts [format "%-10s %-20s" "Port" "Service"]
puts [string repeat "-" 30]

foreach port $common_ports {
    if {[catch {tnm::netdb services name $port tcp} name]} {
        set name "unknown"
    }
    puts [format "%-10s %-20s" $port $name]
}
```

### Subnet Calculator

```tcl
package require tnm 3.1.3

proc subnetInfo {address mask} {
    set network [tnm::netdb ip apply $address $mask]
    set broadcast [tnm::netdb ip broadcast $address $mask]
    set class [tnm::netdb ip class $address]

    puts "Address:   $address"
    puts "Netmask:   $mask"
    puts "Network:   $network"
    puts "Broadcast: $broadcast"
    puts "Class:     $class"
}

subnetInfo 192.168.1.100 255.255.255.0
```

### Resolve Host Information

```tcl
package require tnm 3.1.3

proc hostInfo {host} {
    puts "Host Information for: $host"
    puts [string repeat "-" 40]

    if {[catch {tnm::netdb hosts address $host} ip]} {
        puts "Cannot resolve hostname"
        return
    }
    puts "IP Address: $ip"

    if {[catch {tnm::netdb hosts name $ip} name]} {
        set name "(no reverse)"
    }
    puts "Reverse:    $name"

    set class [tnm::netdb ip class $ip]
    puts "IP Class:   $class"

    if {[catch {tnm::netdb hosts aliases $ip} aliases]} {
        set aliases "(none)"
    }
    puts "Aliases:    $aliases"
}

hostInfo localhost
```

### Generate IP Range

```tcl
package require tnm 3.1.3

# Get all hosts in a /28 subnet
set ips [tnm::netdb ip range 192.168.1.0 255.255.255.240]
puts "Hosts in 192.168.1.0/28:"
foreach ip $ips {
    puts "  $ip"
}
```

---

## Platform Notes

### Windows
- Host lookups work via Windows Sockets API
- Service and protocol databases use Windows equivalents
- Network database queries may be limited

### Linux
- Uses /etc/hosts, /etc/services, /etc/protocols, /etc/networks
- May also query NIS or DNS depending on nsswitch.conf

### Removed Commands
- `tnm::netdb sunrpcs` has been removed (RPC support disabled)

---

## See Also

- [tnm.md](tnm.md) - Main TNM documentation
- [tnm::dns](dns.md) - DNS queries for remote lookups
