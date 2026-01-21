# TNM 3.1.3 Command Reference

**Version**: 3.1.3
**Platform**: Windows (MinGW64) & Linux
**Date**: January 21, 2026

## Purpose

This document provides a comprehensive reference for all TNM (Tcl Network Management) extension commands. TNM provides network management capabilities including SNMP, ICMP, DNS, and other network protocols for Tcl applications.

## Legend

- ✅ **Works**: Fully functional on the platform
- ⚠️ **Limited**: Works with known limitations or differences
- ❌ **Broken**: Non-functional or crashes
- 🔶 **Untested**: Not tested on the platform

---

## Tnm::syslog

**Purpose**: Send messages to system logging facility

| Sub-command | Windows | Linux | Description | Tcl Example |
|-------------|---------|-------|-------------|-------------|
| `emergency <msg>` | ✅ | ✅ | System is unusable | `Tnm::syslog emergency "Disk failure"` |
| `alert <msg>` | ✅ | ✅ | Action must be taken immediately | `Tnm::syslog alert "Database corruption"` |
| `critical <msg>` | ✅ | ✅ | Critical conditions | `Tnm::syslog critical "Temperature too high"` |
| `error <msg>` | ✅ | ✅ | Error conditions | `Tnm::syslog error "Failed to connect"` |
| `warning <msg>` | ✅ | ✅ | Warning conditions | `Tnm::syslog warning "Low disk space"` |
| `notice <msg>` | ✅ | ✅ | Normal but significant | `Tnm::syslog notice "Service started"` |
| `info <msg>` | ✅ | ✅ | Informational messages | `Tnm::syslog info "User login"` |
| `debug <msg>` | ✅ | ✅ | Debug-level messages | `Tnm::syslog debug "Variable x=42"` |

**Options**: `-ident <string>` (set program name), `-facility <name>` (set facility: daemon, user, local0-7)

**Notes**: 100% functional on both platforms. Windows uses internal logging, Linux uses native syslog.

---

## Tnm::map

**Purpose**: Create and manage network topology maps

| Sub-command | Windows | Linux | Description | Tcl Example |
|-------------|---------|-------|-------------|-------------|
| `create` | ✅ | ✅ | Create new map object | `set m [Tnm::map create]` |
| `find ?pattern?` | ✅ | ✅ | Find maps matching pattern | `Tnm::map find *` |
| `info ?option?` | ✅ | ✅ | Get map information | `Tnm::map info class` |
| `$map configure` | ✅ | ✅ | Configure map attributes | `$m configure -name "Network1"` |
| `$map cget` | ✅ | ✅ | Get map attribute | `$m cget -name` |
| `$map destroy` | ✅ | ✅ | Destroy map object | `$m destroy` |

**Options**: `-name <string>`, `-tags <list>`, `-command <script>`

**Notes**: 100% functional. Core object management for network maps.

---

## Tnm::icmp

**Purpose**: Send ICMP packets for network diagnostics

| Sub-command | Windows | Linux | Description | Tcl Example |
|-------------|---------|-------|-------------|-------------|
| `echo <hosts>` | ✅ | ✅ | Send ICMP echo (ping) | `Tnm::icmp echo 192.168.1.1` |
| `mask <hosts>` | ❌ | ✅ | Request address mask | `Tnm::icmp mask 10.0.0.1` |
| `timestamp <hosts>` | ❌ | ✅ | Request timestamp | `Tnm::icmp timestamp 10.0.0.1` |
| `ttl <hop> <hosts>` | ✅ | ✅ | Set TTL for echo | `Tnm::icmp ttl 5 192.168.1.1` |
| `trace <hop> <hosts>` | ✅ | ✅ | Traceroute functionality | `Tnm::icmp trace 10 google.com` |

**Options**: `-timeout <ms>`, `-retries <n>`, `-size <bytes>`, `-delay <ms>` (⚠️ Windows ignores), `-window <n>` (⚠️ Windows differs)

