#!/bin/bash

# GRE Tunnel Creation Script
# This script demonstrates how to create a GRE tunnel on Linux

set -e  # Exit on any error

# Configuration variables - MODIFY THESE FOR YOUR SETUP
TUNNEL_NAME="gre1"                    # Name of the GRE tunnel interface
LOCAL_IP="192.168.1.10"               # Local endpoint IP (your machine's IP)
REMOTE_IP="192.168.1.20"              # Remote endpoint IP (destination machine's IP)
TUNNEL_LOCAL_IP="10.0.0.1/24"         # IP address for the local end of the tunnel
TUNNEL_REMOTE_IP="10.0.0.2"           # IP address for the remote end of the tunnel
TTL="64"                              # Time to live for GRE packets

echo "=== GRE Tunnel Creation Script ==="
echo "Tunnel Name: $TUNNEL_NAME"
echo "Local Endpoint: $LOCAL_IP"
echo "Remote Endpoint: $REMOTE_IP"
echo "Tunnel Local IP: $TUNNEL_LOCAL_IP"
echo "Tunnel Remote IP: $TUNNEL_REMOTE_IP"
echo

# Function to create the GRE tunnel
create_tunnel() {
    echo "1. Creating GRE tunnel interface..."
    sudo ip tunnel add $TUNNEL_NAME mode gre remote $REMOTE_IP local $LOCAL_IP ttl $TTL
    echo "   ✓ GRE tunnel '$TUNNEL_NAME' created"
    
    echo "2. Assigning IP address to tunnel interface..."
    sudo ip addr add $TUNNEL_LOCAL_IP dev $TUNNEL_NAME
    echo "   ✓ IP address $TUNNEL_LOCAL_IP assigned to $TUNNEL_NAME"
    
    echo "3. Bringing tunnel interface up..."
    sudo ip link set $TUNNEL_NAME up
    echo "   ✓ Tunnel interface $TUNNEL_NAME is now up"
    
    echo "4. Verifying tunnel configuration..."
    echo "   Interface details:"
    ip addr show $TUNNEL_NAME
    echo
    echo "   Tunnel details:"
    ip tunnel show $TUNNEL_NAME
}

# Function to test the tunnel
test_tunnel() {
    echo "5. Testing tunnel connectivity..."
    echo "   Attempting to ping remote tunnel endpoint: $TUNNEL_REMOTE_IP"
    if ping -c 3 -W 2 $TUNNEL_REMOTE_IP > /dev/null 2>&1; then
        echo "   ✓ Tunnel is working - can reach remote endpoint"
    else
        echo "   ⚠ Cannot reach remote endpoint (this is expected if remote side is not configured)"
    fi
}

# Function to show tunnel status
show_status() {
    echo "6. Current tunnel status:"
    echo "   All tunnel interfaces:"
    ip tunnel show
    echo
    echo "   Tunnel interface $TUNNEL_NAME:"
    ip addr show $TUNNEL_NAME 2>/dev/null || echo "   Interface $TUNNEL_NAME not found"
}

# Function to remove the tunnel
remove_tunnel() {
    echo "Removing GRE tunnel..."
    sudo ip link set $TUNNEL_NAME down 2>/dev/null || true
    sudo ip tunnel del $TUNNEL_NAME 2>/dev/null || true
    echo "   ✓ Tunnel $TUNNEL_NAME removed"
}

# Main execution
case "${1:-create}" in
    "create")
        create_tunnel
        test_tunnel
        show_status
        echo
        echo "=== GRE Tunnel Setup Complete ==="
        echo "To remove the tunnel, run: $0 remove"
        echo "To check status, run: $0 status"
        ;;
    "remove")
        remove_tunnel
        ;;
    "status")
        show_status
        ;;
    "help")
        echo "Usage: $0 [create|remove|status|help]"
        echo "  create  - Create and configure the GRE tunnel (default)"
        echo "  remove  - Remove the GRE tunnel"
        echo "  status  - Show tunnel status"
        echo "  help    - Show this help message"
        ;;
    *)
        echo "Invalid option. Use 'help' to see available commands."
        exit 1
        ;;
esac