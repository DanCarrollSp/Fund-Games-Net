#include <stdio.h>
#include <arpa/inet.h>

int main() {
    uint32_t host_num = 0x12345678;//Declares a 32 bit unsigned integer in host byte order
    uint32_t net_num = htonl(host_num);//Converts the 32 bit host byte order to network byte order

    printf("Host order: 0x%x\n", host_num);//Prints the original value in host byte order
    printf("Network order: 0x%x\n", net_num);//Prints the converted value in network byte order

    return 0;


    
}