**Notes**: Windows 65% functional - ICMP.DLL limitations prevent mask/timestamp. Echo and trace work reliably. Linux 100% functional.

---

## Tnm::udp

**Purpose**: Create UDP sockets for datagram communication

| Sub-command | Windows | Linux | Description | Tcl Example |
|-------------|---------|-------|-------------|-------------|
| `create` | ✅ | ✅ | Create UDP socket | `set sock [Tnm::udp create]` |
| `$sock send <data>` | ✅ | ✅ | Send UDP datagram | `$sock send "Hello"` |
| `$sock receive` | ✅ | ✅ | Receive UDP datagram | `set data [$sock receive]` |
| `$sock connect <host> <port>` | ✅ | ✅ | Connect to remote endpoint | `$sock connect 10.0.0.1 5000` |
| `$sock configure` | ✅ | ✅ | Configure socket options | `$sock configure -port 8888` |
| `$sock cget` | ✅ | ✅ | Get socket option | `$sock cget -myport` |
| `$sock bind` | ✅ | ✅ | Bind callback to socket | `$sock bind read HandleData` |
| `$sock destroy` | ✅ | ✅ | Destroy socket | `$sock destroy` |

**Options**: `-address <host>`, `-port <n>`, `-myaddress <host>`, `-myport <n>`, `-read <cmd>`, `-write <cmd>`, `-tags <list>`

**Notes**: 97% functional. Reliable UDP implementation on both platforms.

---

## Tnm::job

**Purpose**: Schedule and manage periodic background jobs

| Sub-command | Windows | Linux | Description | Tcl Example |
|-------------|---------|-------|-------------|-------------|
| `create` | ✅ | ✅ | Create job object | `set j [Tnm::job create -interval 5000]` |
| `find ?pattern?` | ✅ | ✅ | Find jobs matching pattern | `Tnm::job find *` |
| `current` | ✅ | ✅ | Get current job handle | `Tnm::job current` |
| `schedule <ms> <script>` | ✅ | ✅ | One-time scheduled execution | `Tnm::job schedule 1000 {puts "Hi"}` |
| `wait` | ✅ | ✅ | Wait for job completion | `Tnm::job wait $j` |
| `$job configure` | ✅ | ✅ | Configure job attributes | `$j configure -interval 10000` |
| `$job cget` | ✅ | ✅ | Get job attribute | `$j cget -status` |
| `$job destroy` | ✅ | ✅ | Destroy job | `$j destroy` |

**Options**: `-interval <ms>`, `-iterations <n>`, `-status <state>`, `-command <script>`, `-exit <script>`, `-error <script>`, `-tags <list>`

**Notes**: 91% functional. Works reliably but has sub-interpreter limitations (cannot create jobs in child interpreters).

---

## Tnm::netdb

**Purpose**: Query network database information (hosts, services, protocols)

