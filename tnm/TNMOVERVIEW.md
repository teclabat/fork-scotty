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
- ✂️ **Removed**: Removed from the build (not available)

## Removed Commands

**Note**: The following commands have been removed from the build:

- ✂️ **tnm::smx** - Removed from all platforms (unused Script MIB Executive feature)
- ✂️ **tnm::ined** - Removed from all platforms (Tkined GUI integration not needed)
- ✂️ **tnm::netdb sunrpcs** - Removed from all platforms (RPC support disabled)
- ✂️ **Windows only**: `tnm::icmp mask` and `timestamp` - Removed due to Windows ICMP.DLL API limitations

---

## Command Summary Table

| Command | Windows Status | Linux Status | Primary Use Case |
|---------|----------------|--------------|------------------|
| **tnm::syslog** | ✅ 100% | ✅ 100% | System logging |
| **tnm::map** | ✅ 100% | ✅ 100% | Network topology maps |
| **tnm::icmp** | ✅ 100% | ✅ 100% | Ping, traceroute (mask/timestamp removed on Windows) |
| **tnm::udp** | ✅ 100% | ✅ 100% | UDP socket communication |
| **tnm::job** | ✅ 91% | ✅ 91% | Scheduled background jobs |
| **tnm::netdb** | ✅ 100% | ✅ 100% | Network database queries (sunrpcs removed) |
| **tnm::snmp** | ✅ 100% | ✅ 100% | SNMP v1/v2c/v3 operations |
| **tnm::mib** | ✅ 100% | ✅ 100% | MIB database operations |
| **tnm::dns** | ❌ 0% | ✅ 100% | DNS queries (CRASHES ON WINDOWS) |
| **tnm::ntp** | ❌ 0% | ✅ 100% | NTP time queries (CRASHES ON WINDOWS) |

---

## tnm::syslog

**Purpose**: Send messages to system logging facility

| Sub-command | Windows | Linux | Description | Tcl Example |
|-------------|---------|-------|-------------|-------------|
| `emergency <msg>` | ✅ | ✅ | System is unusable | `tnm::syslog emergency "Disk failure"` |
| `alert <msg>` | ✅ | ✅ | Action must be taken immediately | `tnm::syslog alert "Database corruption"` |
| `critical <msg>` | ✅ | ✅ | Critical conditions | `tnm::syslog critical "Temperature too high"` |
| `error <msg>` | ✅ | ✅ | Error conditions | `tnm::syslog error "Failed to connect"` |
| `warning <msg>` | ✅ | ✅ | Warning conditions | `tnm::syslog warning "Low disk space"` |
| `notice <msg>` | ✅ | ✅ | Normal but significant | `tnm::syslog notice "Service started"` |
| `info <msg>` | ✅ | ✅ | Informational messages | `tnm::syslog info "User login"` |
| `debug <msg>` | ✅ | ✅ | Debug-level messages | `tnm::syslog debug "Variable x=42"` |

**Options**: `-ident <string>` (set program name), `-facility <name>` (set facility: daemon, user, local0-7)

**Notes**: 100% functional on both platforms. Windows uses internal logging, Linux uses native syslog.

---

## tnm::map

**Purpose**: Create and manage network topology maps

| Sub-command | Windows | Linux | Description | Tcl Example |
|-------------|---------|-------|-------------|-------------|
| `create` | ✅ | ✅ | Create new map object | `set m [tnm::map create]` |
| `find ?pattern?` | ✅ | ✅ | Find maps matching pattern | `tnm::map find *` |
| `info ?option?` | ✅ | ✅ | Get map information | `tnm::map info class` |
| `$map configure` | ✅ | ✅ | Configure map attributes | `$m configure -name "Network1"` |
| `$map cget` | ✅ | ✅ | Get map attribute | `$m cget -name` |
| `$map destroy` | ✅ | ✅ | Destroy map object | `$m destroy` |

