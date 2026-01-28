# tnm::syslog - System Logging Commands

Write messages to the system logging subsystem.

**Status:** ✅ 100% functional on Windows and Linux

---

## Synopsis

```tcl
tnm::syslog [options]
tnm::syslog [options] level message
```

---

## Description

The `tnm::syslog` command writes messages to the local system logging subsystem. On Unix/Linux platforms, messages are written to the syslog daemon. On Windows platforms, messages are written to the Windows Event Log.

The command automatically converts priority levels and facilities to formats understood by the local logging subsystem when needed.

---

## Commands

### tnm::syslog [options]

Query or set default syslog options.

```tcl
# Get current ident
set ident [tnm::syslog -ident]

# Set default ident and facility
tnm::syslog -ident "myapp" -facility daemon
```

### tnm::syslog [options] level message

Write a message to the system log.

```tcl
tnm::syslog info "Application started"
tnm::syslog warning "Connection timeout detected"
tnm::syslog error "Failed to connect to database"
```

---

## Options

| Option | Default | Description |
|--------|---------|-------------|
| `-ident string` | scotty | Identification string for the event source |
| `-facility facility` | local0 | Facility category for the message |

---

## Priority Levels

Messages must include one of the following priority levels:

| Level | Description |
|-------|-------------|
| `emergency` | A panic condition. Normally broadcast to all users. |
| `alert` | A condition requiring immediate correction (e.g., corrupted database). |
| `critical` | Critical conditions such as hard device errors. |
| `error` | Error messages. |
| `warning` | Warning messages. |
| `notice` | Conditions that may require special handling but are not errors. |
| `info` | Informational messages. |
| `debug` | Messages useful only when debugging a program. |

---

## Facilities

The following facilities are supported:

### System Facilities

| Facility | Description |
|----------|-------------|
| `kernel` | Kernel messages |
| `user` | Random user process messages |
| `mail` | Mail subsystem messages |
| `daemon` | System daemon messages |
| `auth` | Security/authorization messages |
| `syslog` | Syslog internal messages |
| `lpr` | Printing subsystem messages |
| `news` | Network news subsystem messages |
| `uucp` | UUCP subsystem messages |
| `cron` | Clock (cron/at) subsystem messages |
| `authpriv` | Security/authorization messages (private) |
| `ftp` | FTP daemon messages |
| `ntp` | NTP daemon messages |
| `audit` | Auditing subsystem messages (private) |
| `alert` | Alert subsystem messages (private) |
| `cronpriv` | Clock subsystem messages (private) |

### Local Use Facilities

| Facility | Description |
|----------|-------------|
| `local0` | Local use messages type 0 (default) |
| `local1` | Local use messages type 1 |
| `local2` | Local use messages type 2 |
| `local3` | Local use messages type 3 |
| `local4` | Local use messages type 4 |
| `local5` | Local use messages type 5 |
| `local6` | Local use messages type 6 |
| `local7` | Local use messages type 7 |

---

## Examples

### Basic Logging

```tcl
package require tnm 3.1.3

# Simple log messages
tnm::syslog info "Application started successfully"
tnm::syslog warning "Configuration file not found, using defaults"
tnm::syslog error "Failed to open database connection"
```

### Custom Ident and Facility

```tcl
package require tnm 3.1.3

# Set application identity
tnm::syslog -ident "network-monitor" -facility daemon

# Log with custom settings
tnm::syslog info "Monitoring service started"
tnm::syslog notice "Scanning network 192.168.1.0/24"
```

### Logging Wrapper Procedure

```tcl
package require tnm 3.1.3

# Initialize logging
tnm::syslog -ident "myapp" -facility local0

proc log {level msg} {
    set timestamp [clock format [clock seconds] -format "%Y-%m-%d %H:%M:%S"]
    puts stderr "\[$timestamp\] \[$level\] $msg"
    tnm::syslog $level $msg
}

proc logInfo {msg} { log info $msg }
proc logWarning {msg} { log warning $msg }
proc logError {msg} { log error $msg }
proc logDebug {msg} { log debug $msg }

# Usage
logInfo "Starting application"
logWarning "Memory usage above 80%"
logError "Connection failed"
```