| Sub-command | Windows | Linux | Description | Tcl Example |
|-------------|---------|-------|-------------|-------------|
| `hosts address <name>` | ✅ | ✅ | Get IP address from hostname | `Tnm::netdb hosts address localhost` |
| `hosts name <addr>` | ✅ | ✅ | Get hostname from IP | `Tnm::netdb hosts name 127.0.0.1` |
| `hosts aliases <arg>` | ✅ | ✅ | Get host aliases | `Tnm::netdb hosts aliases localhost` |
| `services name <num> <proto>` | ✅ | ✅ | Get service name | `Tnm::netdb services name 80 tcp` |
| `services number <name> <proto>` | ✅ | ✅ | Get service port | `Tnm::netdb services number http tcp` |
| `services aliases <arg> <proto>` | ✅ | ✅ | Get service aliases | `Tnm::netdb services aliases http tcp` |
| `protocols name <num>` | ✅ | ✅ | Get protocol name | `Tnm::netdb protocols name 6` |
| `protocols number <name>` | ✅ | ✅ | Get protocol number | `Tnm::netdb protocols number tcp` |
| `protocols aliases <arg>` | ✅ | ✅ | Get protocol aliases | `Tnm::netdb protocols aliases tcp` |
| `networks name <addr>` | ⚠️ | ✅ | Get network name | `Tnm::netdb networks name 10.0.0.0` |
| `networks address <name>` | ⚠️ | ✅ | Get network address | `Tnm::netdb networks address loopback` |
| `networks aliases <arg>` | ⚠️ | ✅ | Get network aliases | `Tnm::netdb networks aliases loopback` |
| `sunrpcs name <num>` | ❌ | ✅ | Get RPC service name | `Tnm::netdb sunrpcs name 100000` |
| `sunrpcs number <name>` | ❌ | ✅ | Get RPC service number | `Tnm::netdb sunrpcs number portmapper` |
| `sunrpcs aliases <arg>` | ❌ | ✅ | Get RPC aliases | `Tnm::netdb sunrpcs aliases portmapper` |
| `ip class <addr>` | ✅ | ✅ | Get IP address class | `Tnm::netdb ip class 192.168.1.1` |
| `ip range <addr>` | ✅ | ✅ | Get address range | `Tnm::netdb ip range 10.0.0.0/24` |
| `ip apply <range> <script>` | ✅ | ✅ | Apply script to range | `Tnm::netdb ip apply 192.168.1.0/24 {puts}` |
| `ip broadcast <addr>` | ✅ | ✅ | Get broadcast address | `Tnm::netdb ip broadcast 192.168.1.0/24` |
| `ip compare <a1> <a2>` | ✅ | ✅ | Compare addresses | `Tnm::netdb ip compare 10.0.0.1 10.0.0.2` |

**Notes**: 76% functional on Windows. Networks queries limited, sunrpcs disabled on Windows (no getrpcent()). All other functions work reliably.

---

## Tnm::snmp

**Purpose**: SNMP protocol operations (v1, v2c, v3)

| Sub-command | Windows | Linux | Description | Tcl Example |
|-------------|---------|-------|-------------|-------------|
| `generator` | ✅ | ✅ | Create SNMP session | `set s [Tnm::snmp generator -address 10.0.0.1]` |
| `listener` | ✅ | ✅ | Create trap listener | `set l [Tnm::snmp listener -port 162]` |
| `responder` | ✅ | ✅ | Create SNMP responder | `set r [Tnm::snmp responder]` |
| `notifier` | ✅ | ✅ | Send notifications | `set n [Tnm::snmp notifier]` |
| `alias <name> <oid>` | ✅ | ✅ | Create OID alias | `Tnm::snmp alias sysDescr 1.3.6.1.2.1.1.1.0` |
| `find ?pattern?` | ✅ | ✅ | Find SNMP sessions | `Tnm::snmp find *` |
| `info ?option?` | ✅ | ✅ | Get session info | `Tnm::snmp info class` |
| `wait` | ✅ | ✅ | Wait for requests | `Tnm::snmp wait` |
| `$s get <vbl>` | ✅ | ✅ | SNMP GET request | `$s get sysDescr.0` |
| `$s getnext <vbl>` | ✅ | ✅ | SNMP GETNEXT request | `$s getnext system` |
| `$s getbulk <n> <m> <vbl>` | ✅ | ✅ | SNMP GETBULK request (v2c+) | `$s getbulk 0 10 ifTable` |
| `$s set <vbl>` | ✅ | ✅ | SNMP SET request | `$s set {sysContact.0 "Admin"}` |
| `$s walk <vbl>` | ✅ | ✅ | Walk MIB tree | `$s walk system` |
| `$s configure` | ✅ | ✅ | Configure session | `$s configure -timeout 5000` |
| `$s cget` | ✅ | ✅ | Get session option | `$s cget -version` |
| `$s destroy` | ✅ | ✅ | Destroy session | `$s destroy` |