**Options**: `-name <string>`, `-tags <list>`, `-command <script>`

**Notes**: 100% functional. Core object management for network maps.

---

## tnm::icmp

**Purpose**: Send ICMP packets for network diagnostics

| Sub-command | Windows | Linux | Description | Tcl Example |
|-------------|---------|-------|-------------|-------------|
| `echo <hosts>` | ✅ | ✅ | Send ICMP echo (ping) | `tnm::icmp echo 192.168.1.1` |
| `mask <hosts>` | ✂️ | ✅ | Request address mask | `tnm::icmp mask 10.0.0.1` |
| `timestamp <hosts>` | ✂️ | ✅ | Request timestamp | `tnm::icmp timestamp 10.0.0.1` |
| `ttl <hop> <hosts>` | ✅ | ✅ | Set TTL for echo | `tnm::icmp ttl 5 192.168.1.1` |
| `trace <hop> <hosts>` | ✅ | ✅ | Traceroute functionality | `tnm::icmp trace 10 google.com` |

**Options**: `-timeout <ms>`, `-retries <n>`, `-size <bytes>`, `-delay <ms>` (⚠️ Not supported on Windows), `-window <n>` (⚠️ Different behavior on Windows)

**Notes**: Windows 100% functional for available commands. `mask` and `timestamp` removed from Windows builds due to ICMP.DLL limitations. Echo and trace work reliably. Linux 100% functional with all commands.

---

## tnm::udp

**Purpose**: Create UDP sockets for datagram communication

| Sub-command | Windows | Linux | Description | Tcl Example |
|-------------|---------|-------|-------------|-------------|
| `create` | ✅ | ✅ | Create UDP socket | `set sock [tnm::udp create]` |
| `$sock send <data>` | ✅ | ✅ | Send UDP datagram | `$sock send "Hello"` |
| `$sock receive` | ✅ | ✅ | Receive UDP datagram | `set data [$sock receive]` |
| `$sock connect <host> <port>` | ✅ | ✅ | Connect to remote endpoint | `$sock connect 10.0.0.1 5000` |
| `$sock configure` | ✅ | ✅ | Configure socket options | `$sock configure -port 8888` |
| `$sock cget` | ✅ | ✅ | Get socket option | `$sock cget -myport` |
| `$sock bind` | ✅ | ✅ | Bind callback to socket | `$sock bind read HandleData` |
| `$sock destroy` | ✅ | ✅ | Destroy socket | `$sock destroy` |

**Options**: `-address <host>`, `-port <n>`, `-myaddress <host>`, `-myport <n>`, `-read <cmd>`, `-write <cmd>`, `-tags <list>`

**Notes**: 100% functional. Fixed socket creation bug where -myport option failed to initialize sin_family on Windows. All 36 UDP tests pass reliably on both platforms.

---

## tnm::job

**Purpose**: Schedule and manage periodic background jobs

| Sub-command | Windows | Linux | Description | Tcl Example |
|-------------|---------|-------|-------------|-------------|
| `create` | ✅ | ✅ | Create job object | `set j [tnm::job create -interval 5000]` |
| `find ?pattern?` | ✅ | ✅ | Find jobs matching pattern | `tnm::job find *` |
| `current` | ✅ | ✅ | Get current job handle | `tnm::job current` |
| `schedule <ms> <script>` | ✅ | ✅ | One-time scheduled execution | `tnm::job schedule 1000 {puts "Hi"}` |
| `wait` | ✅ | ✅ | Wait for job completion | `tnm::job wait $j` |
| `$job configure` | ✅ | ✅ | Configure job attributes | `$j configure -interval 10000` |
| `$job cget` | ✅ | ✅ | Get job attribute | `$j cget -status` |
| `$job destroy` | ✅ | ✅ | Destroy job | `$j destroy` |

**Options**: `-interval <ms>`, `-iterations <n>`, `-status <state>`, `-command <script>`, `-exit <script>`, `-error <script>`, `-tags <list>`

