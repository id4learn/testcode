#!/usr/bin/env python3
"""
GRE Tunnel Creator - Python Implementation
This program creates GRE tunnels programmatically using pyroute2 library.
Uses 10.x.x.x network range to avoid conflicts with common LAN setups.
"""

import sys
import os
import socket
import struct
from contextlib import contextmanager

try:
    from pyroute2 import IPRoute
    PYROUTE2_AVAILABLE = True
except ImportError:
    PYROUTE2_AVAILABLE = False

class GREConfig:
    def __init__(self):
        self.tunnel_name = "gre1"
        self.local_ip = "10.1.1.10"         # Local endpoint IP
        self.remote_ip = "10.1.1.20"        # Remote endpoint IP  
        self.tunnel_local_ip = "10.10.1.1"  # Tunnel local IP
        self.tunnel_prefix = 24
        self.ttl = 64

class GRETunnelCreator:
    def __init__(self, config):
        self.config = config
        self.ip = None
        
    def __enter__(self):
        if not PYROUTE2_AVAILABLE:
            raise ImportError("pyroute2 library not found. Install with: pip install pyroute2")
        
        self.ip = IPRoute()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        if self.ip:
            self.ip.close()
    
    def create_tunnel(self):
        """Create GRE tunnel interface"""
        print(f"Creating GRE tunnel: {self.config.tunnel_name}")
        print(f"Local endpoint: {self.config.local_ip}")
        print(f"Remote endpoint: {self.config.remote_ip}")
        print(f"Tunnel IP: {self.config.tunnel_local_ip}/{self.config.tunnel_prefix}")
        
        try:
            # Create GRE tunnel
            self.ip.link('add', 
                        ifname=self.config.tunnel_name,
                        kind='gre',
                        gre_local=self.config.local_ip,
                        gre_remote=self.config.remote_ip,
                        gre_ttl=self.config.ttl)
            
            print("✓ GRE tunnel created successfully")
            return True
            
        except Exception as e:
            print(f"✗ Failed to create GRE tunnel: {e}")
            return False
    
    def assign_ip_address(self):
        """Assign IP address to tunnel interface"""
        print("Assigning IP address to tunnel interface...")
        
        try:
            # Get interface index
            idx = self.ip.link_lookup(ifname=self.config.tunnel_name)[0]
            
            # Add IP address
            self.ip.addr('add', 
                        index=idx,
                        address=self.config.tunnel_local_ip,
                        prefixlen=self.config.tunnel_prefix)
            
            print("✓ IP address assigned successfully")
            return True
            
        except Exception as e:
            print(f"✗ Failed to assign IP address: {e}")
            return False
    
    def bring_interface_up(self):
        """Bring tunnel interface up"""
        print("Bringing interface up...")
        
        try:
            # Get interface index
            idx = self.ip.link_lookup(ifname=self.config.tunnel_name)[0]
            
            # Bring interface up
            self.ip.link('set', index=idx, state='up')
            
            print("✓ Interface brought up successfully")
            return True
            
        except Exception as e:
            print(f"✗ Failed to bring interface up: {e}")
            return False
    
    def show_tunnel_info(self):
        """Display tunnel information"""
        print("\n=== Tunnel Information ===")
        
        try:
            # Get interface index
            idx = self.ip.link_lookup(ifname=self.config.tunnel_name)[0]
            
            # Get interface info
            links = self.ip.get_links(idx)
            if links:
                link = links[0]
                print(f"Interface: {link.get_attr('IFLA_IFNAME')}")
                print(f"State: {'UP' if link['state'] == 'up' else 'DOWN'}")
                print(f"MTU: {link.get_attr('IFLA_MTU')}")
            
            # Get addresses
            addrs = self.ip.get_addr(index=idx)
            if addrs:
                for addr in addrs:
                    if addr['family'] == socket.AF_INET:
                        ip_addr = addr.get_attr('IFA_ADDRESS')
                        prefix = addr['prefixlen']
                        print(f"IP Address: {ip_addr}/{prefix}")
            
            return True
            
        except Exception as e:
            print(f"✗ Failed to get tunnel info: {e}")
            return False
    
    def remove_tunnel(self):
        """Remove GRE tunnel"""
        print(f"Removing GRE tunnel: {self.config.tunnel_name}")
        
        try:
            # Get interface index
            idx = self.ip.link_lookup(ifname=self.config.tunnel_name)[0]
            
            # Remove tunnel
            self.ip.link('del', index=idx)
            
            print("✓ GRE tunnel removed successfully")
            return True
            
        except Exception as e:
            print(f"✗ Failed to remove tunnel: {e}")
            return False
    
    def test_connectivity(self):
        """Test tunnel connectivity"""
        print(f"Testing connectivity to remote tunnel endpoint...")
        
        # Extract IP without prefix
        remote_tunnel_ip = self.config.tunnel_local_ip.replace('1', '2')  # Simple assumption
        
        try:
            # Simple ping test using os.system
            result = os.system(f"ping -c 3 -W 2 {remote_tunnel_ip} > /dev/null 2>&1")
            if result == 0:
                print("✓ Tunnel connectivity test passed")
            else:
                print("⚠ Cannot reach remote endpoint (expected if remote side not configured)")
            return True
            
        except Exception as e:
            print(f"✗ Connectivity test failed: {e}")
            return False