**Options**: `-address <host>`, `-port <n>`, `-version <v>` (SNMPv1/SNMPv2c/SNMPv3), `-read <community>`, `-write <community>`, `-user <name>` (v3), `-context <name>` (v3), `-timeout <ms>`, `-retries <n>`, `-window <n>`

**Notes**: 100% functional on both platforms. Complete SNMPv1/v2c/v3 implementation.

---

## Tnm::mib

**Purpose**: MIB (Management Information Base) operations

| Sub-command | Windows | Linux | Description | Tcl Example |
|-------------|---------|-------|-------------|-------------|
| `access <oid>` | ✅ | ✅ | Get MIB object access | `Tnm::mib access sysDescr` |
| `children <oid>` | ✅ | ✅ | Get child nodes | `Tnm::mib children system` |
| `compare <o1> <o2>` | ✅ | ✅ | Compare OIDs | `Tnm::mib compare 1.3.6.1.2.1.1 1.3.6.1.2.1.2` |
| `defval <oid>` | ✅ | ✅ | Get default value | `Tnm::mib defval sysContact` |
| `description <oid>` | ✅ | ✅ | Get object description | `Tnm::mib description sysDescr` |
| `displayhint <oid>` | ✅ | ✅ | Get display hint | `Tnm::mib displayhint ifPhysAddress` |
| `enums <oid>` | ✅ | ✅ | Get enumeration values | `Tnm::mib enums ifAdminStatus` |
| `exists <oid>` | ✅ | ✅ | Check if OID exists | `Tnm::mib exists sysDescr` |
| `file <module>` | ✅ | ✅ | Get MIB file path | `Tnm::mib file SNMPv2-MIB` |
| `format <oid> <val>` | ✅ | ✅ | Format value | `Tnm::mib format sysDescr "text"` |
| `index <oid>` | ✅ | ✅ | Get index information | `Tnm::mib index ifEntry` |
| `info <option>` | ✅ | ✅ | Get MIB info | `Tnm::mib info loaded` |
| `label <oid>` | ✅ | ✅ | Get object label | `Tnm::mib label 1.3.6.1.2.1.1.1.0` |
| `length <oid>` | ✅ | ✅ | Get object length | `Tnm::mib length sysDescr` |
| `load <file>` | ✅ | ✅ | Load MIB file | `Tnm::mib load IF-MIB.txt` |
| `macro <name>` | ✅ | ✅ | Get macro definition | `Tnm::mib macro MODULE-IDENTITY` |
| `member <oid> <val>` | ✅ | ✅ | Get member name | `Tnm::mib member ifAdminStatus 1` |
| `module <oid>` | ✅ | ✅ | Get module name | `Tnm::mib module sysDescr` |
| `name <oid>` | ✅ | ✅ | Get fully qualified name | `Tnm::mib name 1.3.6.1.2.1.1.1.0` |
| `oid <name>` | ✅ | ✅ | Get OID from name | `Tnm::mib oid sysDescr.0` |
| `pack <oid> <idx>` | ✅ | ✅ | Pack index values | `Tnm::mib pack ifEntry.1 5` |
| `parent <oid>` | ✅ | ✅ | Get parent OID | `Tnm::mib parent sysDescr` |
| `range <oid>` | ✅ | ✅ | Get value range | `Tnm::mib range ifMtu` |
| `scan <oid> <inst>` | ✅ | ✅ | Scan instance OID | `Tnm::mib scan sysDescr.0 var` |
| `size <oid>` | ✅ | ✅ | Get object size | `Tnm::mib size sysDescr` |
| `split <oid>` | ✅ | ✅ | Split into base and instance | `Tnm::mib split sysDescr.0 base inst` |
| `status <oid>` | ✅ | ✅ | Get object status | `Tnm::mib status sysDescr` |
| `subtree <oid>` | ✅ | ✅ | Get entire subtree | `Tnm::mib subtree system` |
| `syntax <oid>` | ✅ | ✅ | Get object syntax | `Tnm::mib syntax sysUpTime` |
| `type <oid>` | ✅ | ✅ | Get object type | `Tnm::mib type ifTable` |
| `unpack <oid> <inst>` | ✅ | ✅ | Unpack instance values | `Tnm::mib unpack ifEntry.1.5 var` |
| `variables <oid>` | ✅ | ✅ | Get table variables | `Tnm::mib variables ifEntry` |
| `walk <oid> <script>` | ✅ | ✅ | Walk MIB tree | `Tnm::mib walk system {puts}` |