**Notes**: 91% functional. Works reliably but has sub-interpreter limitations (cannot create jobs in child interpreters).

---

## tnm::netdb

**Purpose**: Query network database information (hosts, services, protocols)

| Sub-command | Windows | Linux | Description | Tcl Example |
|-------------|---------|-------|-------------|-------------|
| `hosts address <name>` | ✅ | ✅ | Get IP address from hostname | `tnm::netdb hosts address localhost` |
| `hosts name <addr>` | ✅ | ✅ | Get hostname from IP | `tnm::netdb hosts name 127.0.0.1` |
| `hosts aliases <arg>` | ✅ | ✅ | Get host aliases | `tnm::netdb hosts aliases localhost` |
| `services name <num> <proto>` | ✅ | ✅ | Get service name | `tnm::netdb services name 80 tcp` |
| `services number <name> <proto>` | ✅ | ✅ | Get service port | `tnm::netdb services number http tcp` |
| `services aliases <arg> <proto>` | ✅ | ✅ | Get service aliases | `tnm::netdb services aliases http tcp` |
| `protocols name <num>` | ✅ | ✅ | Get protocol name | `tnm::netdb protocols name 6` |
| `protocols number <name>` | ✅ | ✅ | Get protocol number | `tnm::netdb protocols number tcp` |
| `protocols aliases <arg>` | ✅ | ✅ | Get protocol aliases | `tnm::netdb protocols aliases tcp` |
| `networks name <addr>` | ⚠️ | ✅ | Get network name | `tnm::netdb networks name 10.0.0.0` |
| `networks address <name>` | ⚠️ | ✅ | Get network address | `tnm::netdb networks address loopback` |
| `networks aliases <arg>` | ⚠️ | ✅ | Get network aliases | `tnm::netdb networks aliases loopback` |
| `ip class <addr>` | ✅ | ✅ | Get IP address class | `tnm::netdb ip class 192.168.1.1` |
| `ip range <addr>` | ✅ | ✅ | Get address range | `tnm::netdb ip range 10.0.0.0/24` |
| `ip apply <range> <script>` | ✅ | ✅ | Apply script to range | `tnm::netdb ip apply 192.168.1.0/24 {puts}` |
| `ip broadcast <addr>` | ✅ | ✅ | Get broadcast address | `tnm::netdb ip broadcast 192.168.1.0/24` |
| `ip compare <a1> <a2>` | ✅ | ✅ | Compare addresses | `tnm::netdb ip compare 10.0.0.1 10.0.0.2` |

**Notes**: 100% functional for available commands on both platforms. Networks queries limited on Windows. `sunrpcs` command removed from all platforms (RPC support disabled).

---

## tnm::snmp

**Purpose**: SNMP protocol operations (v1, v2c, v3)

| Sub-command | Windows | Linux | Description | Tcl Example |
|-------------|---------|-------|-------------|-------------|
| `generator` | ✅ | ✅ | Create SNMP session | `set s [tnm::snmp generator -address 10.0.0.1]` |
| `listener` | ✅ | ✅ | Create trap listener | `set l [tnm::snmp listener -port 162]` |
| `responder` | ✅ | ✅ | Create SNMP responder | `set r [tnm::snmp responder]` |
| `notifier` | ✅ | ✅ | Send notifications | `set n [tnm::snmp notifier]` |
| `alias <name> <oid>` | ✅ | ✅ | Create OID alias | `tnm::snmp alias sysDescr 1.3.6.1.2.1.1.1.0` |
| `find ?pattern?` | ✅ | ✅ | Find SNMP sessions | `tnm::snmp find *` |
| `info ?option?` | ✅ | ✅ | Get session info | `tnm::snmp info class` |
| `wait` | ✅ | ✅ | Wait for requests | `tnm::snmp wait` |
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

## tnm::mib

