# tnm::udp - UDP Datagram Commands

Send and receive UDP datagrams.

**Status:** ✅ 100% functional on Windows and Linux

---

## Synopsis

```tcl
tnm::udp create [options]
tnm::udp find [options]

$udp send [host port] message
$udp receive
$udp connect host port
$udp configure [options]
$udp cget option
$udp bind event script
$udp destroy
```

---

## Description

The `tnm::udp` command provides UDP (User Datagram Protocol) socket communication capabilities. It supports both connected and unconnected modes, synchronous and event-driven operation.

---

## Creating UDP Endpoints

### tnm::udp create [options]

Create a new UDP endpoint.

```tcl
# Create with automatic port
set u [tnm::udp create]

# Create with specific local port
set u [tnm::udp create -myport 5000]

# Create with local address binding
set u [tnm::udp create -myaddress 192.168.1.1 -myport 5000]
```

### tnm::udp find [options]

Find existing UDP endpoints.

```tcl
# Find all endpoints
set endpoints [tnm::udp find]

# Find by tags
set endpoints [tnm::udp find -tags "server"]
```

---

## UDP Instance Commands

### $udp send [host port] message

Send a UDP datagram.

**Unconnected mode:**
```tcl
$u send 192.168.1.1 5001 "Hello, World!"
$u send 10.0.0.1 8080 $binaryData
```

**Connected mode:**
```tcl
$u connect 192.168.1.1 5001
$u send "Hello, World!"
```

### $udp receive

Receive a UDP datagram (blocking).

```tcl
set data [$u receive]
# Returns: {address port message}

lassign $data addr port msg
puts "Received from $addr:$port: $msg"
```

### $udp connect host port

Connect the endpoint to a remote address.

```tcl
$u connect 192.168.1.1 5001
```

After connecting:
- `send` no longer requires host/port arguments
- Only datagrams from the connected address are received

### $udp configure [options]

Configure endpoint options.

```tcl
$u configure -tags "myserver"
$u configure -read {handleRead %S}
```

### $udp cget option

Get an option value.

```tcl
set port [$u cget -myport]
set addr [$u cget -myaddress]
```

### $udp bind event script

Bind a callback script to events. *(Alternative to -read/-write options)*

```tcl
$u bind readable {
    set data [%S receive]
    puts "Got: $data"
}
```

### $udp destroy

Destroy the UDP endpoint.

```tcl
$u destroy
```

---

## Options

| Option | Description |
|--------|-------------|
| `-address addr` | Remote IP address |
| `-port port` | Remote port number |
| `-myaddress addr` | Local IP address to bind |
| `-myport port` | Local port number to bind |
| `-read command` | Callback when data is available |
| `-write command` | Callback when ready to write |
| `-tags tagList` | Tags for grouping endpoints |

---

## Event-Driven Operation

For non-blocking operation, use the `-read` callback:

```tcl
set u [tnm::udp create -myport 5000]

$u configure -read {
    set data [%S receive]
    lassign $data addr port msg
    puts "From $addr:$port -> $msg"

    # Echo back
    %S send $addr $port "Echo: $msg"
}

# Enter event loop
vwait forever
```

### Callback Escapes

| Escape | Description |
|--------|-------------|
| `%S` | The UDP endpoint command name |

---

## Examples

### Simple UDP Client

```tcl
package require tnm 3.1.3

# Create UDP socket
set u [tnm::udp create]

# Send message
$u send 192.168.1.1 5000 "Hello, Server!"

# Wait for response (with timeout using after)
set response ""
after 5000 {set response "TIMEOUT"}

$u configure -read {
    set ::response [%S receive]
}

vwait response

if {$response eq "TIMEOUT"} {
    puts "No response received"
} else {
    lassign $response addr port msg
    puts "Response: $msg"
}

$u destroy
```

### UDP Echo Server

```tcl
package require tnm 3.1.3

set port 5000
set u [tnm::udp create -myport $port]

puts "UDP Echo Server listening on port $port"

$u configure -read {
    set data [%S receive]
    lassign $data addr port msg

    puts "Received from $addr:$port: $msg"

    # Send echo response
    %S send $addr $port "Echo: $msg"
}

vwait forever
```

### Connected UDP Communication

```tcl
package require tnm 3.1.3

set u [tnm::udp create]

# Connect to remote endpoint
$u connect 192.168.1.1 5000

# Now can send without specifying address
$u send "First message"
$u send "Second message"

# Receive (blocking)
set response [$u receive]
puts "Response: [lindex $response 2]"

$u destroy
```

### UDP Multicast (if supported)

```tcl
package require tnm 3.1.3

set u [tnm::udp create]

# Join multicast group
$u join 224.0.0.1 5000

$u configure -read {
    set data [%S receive]
    puts "Multicast: [lindex $data 2]"
}

vwait forever
```

### Broadcast

```tcl
package require tnm 3.1.3

set u [tnm::udp create]

# Send broadcast
$u send 255.255.255.255 5000 "Broadcast message"

# Or to subnet broadcast
$u send 192.168.1.255 5000 "Subnet broadcast"

$u destroy
```

---

## Error Handling

```tcl
if {[catch {$u send 192.168.1.1 5000 "test"} err]} {
    puts "Send failed: $err"
}

if {[catch {$u receive} data]} {
    puts "Receive failed: $data"
}
```

---

## Notes

- UDP is connectionless and unreliable - messages may be lost, duplicated, or arrive out of order
- Maximum UDP datagram size is typically ~65507 bytes (65535 - 20 IP header - 8 UDP header)
- For reliable communication, consider using TCP or implementing acknowledgment logic
- The `receive` command blocks until data arrives; use `-read` callback for non-blocking operation

---

## See Also

- [tnm.md](tnm.md) - Main TNM documentation
- RFC 768 - User Datagram Protocol