**Notes**: 100% functional on both platforms. Complete MIB database operations with 33 sub-commands.

---

## Tnm::dns

**Purpose**: DNS (Domain Name System) queries

| Sub-command | Windows | Linux | Description | Tcl Example |
|-------------|---------|-------|-------------|-------------|
| `address <name>` | ❌ | ✅ | Query A records | `Tnm::dns address www.google.com` |
| `name <addr>` | ❌ | ✅ | Query PTR records | `Tnm::dns name 8.8.8.8` |
| `hinfo <name>` | ❌ | ✅ | Query HINFO records | `Tnm::dns hinfo host.example.com` |
| `mx <name>` | ❌ | ✅ | Query MX records | `Tnm::dns mx example.com` |
| `soa <name>` | ❌ | ✅ | Query SOA records | `Tnm::dns soa example.com` |
| `txt <name>` | ❌ | ✅ | Query TXT records | `Tnm::dns txt example.com` |
| `cname <name>` | ❌ | ✅ | Query CNAME records | `Tnm::dns cname www.example.com` |
| `ns <name>` | ❌ | ✅ | Query NS records | `Tnm::dns ns example.com` |

**Options**: `-timeout <ms>`, `-retries <n>`, `-server <addr>`

**Notes**: ❌ **CRASHES ON WINDOWS** with segmentation fault. Root cause: Incomplete `_res` structure in MinGW64 headers. Linux 100% functional. **DO NOT USE ON WINDOWS**.

---

## Tnm::ntp

**Purpose**: NTP (Network Time Protocol) queries

| Sub-command | Windows | Linux | Description | Tcl Example |
|-------------|---------|-------|-------------|-------------|
| `<server> <arrayvar>` | ❌ | ✅ | Query NTP server | `Tnm::ntp pool.ntp.org result` |

**Options**: `-timeout <ms>`, `-retries <n>`

**Notes**: ❌ **CRASHES ON WINDOWS** due to resolver dependency issues. Linux functional. Returns time synchronization data in array variable. **DO NOT USE ON WINDOWS**.

---

## Tnm::smx

**Purpose**: SMX (SNMP Multiplexer) operations for distributed SNMP processing

| Sub-command | Windows | Linux | Description | Tcl Example |
|-------------|---------|-------|-------------|-------------|
| `error <code> <msg>` | 🔶 | 🔶 | Report SMX error | `Tnm::smx error 1 "Connection failed"` |
| `exit <code>` | 🔶 | 🔶 | Exit with code | `Tnm::smx exit 0` |
| `log <msg>` | 🔶 | 🔶 | Log SMX message | `Tnm::smx log "Processing started"` |
| `profiles` | 🔶 | 🔶 | Get available profiles | `Tnm::smx profiles` |
| `result <data>` | 🔶 | 🔶 | Return result data | `Tnm::smx result $output` |

**Notes**: SMX functionality for distributed SNMP management. Requires SMX framework setup. Untested in current environment.

---

## Tnm::ined

**Purpose**: Tkined (Tk-based Network Editor) integration

| Sub-command | Windows | Linux | Description | Tcl Example |
|-------------|---------|-------|-------------|-------------|
| Various GUI commands | 🔶 | 🔶 | Network topology editor | Requires Tkined application |

**Notes**: Tkined integration for graphical network management. Requires Tk toolkit and Tkined application. Not applicable for console-only TNM usage. See Tkined documentation for GUI-specific commands.

---

## Command Summary Table