**Purpose**: MIB (Management Information Base) operations

| Sub-command | Windows | Linux | Description | Tcl Example |
|-------------|---------|-------|-------------|-------------|
| `access <oid>` | ✅ | ✅ | Get MIB object access | `tnm::mib access sysDescr` |
| `children <oid>` | ✅ | ✅ | Get child nodes | `tnm::mib children system` |
| `compare <o1> <o2>` | ✅ | ✅ | Compare OIDs | `tnm::mib compare 1.3.6.1.2.1.1 1.3.6.1.2.1.2` |
| `defval <oid>` | ✅ | ✅ | Get default value | `tnm::mib defval sysContact` |
| `description <oid>` | ✅ | ✅ | Get object description | `tnm::mib description sysDescr` |
| `displayhint <oid>` | ✅ | ✅ | Get display hint | `tnm::mib displayhint ifPhysAddress` |
| `enums <oid>` | ✅ | ✅ | Get enumeration values | `tnm::mib enums ifAdminStatus` |
| `exists <oid>` | ✅ | ✅ | Check if OID exists | `tnm::mib exists sysDescr` |
| `file <module>` | ✅ | ✅ | Get MIB file path | `tnm::mib file SNMPv2-MIB` |
| `format <oid> <val>` | ✅ | ✅ | Format value | `tnm::mib format sysDescr "text"` |
| `index <oid>` | ✅ | ✅ | Get index information | `tnm::mib index ifEntry` |
| `info <option>` | ✅ | ✅ | Get MIB info | `tnm::mib info loaded` |
| `label <oid>` | ✅ | ✅ | Get object label | `tnm::mib label 1.3.6.1.2.1.1.1.0` |
| `length <oid>` | ✅ | ✅ | Get object length | `tnm::mib length sysDescr` |
| `load <file>` | ✅ | ✅ | Load MIB file | `tnm::mib load IF-MIB.txt` |
| `macro <name>` | ✅ | ✅ | Get macro definition | `tnm::mib macro MODULE-IDENTITY` |
| `member <oid> <val>` | ✅ | ✅ | Get member name | `tnm::mib member ifAdminStatus 1` |
| `module <oid>` | ✅ | ✅ | Get module name | `tnm::mib module sysDescr` |
| `name <oid>` | ✅ | ✅ | Get fully qualified name | `tnm::mib name 1.3.6.1.2.1.1.1.0` |
| `oid <name>` | ✅ | ✅ | Get OID from name | `tnm::mib oid sysDescr.0` |
| `pack <oid> <idx>` | ✅ | ✅ | Pack index values | `tnm::mib pack ifEntry.1 5` |
| `parent <oid>` | ✅ | ✅ | Get parent OID | `tnm::mib parent sysDescr` |
| `range <oid>` | ✅ | ✅ | Get value range | `tnm::mib range ifMtu` |
| `scan <oid> <inst>` | ✅ | ✅ | Scan instance OID | `tnm::mib scan sysDescr.0 var` |
| `size <oid>` | ✅ | ✅ | Get object size | `tnm::mib size sysDescr` |
| `split <oid>` | ✅ | ✅ | Split into base and instance | `tnm::mib split sysDescr.0 base inst` |
| `status <oid>` | ✅ | ✅ | Get object status | `tnm::mib status sysDescr` |
| `subtree <oid>` | ✅ | ✅ | Get entire subtree | `tnm::mib subtree system` |
| `syntax <oid>` | ✅ | ✅ | Get object syntax | `tnm::mib syntax sysUpTime` |
| `type <oid>` | ✅ | ✅ | Get object type | `tnm::mib type ifTable` |
| `unpack <oid> <inst>` | ✅ | ✅ | Unpack instance values | `tnm::mib unpack ifEntry.1.5 var` |
| `variables <oid>` | ✅ | ✅ | Get table variables | `tnm::mib variables ifEntry` |
| `walk <oid> <script>` | ✅ | ✅ | Walk MIB tree | `tnm::mib walk system {puts}` |

