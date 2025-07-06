#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <linux/if_tunnel.h>

#define BUFFER_SIZE 8192
#define MAX_PAYLOAD 1024

struct gre_config {
    char tunnel_name[IFNAMSIZ];
    char local_ip[INET_ADDRSTRLEN];
    char remote_ip[INET_ADDRSTRLEN];
    char tunnel_local_ip[INET_ADDRSTRLEN];
    int tunnel_prefix;
    int ttl;
};

// Function to send netlink message
int send_netlink_msg(int sock, struct nlmsghdr *nlh) {
    struct sockaddr_nl dest_addr;
    struct iovec iov;
    struct msghdr msg;
    
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0;    // Kernel
    dest_addr.nl_groups = 0;
    
    iov.iov_base = nlh;
    iov.iov_len = nlh->nlmsg_len;
    
    msg.msg_name = &dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    msg.msg_flags = 0;
    
    return sendmsg(sock, &msg, 0);
}

// Function to receive netlink response
int recv_netlink_msg(int sock) {
    char buffer[BUFFER_SIZE];
    struct nlmsghdr *nlh;
    int len;
    
    len = recv(sock, buffer, BUFFER_SIZE, 0);
    if (len < 0) {
        perror("recv");
        return -1;
    }
    
    nlh = (struct nlmsghdr *)buffer;
    if (nlh->nlmsg_type == NLMSG_ERROR) {
        struct nlmsgerr *err = (struct nlmsgerr *)NLMSG_DATA(nlh);
        if (err->error != 0) {
            fprintf(stderr, "Netlink error: %s\n", strerror(-err->error));
            return -1;
        }
    }
    
    return 0;
}

// Function to create GRE tunnel
int create_gre_tunnel(struct gre_config *config) {
    int sock;
    struct nlmsghdr *nlh;
    struct ifinfomsg *ifi;
    struct rtattr *linkinfo, *infodata;
    char buffer[BUFFER_SIZE];
    int seq = 1;
    
    printf("Creating GRE tunnel: %s\n", config->tunnel_name);
    printf("Local endpoint: %s\n", config->local_ip);
    printf("Remote endpoint: %s\n", config->remote_ip);
    printf("Tunnel IP: %s/%d\n", config->tunnel_local_ip, config->tunnel_prefix);
    
    // Create netlink socket
    sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (sock < 0) {
        perror("socket");
        return -1;
    }
    
    // Prepare netlink message
    memset(buffer, 0, BUFFER_SIZE);
    nlh = (struct nlmsghdr *)buffer;
    nlh->nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
    nlh->nlmsg_type = RTM_NEWLINK;
    nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_CREATE | NLM_F_EXCL;
    nlh->nlmsg_seq = seq++;
    nlh->nlmsg_pid = getpid();
    
    ifi = (struct ifinfomsg *)NLMSG_DATA(nlh);
    ifi->ifi_family = AF_UNSPEC;
    ifi->ifi_type = 0;
    ifi->ifi_index = 0;
    ifi->ifi_flags = 0;
    ifi->ifi_change = 0;
    
    // Add interface name
    struct rtattr *ifname = (struct rtattr *)(((char *)nlh) + NLMSG_ALIGN(nlh->nlmsg_len));
    ifname->rta_type = IFLA_IFNAME;
    ifname->rta_len = RTA_LENGTH(strlen(config->tunnel_name) + 1);
    strcpy(RTA_DATA(ifname), config->tunnel_name);
    nlh->nlmsg_len = NLMSG_ALIGN(nlh->nlmsg_len) + RTA_LENGTH(strlen(config->tunnel_name) + 1);
    
    // Add link info
    linkinfo = (struct rtattr *)(((char *)nlh) + NLMSG_ALIGN(nlh->nlmsg_len));
    linkinfo->rta_type = IFLA_LINKINFO;
    linkinfo->rta_len = RTA_LENGTH(0);
    
    // Add kind (gre)
    struct rtattr *kind = (struct rtattr *)(((char *)linkinfo) + RTA_LENGTH(0));
    kind->rta_type = IFLA_INFO_KIND;
    kind->rta_len = RTA_LENGTH(4);
    strcpy(RTA_DATA(kind), "gre");
    linkinfo->rta_len += RTA_LENGTH(4);
    
    // Add GRE-specific data
    infodata = (struct rtattr *)(((char *)linkinfo) + linkinfo->rta_len);
    infodata->rta_type = IFLA_INFO_DATA;
    infodata->rta_len = RTA_LENGTH(0);
    
    // Add local IP
    struct rtattr *local = (struct rtattr *)(((char *)infodata) + RTA_LENGTH(0));
    local->rta_type = IFLA_GRE_LOCAL;
    local->rta_len = RTA_LENGTH(sizeof(uint32_t));
    inet_pton(AF_INET, config->local_ip, RTA_DATA(local));
    infodata->rta_len += RTA_LENGTH(sizeof(uint32_t));
    
    // Add remote IP
    struct rtattr *remote = (struct rtattr *)(((char *)infodata) + infodata->rta_len);
    remote->rta_type = IFLA_GRE_REMOTE;
    remote->rta_len = RTA_LENGTH(sizeof(uint32_t));
    inet_pton(AF_INET, config->remote_ip, RTA_DATA(remote));
    infodata->rta_len += RTA_LENGTH(sizeof(uint32_t));
    
    // Add TTL
    struct rtattr *ttl = (struct rtattr *)(((char *)infodata) + infodata->rta_len);
    ttl->rta_type = IFLA_GRE_TTL;
    ttl->rta_len = RTA_LENGTH(sizeof(uint8_t));
    *(uint8_t *)RTA_DATA(ttl) = config->ttl;
    infodata->rta_len += RTA_LENGTH(sizeof(uint8_t));
    
    linkinfo->rta_len += infodata->rta_len;
    nlh->nlmsg_len = NLMSG_ALIGN(nlh->nlmsg_len) + linkinfo->rta_len;
    
    // Send message
    if (send_netlink_msg(sock, nlh) < 0) {
        perror("send_netlink_msg");
        close(sock);
        return -1;
    }
    
    // Receive response
    if (recv_netlink_msg(sock) < 0) {
        close(sock);
        return -1;
    }
    
    close(sock);
    printf("✓ GRE tunnel created successfully\n");
    return 0;
}