| Command | Windows Status | Linux Status | Primary Use Case |
|---------|----------------|--------------|------------------|
| **Tnm::syslog** | ✅ 100% | ✅ 100% | System logging |
| **Tnm::map** | ✅ 100% | ✅ 100% | Network topology maps |
| **Tnm::icmp** | ⚠️ 65% | ✅ 100% | Ping, traceroute (mask/timestamp broken on Windows) |
| **Tnm::udp** | ✅ 97% | ✅ 97% | UDP socket communication |
| **Tnm::job** | ✅ 91% | ✅ 91% | Scheduled background jobs |
| **Tnm::netdb** | ⚠️ 76% | ✅ 100% | Network database queries (sunrpcs disabled on Windows) |
| **Tnm::snmp** | ✅ 100% | ✅ 100% | SNMP v1/v2c/v3 operations |
| **Tnm::mib** | ✅ 100% | ✅ 100% | MIB database operations |
| **Tnm::dns** | ❌ 0% | ✅ 100% | DNS queries (CRASHES ON WINDOWS) |
| **Tnm::ntp** | ❌ 0% | ✅ 100% | NTP time queries (CRASHES ON WINDOWS) |
| **Tnm::smx** | 🔶 Untested | 🔶 Untested | SNMP multiplexer operations |
| **Tnm::ined** | 🔶 GUI | 🔶 GUI | Tkined graphical interface |

---

## Platform-Specific Notes

### Windows (MinGW64)

**Working Well (7 commands):**
- Tnm::syslog, Tnm::map, Tnm::udp, Tnm::job, Tnm::snmp, Tnm::mib, Tnm::icmp (partial)

**Limited Functionality (1 command):**
- Tnm::netdb: sunrpcs disabled, networks queries limited
- Tnm::icmp: mask and timestamp commands fail (ICMP.DLL limitations)

**Broken/Unsafe (2 commands):**
- Tnm::dns: Segmentation fault due to incomplete _res structure in MinGW64 headers
- Tnm::ntp: Crashes due to resolver dependencies

**Recommendation**: Use Tnm::netdb hosts for DNS lookups instead of Tnm::dns on Windows.

### Linux

**All core commands fully functional** except:
- Tnm::smx: Untested (requires SMX framework)
- Tnm::ined: GUI-specific (requires Tkined)

---

## Quick Start Examples

### SNMP Device Query
```tcl
package require Tnm 3.1.3

# Query system information
set s [Tnm::snmp generator -address 192.168.1.1 -read public]
set vbl [$s get sysDescr.0 sysUpTime.0 sysContact.0]
foreach vb $vbl {
    puts "[lindex $vb 0]: [lindex $vb 2]"
}
$s destroy
```

### ICMP Network Sweep
```tcl
package require Tnm 3.1.3

# Ping multiple hosts
set hosts {192.168.1.1 192.168.1.2 192.168.1.3}
set results [Tnm::icmp echo $hosts]
foreach result $results {
    puts "Host [lindex $result 0]: RTT [lindex $result 1]ms"
}
```

### Periodic Monitoring Job
```tcl
package require Tnm 3.1.3

# Monitor every 10 seconds
set job [Tnm::job create -command {
    puts "Checking status: [clock format [clock seconds]]"
    # Add monitoring logic here
} -interval 10000]

# Run for 60 seconds then stop
after 60000 [list $job destroy]
vwait forever
```

### MIB Tree Walk
```tcl
package require Tnm 3.1.3

# Walk system subtree
Tnm::mib walk system oidVar labelVar {
    puts "$labelVar ($oidVar)"
}
```

---

## Additional Resources

- **TNM Documentation**: See `doc/` directory in scotty source
- **MIB Files**: Located in `mibs/` directory
- **Test Suite**: See `tests/` directory for usage examples
- **Platform Issues**: See `TEST_FAILURES_ANALYSIS.md` for detailed platform-specific behavior

---

**Document Generated**: January 21, 2026
**TNM Version**: 3.1.3
**Build Platform**: Windows MinGW64 / Linux
