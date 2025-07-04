# SD-WAN Project Setup Guide for Linux Machines

## Table of Contents
1. [Introduction](#introduction)
2. [Open Source SD-WAN Solutions](#open-source-sd-wan-solutions)
3. [Hardware Requirements](#hardware-requirements)
4. [Setup Methods](#setup-methods)
5. [Popular Solutions Deep Dive](#popular-solutions-deep-dive)
6. [Network Configuration](#network-configuration)
7. [Troubleshooting](#troubleshooting)
8. [Best Practices](#best-practices)

## Introduction

Software-Defined Wide Area Networks (SD-WAN) can be implemented on Linux machines using various open-source solutions. This guide covers the setup and configuration of different SD-WAN projects, from simple implementations to enterprise-grade solutions.

## Open Source SD-WAN Solutions

### 1. flexiWAN
- **Description**: World's first open-source SD-WAN solution
- **Architecture**: Modular design with flexiEdge (router) and flexiManage (management)
- **Technology Stack**: 
  - FD.io Vector Packet Processor (VPP) for data plane
  - Free Range Routing (FRR) for control plane
  - flexiWAN Agent for management communication

### 2. OpenMPTCProuter
- **Description**: MultiPath TCP (MPTCP) based solution for aggregating multiple connections
- **Key Features**:
  - Bandwidth aggregation from up to 8 internet connections
  - Connection failover
  - Built-in security with encryption

### 3. OpenWRT-based Solutions
- **Description**: Using OpenWRT as SD-WAN platform with routing protocols
- **Protocols Supported**: OSPF, BGP, MWAN3
- **Tools**: Free Range Routing (FRR), Quagga

### 4. VPP-based Solutions
- **Description**: High-performance packet processing using Vector Packet Processor
- **Features**: 
  - DPDK acceleration
  - Segment Routing (SRv6)
  - IPSec tunneling

## Hardware Requirements

### Minimum Requirements
- **CPU**: 2-4 cores (Intel x86-64 recommended)
- **Memory**: 4-8GB RAM
- **Storage**: 50-100GB SSD/HDD
- **Network**: Multiple network interfaces (2+ recommended)

### Recommended for Production
- **CPU**: 8+ cores with AES-NI, SSSE3, SSE4 support
- **Memory**: 16-32GB RAM
- **Storage**: 128GB+ SSD with 200+ IOPS
- **Network**: 10GbE interfaces with SR-IOV support
- **Special Hardware**: Intel 82599/X710/E810 NICs for high performance

### Cloud/VM Requirements
- **Virtualization**: KVM, VMware ESXi, VirtualBox supported
- **Cloud Platforms**: AWS EC2, Google Cloud, Azure, DigitalOcean
- **VM Specs**: Match physical requirements above

## Setup Methods

### Method 1: flexiWAN Installation

#### System Requirements
- Ubuntu 18.04.5+ or Debian-based Linux
- Internet connectivity for package downloads
- Sudo/root access

#### Installation Steps
```bash
# Update system
sudo apt update && sudo apt upgrade -y

# Download flexiWAN image
wget https://download.flexiwan.com/latest/flexiwan-x.x.x.iso

# Create bootable USB or VM installation
# Follow guided installation process

# Post-installation: Configure management interface
sudo flexiwan-agent configure
```

#### Configuration
```bash
# Access flexiManage web interface
# Default: https://manage.flexiwan.com

# Register device with organization token
# Configure interfaces and tunnels through web UI
```

### Method 2: OpenWRT SD-WAN Setup

#### Installation on Hardware
```bash
# Flash OpenWRT firmware to device
# Or install in VM/container

# Update package lists
opkg update

# Install FRR for routing protocols
opkg install frr frr-watchfrr frr-ospfd frr-staticd frr-zebra frr-vtysh

# Install additional tools
opkg install nano btop mtr curl
```

#### OSPF Configuration
```bash
# Enable OSPF daemon
echo "ospfd=yes" >> /etc/frr/daemons

# Configure OSPF
cat > /etc/frr/frr.conf << EOF
router ospf
  router-id 192.168.1.1
  network 192.168.0.0/16 area 0.0.0.0
  area 0.0.0.0 range 192.168.0.0/16
  default-information originate always
  redistribute kernel
  redistribute connected
  redistribute static
EOF

# Start FRR service
service frr enable
service frr start
```

### Method 3: VPP-based Setup

#### Installing VPP on Ubuntu/Debian
```bash
# Add FD.io repository
curl -L https://packagecloud.io/fdio/release/gpgkey | sudo apt-key add -
echo "deb [trusted=yes] https://packagecloud.io/fdio/release/ubuntu $(lsb_release -cs) main" | sudo tee /etc/apt/sources.list.d/99fd.io.list

# Install VPP packages
sudo apt update
sudo apt install vpp vpp-plugin-core vpp-plugin-dpdk

# Configure VPP
sudo vi /etc/vpp/startup.conf
```

#### Basic VPP Configuration
```
unix {
  cli-listen /run/vpp/cli.sock
  gid vpp
}

api-trace {
  on
}

api-segment {
  gid vpp
}

cpu {
  main-core 1
  corelist-workers 2-3
}

dpdk {
  dev 0000:00:08.0
  dev 0000:00:09.0
}
```

### Method 4: OpenMPTCProuter Setup

#### Installation
```bash
# Download latest OpenMPTCProuter image
wget https://github.com/Ysurac/openmptcprouter/releases/latest

# Flash to device or create VM
# Follow setup wizard for network configuration

# Configure multiple WAN interfaces through web UI
# Access: http://192.168.100.1
```

## Popular Solutions Deep Dive

### flexiWAN Architecture

#### Components
1. **flexiEdge**: Router software running on premises
2. **flexiManage**: Cloud-based management platform
3. **flexiWAN Agent**: Communication bridge

#### Key Features
- Zero-touch provisioning
- Multi-tenant management
- Application-based routing
- Quality of Service (QoS)
- Security firewall
- High availability support

#### Tunnel Configuration
- IPSec over VxLAN (default)
- IKEv2 support
- NAT traversal
- Flexible topologies (full-mesh, hub-spoke, partial-mesh)

### OpenWRT Routing Setup

#### MWAN3 Configuration
```bash
# Install mwan3 for multi-WAN support
opkg install mwan3 luci-app-mwan3

# Configure interfaces
uci set mwan3.wan.enabled='1'
uci set mwan3.wan2.enabled='1'

# Set load balancing policy
uci set mwan3.balanced.last_resort='unreachable'
uci commit mwan3

# Restart services
/etc/init.d/mwan3 restart
```

#### BGP Configuration with Quagga
```bash
# Install Quagga
opkg install quagga-bgpd quagga-zebra

# Configure BGP
cat > /etc/quagga/bgpd.conf << EOF
hostname openwrt
password zebra
enable password zebra

router bgp 65001
  bgp router-id 192.168.1.1
  neighbor 192.168.1.2 remote-as 65002
  redistribute kernel
  redistribute connected
EOF

# Start services
/etc/init.d/quagga start
/etc/init.d/quagga enable
```

## Network Configuration

### Interface Setup
```bash
# Configure WAN interfaces
sudo ip addr add 10.1.1.1/24 dev eth0
sudo ip addr add 10.2.2.1/24 dev eth1

# Configure LAN interface
sudo ip addr add 192.168.1.1/24 dev eth2

# Enable IP forwarding
echo 'net.ipv4.ip_forward=1' | sudo tee -a /etc/sysctl.conf
sudo sysctl -p
```

### IPSec Tunnel Configuration
```bash
# Install strongSwan
sudo apt install strongswan

# Configure IPSec
sudo cat > /etc/ipsec.conf << EOF
config setup
    charondebug="all"
    uniqueids=yes

conn tunnel1
    auto=start
    left=10.1.1.1
    leftsubnet=192.168.1.0/24
    right=10.2.2.1
    rightsubnet=192.168.2.0/24
    ike=aes256-sha256-modp2048
    esp=aes256-sha256
    authby=secret
EOF

# Configure pre-shared keys
sudo cat > /etc/ipsec.secrets << EOF
10.1.1.1 10.2.2.1 : PSK "your-secret-key"
EOF

# Start IPSec
sudo systemctl enable strongswan
sudo systemctl start strongswan
```

### Firewall Configuration
```bash
# Configure iptables for SD-WAN
# Allow IPSec traffic
sudo iptables -A INPUT -p udp --dport 500 -j ACCEPT
sudo iptables -A INPUT -p udp --dport 4500 -j ACCEPT
sudo iptables -A INPUT -p esp -j ACCEPT

# NAT configuration for outbound traffic
sudo iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE
sudo iptables -t nat -A POSTROUTING -o eth1 -j MASQUERADE

# Save rules
sudo iptables-save > /etc/iptables/rules.v4
```

## Troubleshooting

### Common Issues and Solutions

#### 1. Package Installation Failures
```bash
# Update package sources
sudo apt update
sudo apt --fix-broken install

# Check system requirements
uname -a
cat /proc/cpuinfo | grep flags
```

#### 2. Network Interface Issues
```bash
# Check interface status
ip link show
ip addr show

# Verify driver support
lspci | grep -i network
ethtool -i eth0
```

#### 3. VPP Connection Issues
```bash
# Check VPP status
sudo systemctl status vpp

# Access VPP CLI
sudo vppctl

# Check interface status in VPP
vpp# show interface
vpp# show ip fib
```

#### 4. Routing Problems
```bash
# Check routing table
ip route show

# Verify FRR status
sudo systemctl status frr

# Access FRR CLI
sudo vtysh
```

### Debugging Commands

#### Network Diagnostics
```bash
# Test connectivity
ping -c 4 8.8.8.8
traceroute 8.8.8.8
mtr google.com

# Check port connectivity
nc -zv target_ip port
nmap -p 500,4500 target_ip

# Monitor traffic
tcpdump -i any -n host target_ip
```

#### Log Analysis
```bash
# System logs
journalctl -u vpp -f
journalctl -u strongswan -f
journalctl -u frr -f

# Check dmesg for hardware issues
dmesg | grep -i error
dmesg | grep -i network
```

## Best Practices

### Security
1. **Use strong authentication**: Implement certificate-based auth where possible
2. **Regular updates**: Keep all components updated
3. **Firewall rules**: Implement strict ingress/egress filtering
4. **Monitoring**: Deploy comprehensive logging and monitoring

### Performance
1. **Hardware acceleration**: Use SR-IOV and DPDK where available
2. **CPU affinity**: Pin VPP workers to specific CPU cores
3. **Memory management**: Configure hugepages for VPP
4. **Network tuning**: Optimize network buffer sizes

### High Availability
1. **Redundant links**: Configure multiple WAN connections
2. **Health monitoring**: Implement link quality monitoring
3. **Failover policies**: Configure automatic failover mechanisms
4. **Load balancing**: Distribute traffic across multiple paths

### Monitoring and Management
1. **Centralized logging**: Use syslog or ELK stack
2. **SNMP monitoring**: Enable SNMP for network monitoring
3. **Performance metrics**: Monitor bandwidth, latency, packet loss
4. **Alerting**: Configure alerts for network events

### Sample Network Monitoring Script
```bash
#!/bin/bash
# SD-WAN health check script

check_interface() {
    interface=$1
    if ip link show $interface | grep -q "state UP"; then
        echo "✓ Interface $interface is UP"
        return 0
    else
        echo "✗ Interface $interface is DOWN"
        return 1
    fi
}

check_tunnel() {
    remote_ip=$1
    if ping -c 1 -W 2 $remote_ip > /dev/null 2>&1; then
        echo "✓ Tunnel to $remote_ip is active"
        return 0
    else
        echo "✗ Tunnel to $remote_ip is down"
        return 1
    fi
}

# Check all interfaces
check_interface eth0
check_interface eth1
check_interface eth2

# Check tunnel connectivity
check_tunnel 10.2.2.1
check_tunnel 10.3.3.1

# Check routing
if ip route get 8.8.8.8 > /dev/null 2>&1; then
    echo "✓ Routing is functional"
else
    echo "✗ Routing issues detected"
fi
```

## Conclusion

Setting up an SD-WAN project on Linux machines involves choosing the right solution based on your requirements, properly configuring hardware and software components, and implementing appropriate monitoring and security measures. The open-source solutions mentioned in this guide provide enterprise-grade capabilities while maintaining flexibility and cost-effectiveness.

For production deployments, consider factors such as:
- Scalability requirements
- Security compliance needs
- Performance expectations
- Management complexity
- Support availability

## Additional Resources

- [flexiWAN Documentation](https://docs.flexiwan.com/)
- [OpenWRT Documentation](https://openwrt.org/docs/start)
- [FD.io VPP Documentation](https://docs.fd.io/vpp/latest/)
- [FRR Documentation](https://frrouting.org/documentation/)
- [StrongSwan Documentation](https://wiki.strongswan.org/)