// Function to assign IP address to interface
int assign_ip_address(struct gre_config *config) {
    int sock;
    struct nlmsghdr *nlh;
    struct ifaddrmsg *ifa;
    char buffer[BUFFER_SIZE];
    int seq = 1;
    int if_index;
    
    printf("Assigning IP address to tunnel interface...\n");
    
    // Get interface index
    if_index = if_nametoindex(config->tunnel_name);
    if (if_index == 0) {
        perror("if_nametoindex");
        return -1;
    }
    
    // Create netlink socket
    sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (sock < 0) {
        perror("socket");
        return -1;
    }
    
    // Prepare netlink message
    memset(buffer, 0, BUFFER_SIZE);
    nlh = (struct nlmsghdr *)buffer;
    nlh->nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
    nlh->nlmsg_type = RTM_NEWADDR;
    nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_CREATE | NLM_F_EXCL;
    nlh->nlmsg_seq = seq++;
    nlh->nlmsg_pid = getpid();
    
    ifa = (struct ifaddrmsg *)NLMSG_DATA(nlh);
    ifa->ifa_family = AF_INET;
    ifa->ifa_prefixlen = config->tunnel_prefix;
    ifa->ifa_flags = IFA_F_PERMANENT;
    ifa->ifa_scope = RT_SCOPE_UNIVERSE;
    ifa->ifa_index = if_index;
    
    // Add local address
    struct rtattr *local = (struct rtattr *)(((char *)nlh) + NLMSG_ALIGN(nlh->nlmsg_len));
    local->rta_type = IFA_LOCAL;
    local->rta_len = RTA_LENGTH(sizeof(uint32_t));
    inet_pton(AF_INET, config->tunnel_local_ip, RTA_DATA(local));
    nlh->nlmsg_len = NLMSG_ALIGN(nlh->nlmsg_len) + RTA_LENGTH(sizeof(uint32_t));
    
    // Add address
    struct rtattr *addr = (struct rtattr *)(((char *)nlh) + NLMSG_ALIGN(nlh->nlmsg_len));
    addr->rta_type = IFA_ADDRESS;
    addr->rta_len = RTA_LENGTH(sizeof(uint32_t));
    inet_pton(AF_INET, config->tunnel_local_ip, RTA_DATA(addr));
    nlh->nlmsg_len = NLMSG_ALIGN(nlh->nlmsg_len) + RTA_LENGTH(sizeof(uint32_t));
    
    // Send message
    if (send_netlink_msg(sock, nlh) < 0) {
        perror("send_netlink_msg");
        close(sock);
        return -1;
    }
    
    // Receive response
    if (recv_netlink_msg(sock) < 0) {
        close(sock);
        return -1;
    }
    
    close(sock);
    printf("✓ IP address assigned successfully\n");
    return 0;
}