def install_dependencies():
    """Install required dependencies"""
    print("Installing required dependencies...")
    try:
        os.system("pip3 install pyroute2")
        print("✓ Dependencies installed successfully")
        return True
    except Exception as e:
        print(f"✗ Failed to install dependencies: {e}")
        return False

def main():
    if len(sys.argv) > 1 and sys.argv[1] == "install-deps":
        install_dependencies()
        return
    
    # Check if running as root
    if os.geteuid() != 0:
        print("Error: This program must be run as root")
        print("Use: sudo python3 gre_tunnel.py")
        return 1
    
    # Check if pyroute2 is available
    if not PYROUTE2_AVAILABLE:
        print("Error: pyroute2 library not found")
        print("Install with: python3 gre_tunnel.py install-deps")
        print("Or manually: pip3 install pyroute2")
        return 1
    
    # Create configuration
    config = GREConfig()
    
    # Parse command line arguments
    if len(sys.argv) >= 3:
        config.local_ip = sys.argv[1]
        config.remote_ip = sys.argv[2]
    
    if len(sys.argv) >= 4:
        config.tunnel_local_ip = sys.argv[3]
    
    print("=== GRE Tunnel Creator (Python) ===")
    print("This program creates GRE tunnels programmatically using pyroute2\n")
    
    # Handle different commands
    command = sys.argv[-1] if len(sys.argv) > 1 else "create"
    
    try:
        with GRETunnelCreator(config) as tunnel_creator:
            if command == "remove":
                tunnel_creator.remove_tunnel()
            elif command == "status":
                tunnel_creator.show_tunnel_info()
            else:  # create (default)
                # Create tunnel
                if not tunnel_creator.create_tunnel():
                    return 1
                
                # Assign IP address
                if not tunnel_creator.assign_ip_address():
                    return 1
                
                # Bring interface up
                if not tunnel_creator.bring_interface_up():
                    return 1
                
                # Show tunnel info
                tunnel_creator.show_tunnel_info()
                
                # Test connectivity
                tunnel_creator.test_connectivity()
                
                print("\n=== GRE Tunnel Setup Complete ===")
                print(f"Tunnel name: {config.tunnel_name}")
                print(f"Local endpoint: {config.local_ip}")
                print(f"Remote endpoint: {config.remote_ip}")
                print(f"Tunnel IP: {config.tunnel_local_ip}/{config.tunnel_prefix}")
                print(f"\nUsage: python3 {sys.argv[0]} [local_ip] [remote_ip] [tunnel_ip]")
                print(f"Remove: python3 {sys.argv[0]} remove")
                print(f"Status: python3 {sys.argv[0]} status")
    
    except Exception as e:
        print(f"Error: {e}")
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())