# GRE Tunnel Setup Guide

This guide explains how to create a GRE (Generic Routing Encapsulation) tunnel on Linux.

## Prerequisites

- Linux system with root/sudo privileges
- `iproute2` package installed (provides the `ip` command)
- Network connectivity between the local and remote endpoints

## What is a GRE Tunnel?

A GRE tunnel creates a virtual point-to-point connection between two network endpoints. It encapsulates packets inside IP packets, allowing you to create a logical network connection over an existing IP network.

## Quick Start

1. **Edit the configuration** in `create_gre_tunnel.sh`:
   ```bash
   LOCAL_IP="192.168.1.10"        # Your machine's IP address
   REMOTE_IP="192.168.1.20"       # Remote machine's IP address
   TUNNEL_LOCAL_IP="10.0.0.1/24"  # IP for your tunnel endpoint
   TUNNEL_REMOTE_IP="10.0.0.2"    # IP for remote tunnel endpoint
   ```

2. **Run the script**:
   ```bash
   ./create_gre_tunnel.sh
   ```

## Manual Commands

If you prefer to run the commands manually:

### 1. Create the GRE tunnel
```bash
sudo ip tunnel add gre1 mode gre remote 192.168.1.20 local 192.168.1.10 ttl 64
```

### 2. Assign IP address to the tunnel
```bash
sudo ip addr add 10.0.0.1/24 dev gre1
```

### 3. Bring the interface up
```bash
sudo ip link set gre1 up
```

### 4. Verify the configuration
```bash
ip addr show gre1
ip tunnel show gre1
```

## Script Usage

The script supports multiple operations:

- `./create_gre_tunnel.sh create` - Create the tunnel (default)
- `./create_gre_tunnel.sh status` - Show tunnel status
- `./create_gre_tunnel.sh remove` - Remove the tunnel
- `./create_gre_tunnel.sh help` - Show help

## Configuration Parameters

| Parameter | Description | Example |
|-----------|-------------|---------|
| `TUNNEL_NAME` | Name of the tunnel interface | `gre1` |
| `LOCAL_IP` | IP address of your machine | `192.168.1.10` |
| `REMOTE_IP` | IP address of the remote endpoint | `192.168.1.20` |
| `TUNNEL_LOCAL_IP` | IP address for your tunnel endpoint | `10.0.0.1/24` |
| `TUNNEL_REMOTE_IP` | IP address for remote tunnel endpoint | `10.0.0.2` |
| `TTL` | Time to live for GRE packets | `64` |

## Important Notes

1. **Both sides must be configured**: The GRE tunnel needs to be set up on both the local and remote machines with matching but reversed parameters.

2. **Firewall considerations**: Ensure that GRE protocol (IP protocol 47) is allowed through any firewalls between the endpoints.

3. **Network routing**: You may need to add routes to direct traffic through the tunnel:
   ```bash
   sudo ip route add 10.0.0.0/24 dev gre1
   ```

4. **Persistent configuration**: The tunnel created with these commands is temporary and will be lost on reboot. To make it persistent, add the commands to your network configuration files.

## Troubleshooting

- **"Cannot find device"**: Make sure the tunnel interface exists and is up
- **"Network is unreachable"**: Check if the remote endpoint is reachable
- **"Operation not permitted"**: Ensure you have sudo privileges
- **GRE packets blocked**: Check firewall rules for IP protocol 47

## Example: Complete Setup for Two Machines

**Machine A (192.168.1.10):**
```bash
sudo ip tunnel add gre1 mode gre remote 192.168.1.20 local 192.168.1.10 ttl 64
sudo ip addr add 10.0.0.1/24 dev gre1
sudo ip link set gre1 up
```

**Machine B (192.168.1.20):**
```bash
sudo ip tunnel add gre1 mode gre remote 192.168.1.10 local 192.168.1.20 ttl 64
sudo ip addr add 10.0.0.2/24 dev gre1
sudo ip link set gre1 up
```

After setup, Machine A can reach Machine B at `10.0.0.2` and vice versa.