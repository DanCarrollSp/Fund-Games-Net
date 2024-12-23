#include <stdio.h>
#include <arpa/inet.h>

int main() {
    uint16_t host_short = 0x1234;//Declares a 16 bit unsigned integer in host byte order
    uint16_t net_short = htons(host_short);//Converts the 16 bit host byte order to network byte order

    printf("Host order (16-bit): 0x%x\n", host_short);//Prints the original value in host byte order
    printf("Network order (16-bit): 0x%x\n", net_short);//Prints the converted value in network byte order

    return 0;
}