// Function to bring interface up
int bring_interface_up(struct gre_config *config) {
    int sock;
    struct nlmsghdr *nlh;
    struct ifinfomsg *ifi;
    char buffer[BUFFER_SIZE];
    int seq = 1;
    int if_index;
    
    printf("Bringing interface up...\n");
    
    // Get interface index
    if_index = if_nametoindex(config->tunnel_name);
    if (if_index == 0) {
        perror("if_nametoindex");
        return -1;
    }
    
    // Create netlink socket
    sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (sock < 0) {
        perror("socket");
        return -1;
    }
    
    // Prepare netlink message
    memset(buffer, 0, BUFFER_SIZE);
    nlh = (struct nlmsghdr *)buffer;
    nlh->nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
    nlh->nlmsg_type = RTM_NEWLINK;
    nlh->nlmsg_flags = NLM_F_REQUEST;
    nlh->nlmsg_seq = seq++;
    nlh->nlmsg_pid = getpid();
    
    ifi = (struct ifinfomsg *)NLMSG_DATA(nlh);
    ifi->ifi_family = AF_UNSPEC;
    ifi->ifi_type = 0;
    ifi->ifi_index = if_index;
    ifi->ifi_flags = IFF_UP;
    ifi->ifi_change = IFF_UP;
    
    // Send message
    if (send_netlink_msg(sock, nlh) < 0) {
        perror("send_netlink_msg");
        close(sock);
        return -1;
    }
    
    // Receive response
    if (recv_netlink_msg(sock) < 0) {
        close(sock);
        return -1;
    }
    
    close(sock);
    printf("✓ Interface brought up successfully\n");
    return 0;
}

int main(int argc, char *argv[]) {
    struct gre_config config;
    
    // Default configuration with 10.x.x.x network
    strcpy(config.tunnel_name, "gre1");
    strcpy(config.local_ip, "10.1.1.10");      // Local endpoint IP
    strcpy(config.remote_ip, "10.1.1.20");     // Remote endpoint IP
    strcpy(config.tunnel_local_ip, "10.10.1.1"); // Tunnel local IP
    config.tunnel_prefix = 24;
    config.ttl = 64;
    
    // Parse command line arguments if provided
    if (argc >= 3) {
        strcpy(config.local_ip, argv[1]);
        strcpy(config.remote_ip, argv[2]);
    }
    if (argc >= 4) {
        strcpy(config.tunnel_local_ip, argv[3]);
    }
    
    printf("=== GRE Tunnel Creator ===\n");
    printf("This program creates a GRE tunnel programmatically using netlink sockets\n\n");
    
    // Check if running as root
    if (geteuid() != 0) {
        fprintf(stderr, "This program must be run as root\n");
        return 1;
    }
    
    // Create GRE tunnel
    if (create_gre_tunnel(&config) < 0) {
        fprintf(stderr, "Failed to create GRE tunnel\n");
        return 1;
    }
    
    // Assign IP address
    if (assign_ip_address(&config) < 0) {
        fprintf(stderr, "Failed to assign IP address\n");
        return 1;
    }
    
    // Bring interface up
    if (bring_interface_up(&config) < 0) {
        fprintf(stderr, "Failed to bring interface up\n");
        return 1;
    }
    
    printf("\n=== GRE Tunnel Setup Complete ===\n");
    printf("Tunnel name: %s\n", config.tunnel_name);
    printf("Local endpoint: %s\n", config.local_ip);
    printf("Remote endpoint: %s\n", config.remote_ip);
    printf("Tunnel IP: %s/%d\n", config.tunnel_local_ip, config.tunnel_prefix);
    printf("\nUsage: %s [local_ip] [remote_ip] [tunnel_ip]\n", argv[0]);
    
    return 0;
}