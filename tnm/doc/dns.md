# tnm::dns - DNS Query Commands

Query the Domain Name System (DNS) for host and domain information.

**Status:** ✅ 100% functional on Windows and Linux

---

## Synopsis

```tcl
tnm::dns [options] address host
tnm::dns [options] name address
tnm::dns [options] cname name
tnm::dns [options] hinfo name
tnm::dns [options] mx name
tnm::dns [options] ns name
tnm::dns [options] soa name
tnm::dns [options] txt name
```

---

## Description

The `tnm::dns` command provides access to the Domain Name System (DNS). It allows querying various DNS record types including A, PTR, CNAME, HINFO, MX, NS, SOA, and TXT records.

---

## Commands

### tnm::dns address host

Query A records to get IP addresses for a hostname.

**Syntax:**
```tcl
tnm::dns [options] address host
```

**Parameters:**
- `host` - The hostname to resolve

**Returns:** List of IP addresses

**Example:**
```tcl
set ips [tnm::dns address www.example.com]
# Returns: {93.184.216.34}

set ips [tnm::dns address google.com]
# Returns: {142.250.185.46 142.250.185.78 ...}
```

---

### tnm::dns name address

Query PTR records to get hostname from IP address (reverse lookup).

**Syntax:**
```tcl
tnm::dns [options] name address
```

**Parameters:**
- `address` - The IP address to look up

**Returns:** Hostname string

**Example:**
```tcl
set name [tnm::dns name 8.8.8.8]
# Returns: dns.google

set name [tnm::dns name 127.0.0.1]
# Returns: localhost
```

---

### tnm::dns cname name

Query CNAME records to get canonical name.

**Syntax:**
```tcl
tnm::dns [options] cname name
```

**Parameters:**
- `name` - The alias name to look up

**Returns:** Canonical name string

**Example:**
```tcl
set canonical [tnm::dns cname www.example.com]
```

---

### tnm::dns hinfo name

Query HINFO records to get host hardware and OS information.

**Syntax:**
```tcl
tnm::dns [options] hinfo name
```

**Parameters:**
- `name` - The hostname to query

**Returns:** List containing CPU type and OS type

**Example:**
```tcl
set info [tnm::dns hinfo host.example.com]
# Returns: {CPU-TYPE OS-TYPE}
```

**Note:** HINFO records are rarely used in modern DNS configurations.

---

### tnm::dns mx name

Query MX records to get mail exchangers for a domain.

**Syntax:**
```tcl
tnm::dns [options] mx name
```

**Parameters:**
- `name` - The domain name to query

**Returns:** List of {mailserver priority} pairs, sorted by priority

**Example:**
```tcl
set mxlist [tnm::dns mx example.com]
# Returns: {{mail1.example.com 10} {mail2.example.com 20}}

foreach mx $mxlist {
    lassign $mx server priority
    puts "MX $priority: $server"
}
```

---

### tnm::dns ns name

Query NS records to get authoritative name servers for a domain.

**Syntax:**
```tcl
tnm::dns [options] ns name
```

**Parameters:**
- `name` - The domain name to query

**Returns:** List of name server hostnames

**Example:**
```tcl
set servers [tnm::dns ns example.com]
# Returns: {ns1.example.com ns2.example.com}
```

---

### tnm::dns soa name

Query SOA (Start of Authority) record for a domain.

**Syntax:**
```tcl
tnm::dns [options] soa name
```

**Parameters:**
- `name` - The domain name to query

**Returns:** SOA record information including primary NS, admin email, serial, refresh, retry, expire, and minimum TTL

**Example:**
```tcl
set soa [tnm::dns soa example.com]
```

---

### tnm::dns txt name

Query TXT records for a domain.

**Syntax:**
```tcl
tnm::dns [options] txt name
```

**Parameters:**
- `name` - The domain name to query

**Returns:** List of TXT record strings

**Example:**
```tcl
set txt [tnm::dns txt example.com]
# May return SPF records, DKIM keys, verification tokens, etc.
```

---

## Options

| Option | Default | Description |
|--------|---------|-------------|
| `-server server` | System default | DNS server(s) to query |
| `-timeout time` | 2 | Timeout in seconds |
| `-retries number` | 2 | Number of retries |

### Setting Options

Options can be set for individual queries:

```tcl
tnm::dns -timeout 5 -retries 3 address www.example.com
```

Or set as defaults for the interpreter:

```tcl
tnm::dns -timeout 5 -retries 3
# Now all subsequent queries use these defaults
```

---

## Platform Notes

### Windows
- Uses native DnsQuery API (dnsapi.dll)
- The `-server` option is **not supported** - queries always use system-configured DNS servers
- All record types are fully functional

### Linux
- Uses BSD resolver library
- The `-server` option allows specifying custom DNS servers
- All record types are fully functional

---

## Error Handling

DNS queries may fail for various reasons:

```tcl
if {[catch {tnm::dns address nonexistent.example.com} result]} {
    puts "DNS lookup failed: $result"
}
```

Common errors:
- Host not found (NXDOMAIN)
- Server timeout
- Network unreachable

---

## Examples

### Basic Host Lookup

```tcl
package require tnm 3.1.3

# Forward lookup
set ip [tnm::dns address www.google.com]
puts "IP: $ip"

# Reverse lookup
set name [tnm::dns name $ip]
puts "Name: $name"
```

### Get Mail Servers

```tcl
package require tnm 3.1.3

set domain "gmail.com"
set mxlist [tnm::dns mx $domain]

puts "Mail servers for $domain:"
foreach mx $mxlist {
    lassign $mx server priority
    puts "  Priority $priority: $server"
}
```

### Check Domain Name Servers

```tcl
package require tnm 3.1.3

set domain "example.com"
set ns [tnm::dns ns $domain]
set soa [tnm::dns soa $domain]

puts "Name servers for $domain:"
foreach server $ns {
    puts "  $server"
}
puts "\nSOA record:"
puts "  $soa"
```

---

## See Also

- [tnm.md](tnm.md) - Main TNM documentation
- [tnm::netdb](netdb.md) - Local network database queries
- RFC 1034, RFC 1035 - Domain Name System specification