**Notes**: 100% functional on both platforms. Complete MIB database operations with 33 sub-commands.

---

## tnm::dns

**Purpose**: DNS (Domain Name System) queries

| Sub-command | Windows | Linux | Description | Tcl Example |
|-------------|---------|-------|-------------|-------------|
| `address <name>` | ❌ | ✅ | Query A records | `tnm::dns address www.google.com` |
| `name <addr>` | ❌ | ✅ | Query PTR records | `tnm::dns name 8.8.8.8` |
| `hinfo <name>` | ❌ | ✅ | Query HINFO records | `tnm::dns hinfo host.example.com` |
| `mx <name>` | ❌ | ✅ | Query MX records | `tnm::dns mx example.com` |
| `soa <name>` | ❌ | ✅ | Query SOA records | `tnm::dns soa example.com` |
| `txt <name>` | ❌ | ✅ | Query TXT records | `tnm::dns txt example.com` |
| `cname <name>` | ❌ | ✅ | Query CNAME records | `tnm::dns cname www.example.com` |
| `ns <name>` | ❌ | ✅ | Query NS records | `tnm::dns ns example.com` |

**Options**: `-timeout <ms>`, `-retries <n>`, `-server <addr>`

**Notes**: ❌ **CRASHES ON WINDOWS** with segmentation fault. Root cause: Incomplete `_res` structure in MinGW64 headers. Linux 100% functional. **DO NOT USE ON WINDOWS**.

---

## tnm::ntp

**Purpose**: NTP (Network Time Protocol) queries

| Sub-command | Windows | Linux | Description | Tcl Example |
|-------------|---------|-------|-------------|-------------|
| `<server> <arrayvar>` | ❌ | ✅ | Query NTP server | `tnm::ntp pool.ntp.org result` |

**Options**: `-timeout <ms>`, `-retries <n>`

**Notes**: ❌ **CRASHES ON WINDOWS** due to resolver dependency issues. Linux functional. Returns time synchronization data in array variable. **DO NOT USE ON WINDOWS**.

---

## Platform-Specific Notes

### Windows (MinGW64)

**Working Well (7 commands):**
- tnm::syslog, tnm::map, tnm::udp, tnm::job, tnm::snmp, tnm::mib, tnm::icmp (partial)

**Limited Functionality (1 command):**
- tnm::netdb: sunrpcs disabled, networks queries limited
- tnm::icmp: mask and timestamp commands fail (ICMP.DLL limitations)

**Broken/Unsafe (2 commands):**
- tnm::dns: Segmentation fault due to incomplete _res structure in MinGW64 headers
- tnm::ntp: Crashes due to resolver dependencies

**Recommendation**: Use tnm::netdb hosts for DNS lookups instead of tnm::dns on Windows.

### Linux

**All core commands fully functional** except:
- tnm::smx: Untested (requires SMX framework)
- tnm::ined: GUI-specific (requires Tkined)

---

## Quick Start Examples

### SNMP Device Query
```tcl
package require tnm 3.1.3

# Query system information
set s [tnm::snmp generator -address 192.168.1.1 -read public]
set vbl [$s get sysDescr.0 sysUpTime.0 sysContact.0]
foreach vb $vbl {
    puts "[lindex $vb 0]: [lindex $vb 2]"
}
$s destroy
```

### ICMP Network Sweep
```tcl
package require tnm 3.1.3

# Ping multiple hosts
set hosts {192.168.1.1 192.168.1.2 192.168.1.3}
set results [tnm::icmp echo $hosts]
foreach result $results {
    puts "Host [lindex $result 0]: RTT [lindex $result 1]ms"
}
```

### Periodic Monitoring Job
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

### MIB Tree Walk
```tcl
package require tnm 3.1.3

# Walk system subtree
tnm::mib walk system oidVar labelVar {
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
