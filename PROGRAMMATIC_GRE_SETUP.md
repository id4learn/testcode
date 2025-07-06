# Programmatic GRE Tunnel Setup

This directory contains two programmatic implementations for creating GRE tunnels on Linux:

1. **C Program** (`gre_tunnel.c`) - Uses netlink sockets directly
2. **Python Program** (`gre_tunnel.py`) - Uses pyroute2 library

Both programs use the **10.x.x.x** network range to avoid conflicts with common LAN setups that use 192.168.x.x.

## Quick Start

### Python Version (Recommended)

```bash
# Install dependencies
sudo python3 gre_tunnel.py install-deps

# Create tunnel with default settings
sudo python3 gre_tunnel.py

# Create tunnel with custom endpoints
sudo python3 gre_tunnel.py 10.1.1.10 10.1.1.20 10.10.1.1

# Check tunnel status
sudo python3 gre_tunnel.py status

# Remove tunnel
sudo python3 gre_tunnel.py remove
```

### C Version

```bash
# Compile the program
make

# Create tunnel with default settings
sudo ./gre_tunnel

# Create tunnel with custom endpoints
sudo ./gre_tunnel 10.1.1.10 10.1.1.20 10.10.1.1
```

## Default Configuration

| Parameter | Default Value | Description |
|-----------|---------------|-------------|
| Tunnel Name | `gre1` | Name of the GRE interface |
| Local Endpoint | `10.1.1.10` | Your machine's IP address |
| Remote Endpoint | `10.1.1.20` | Remote machine's IP address |
| Tunnel Local IP | `10.10.1.1/24` | IP for your tunnel endpoint |
| Tunnel Remote IP | `10.10.1.2` | IP for remote tunnel endpoint |
| TTL | `64` | Time to live for GRE packets |

## Network Ranges Used

The programs use the following IP ranges to avoid conflicts:

- **10.1.1.x** - For physical network endpoints
- **10.10.1.x** - For tunnel network addresses

This avoids the common 192.168.x.x range used by most home/office LANs.

## Features

### Python Program Features
- ✅ Automatic dependency installation
- ✅ Clean error handling with detailed messages
- ✅ Tunnel status display
- ✅ Easy to modify and extend
- ✅ Cross-platform compatibility
- ✅ Built-in connectivity testing

### C Program Features
- ✅ No external dependencies
- ✅ Direct netlink socket communication
- ✅ Minimal resource usage
- ✅ Fast execution
- ✅ Educational value for understanding netlink

## Installation

### Python Version Dependencies

```bash
# Install pyroute2 library
pip3 install pyroute2

# Or use the built-in installer
python3 gre_tunnel.py install-deps
```

### C Version Dependencies

```bash
# Install build tools (if not already installed)
sudo apt-get install build-essential

# Compile
make
```

## Usage Examples

### Example 1: Simple Tunnel Creation

```bash
# Python
sudo python3 gre_tunnel.py

# C
sudo ./gre_tunnel
```

### Example 2: Custom IP Addresses

```bash
# Create tunnel between 10.5.5.10 and 10.5.5.20
# with tunnel IPs 10.20.1.1 and 10.20.1.2

# Python
sudo python3 gre_tunnel.py 10.5.5.10 10.5.5.20 10.20.1.1

# C
sudo ./gre_tunnel 10.5.5.10 10.5.5.20 10.20.1.1
```

### Example 3: Tunnel Management

```bash
# Check tunnel status
sudo python3 gre_tunnel.py status

# Remove tunnel
sudo python3 gre_tunnel.py remove
```

## Complete Two-Machine Setup

### Machine A (10.1.1.10)
```bash
# Create tunnel
sudo python3 gre_tunnel.py 10.1.1.10 10.1.1.20 10.10.1.1

# Verify
ping 10.10.1.2
```

### Machine B (10.1.1.20)
```bash
# Create tunnel (note reversed endpoints)
sudo python3 gre_tunnel.py 10.1.1.20 10.1.1.10 10.10.1.2

# Verify
ping 10.10.1.1
```

## Troubleshooting

### Common Issues

1. **"Must be run as root"**
   - Solution: Use `sudo` when running the programs

2. **"pyroute2 library not found"**
   - Solution: Install with `pip3 install pyroute2`

3. **"Failed to create GRE tunnel"**
   - Check if tunnel name already exists
   - Verify IP addresses are valid
   - Ensure GRE protocol is not blocked by firewall

4. **"Cannot reach remote endpoint"**
   - This is normal if remote side is not configured
   - Set up both ends of the tunnel

### Debug Commands

```bash
# Check if tunnel exists
ip link show gre1

# Check tunnel configuration
ip tunnel show gre1

# Check IP addresses
ip addr show gre1

# Check routes
ip route show

# Check if GRE is blocked
sudo iptables -L | grep -i gre
```

## Advanced Configuration

### Firewall Rules

Ensure GRE protocol (IP protocol 47) is allowed:

```bash
# Allow GRE traffic
sudo iptables -A INPUT -p gre -j ACCEPT
sudo iptables -A OUTPUT -p gre -j ACCEPT
```

### Persistent Configuration

To make tunnels persistent across reboots, add the commands to your network configuration:

**Ubuntu/Debian:**
```bash
# Add to /etc/network/interfaces
auto gre1
iface gre1 inet static
    address 10.10.1.1
    netmask 255.255.255.0
    pre-up ip tunnel add gre1 mode gre local 10.1.1.10 remote 10.1.1.20 ttl 64
    post-down ip tunnel del gre1
```

**SystemD:**
```bash
# Create /etc/systemd/system/gre-tunnel.service
[Unit]
Description=GRE Tunnel
After=network.target

[Service]
Type=oneshot
ExecStart=/path/to/gre_tunnel.py
RemainAfterExit=true

[Install]
WantedBy=multi-user.target
```

## Program Architecture

### Python Program Structure
```
gre_tunnel.py
├── GREConfig class - Configuration management
├── GRETunnelCreator class - Main tunnel operations
│   ├── create_tunnel() - Create GRE interface
│   ├── assign_ip_address() - Assign IP to interface
│   ├── bring_interface_up() - Activate interface
│   ├── show_tunnel_info() - Display tunnel status
│   └── test_connectivity() - Test tunnel connectivity
└── main() - Command-line interface
```

### C Program Structure
```
gre_tunnel.c
├── gre_config struct - Configuration storage
├── send_netlink_msg() - Send netlink messages
├── recv_netlink_msg() - Receive netlink responses
├── create_gre_tunnel() - Create GRE interface
├── assign_ip_address() - Assign IP to interface
└── bring_interface_up() - Activate interface
```

## Security Considerations

1. **Root Privileges**: Both programs require root access to create network interfaces
2. **Firewall**: Ensure GRE protocol is properly configured in firewall rules
3. **Network Isolation**: Consider using VPN or other security measures for public networks
4. **Monitoring**: Monitor tunnel traffic for security purposes

## Performance Notes

- **C Program**: Minimal overhead, fastest execution
- **Python Program**: Slightly higher overhead but more features
- **GRE Protocol**: Low overhead tunneling protocol
- **MTU**: Default MTU is typically 1476 bytes (1500 - 24 byte GRE header)

## Contributing

To extend these programs:

1. **Python**: Modify the `GRETunnelCreator` class
2. **C**: Add new functions following the existing netlink pattern
3. **Testing**: Use the provided test frameworks
4. **Documentation**: Update this README with new features