### SNMP Event Logging

```tcl
package require tnm 3.1.3

tnm::syslog -ident "snmp-monitor" -facility daemon

proc logSnmpEvent {host oid status value} {
    if {$status eq "noError"} {
        tnm::syslog info "SNMP GET $host $oid = $value"
    } else {
        tnm::syslog warning "SNMP ERROR $host $oid: $status"
    }
}

# Query and log
set s [tnm::snmp generator -address 192.168.1.1 -read public]
set result [$s get {sysDescr.0}]
lassign [lindex $result 0] oid type value
logSnmpEvent 192.168.1.1 $oid "noError" $value
$s destroy
```

### Monitor with Syslog Alerts

```tcl
package require tnm 3.1.3

tnm::syslog -ident "icmp-monitor" -facility local0

proc checkHost {host} {
    set result [tnm::icmp -timeout 2000 echo $host]
    lassign [lindex $result 0] addr rtt

    if {$rtt eq ""} {
        tnm::syslog alert "Host $host is UNREACHABLE"
        return 0
    } else {
        if {$rtt > 100} {
            tnm::syslog warning "Host $host high latency: ${rtt}ms"
        }
        return 1
    }
}

# Monitor hosts
set hosts {192.168.1.1 192.168.1.2 192.168.1.3}

foreach host $hosts {
    checkHost $host
}
```

### Structured Logging

```tcl
package require tnm 3.1.3

tnm::syslog -ident "structured-logger" -facility daemon

proc structuredLog {level data} {
    # Convert dict to key=value format
    set parts {}
    dict for {key value} $data {
        # Escape quotes in value
        set value [string map {\" \\\"} $value]
        lappend parts "$key=\"$value\""
    }
    set msg [join $parts " "]
    tnm::syslog $level $msg
}

# Usage
structuredLog info {
    event "user_login"
    user "admin"
    ip "192.168.1.100"
    status "success"
}

structuredLog warning {
    event "high_cpu"
    host "server1"
    cpu_percent "95"
    threshold "90"
}
```

### Different Facilities for Different Components

```tcl
package require tnm 3.1.3

proc logAuth {level msg} {
    tnm::syslog -facility auth $level $msg
}

proc logDaemon {level msg} {
    tnm::syslog -facility daemon $level $msg
}

proc logLocal {level msg} {
    tnm::syslog -facility local0 $level $msg
}

# Authentication events go to auth facility
logAuth info "User admin logged in"
logAuth warning "Failed login attempt for user root"

# Daemon messages
logDaemon info "Service started on port 8080"

# Application-specific messages
logLocal info "Processing batch job #12345"
```

---

## Platform Notes

### Linux/Unix

- Messages are sent to the syslog daemon (typically rsyslog or syslog-ng)
- Check `/var/log/syslog`, `/var/log/messages`, or facility-specific logs
- Configure rsyslog.conf to route messages based on facility and priority
- Use `journalctl` on systemd systems to view logs

Example rsyslog configuration to capture local0 messages:
```
local0.*    /var/log/myapp.log
```

### Windows

- Messages are written to the Windows Event Log
- View with Event Viewer (eventvwr.msc)
- Look under "Windows Logs" > "Application"
- Priority levels are mapped to Windows event types:
  - emergency, alert, critical, error -> Error
  - warning -> Warning
  - notice, info, debug -> Information

---

## Error Handling

```tcl
if {[catch {tnm::syslog error "Critical failure occurred"} err]} {
    # Fallback to stderr if syslog fails
    puts stderr "Failed to write to syslog: $err"
    puts stderr "Original message: Critical failure occurred"
}
```

---

## Notes

- All syslog priority levels are supported
- Not all systems support all facilities; automatic conversion occurs
- Default ident is "scotty" (the name of the original tool)
- Default facility is "local0" (reserved for local use)
- Messages are typically limited in length by the syslog implementation
- On Windows, long messages may be truncated

---

## See Also

- [tnm.md](tnm.md) - Main TNM documentation
- syslog(3) - Unix syslog documentation
- RFC 5424 - The Syslog Protocol
