#include <stdio.h>
#include <stdint.h>
#include <winsock2.h> // For htons, htonl, ntohs, ntohl

int main() {
    // Our starting data
    uint16_t host_port = 80;            // 2 Bytes -> Hex: 0x0050
    uint32_t host_ip   = 0xC0A80105;    // 4 Bytes -> Hex: 0xC0A80105 (192.168.1.5)

    printf("=== STEP 1: HOST TO NETWORK (PACKING) ===\n");

    // --- Method A: Using Built-in Functions ---
    uint16_t net_port_func = htons(host_port);
    uint32_t net_ip_func   = htonl(host_ip);

    // --- Method B: Using Manual Bit Shifts ---
    // For 16-bit: Move lower byte up 8 bits, and upper byte down 8 bits
    uint16_t net_port_shift = ((host_port & 0x00FF) << 8) | 
                              ((host_port & 0xFF00) >> 8);

    // For 32-bit: Reverse all 4 bytes completely
    uint32_t net_ip_shift = ((host_ip & 0x000000FF) << 24) |
                            ((host_ip & 0x0000FF00) << 8)  |
                            ((host_ip & 0x00FF0000) >> 8)  |
                            ((host_ip & 0xFF000000) >> 24);

    printf("Port (Functions): 0x%04X | Port (Shifts): 0x%04X\n", net_port_func, net_port_shift);
    printf("IP   (Functions): 0x%08X | IP   (Shifts): 0x%08X\n\n", net_ip_func, net_ip_shift);


    printf("=== STEP 2: NETWORK TO HOST (UNPACKING) ===\n");

    // --- Method A: Using Built-in Functions ---
    uint16_t back_port_func = ntohs(net_port_func);
    uint32_t back_ip_func   = ntohl(net_ip_func);

    // --- Method B: Using Manual Bit Shifts ---
    // Shifting network data back follows the exact same swapping logic
    uint16_t back_port_shift = ((net_port_func & 0x00FF) << 8) | 
                               ((net_port_func & 0xFF00) >> 8);

    uint32_t back_ip_shift = ((net_ip_func & 0x000000FF) << 24) |
                             ((net_ip_func & 0x0000FF00) << 8)  |
                             ((net_ip_func & 0x00FF0000) >> 8)  |
                             ((net_ip_func & 0xFF000000) >> 24);

    printf("Port Back: %d (Hex: 0x%04X)\n", back_port_shift, back_port_shift);
    printf("IP Back:   0x%08X\n", back_ip_shift);

    return 0;
}

/*
The Four Main Functions1. 
htons()
Read it as: Host To Network Short
What it does: Takes a 16-bit port number from your computer and swaps the bytes to make it ready for the internet.
2. htonl()
Read it as: Host To Network Long
What it does: Takes a 32-bit IP address from your computer and flips all four bytes so network routers can read it.
3. ntohs()
Read it as: Network To Host Short
What it does: Takes a 16-bit port number you just received from the internet and flips it back so your computer can read it normally.
4. ntohl()
Read it as: Network To Host Long
What it does: Takes a 32-bit IP address you received from the internet and un-swaps the bytes so your computer recognizes the original number.

gcc -O2 .\endian.c -o endian -lws2_32
*/