#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#define FRAME_SIZE 164
#define BUFFER_SIZE 2048
#define LOCALHOST "127.0.0.1"
#define PORT_IN 47002
#define PORT_OUT 47020

int main(void) {
    
    // 1. Setup Input Socket (Listening for media from relay)
   
    int in_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (in_fd < 0) {
        perror("Failed to create input socket");
        return 1;
    }

    struct sockaddr_in in_addr = {0};
    in_addr.sin_family = AF_INET;
    in_addr.sin_port = htons(PORT_IN);
    in_addr.sin_addr.s_addr = inet_addr(LOCALHOST);

    if (bind(in_fd, (struct sockaddr *)&in_addr, sizeof(in_addr)) < 0) {
        perror("bind 47002 failed");
        close(in_fd);
        return 1;
    }

  
    // 2. Setup Output Socket & Target Destination (Harness Player)
    
    int out_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (out_fd < 0) {
        perror("Failed to create output socket");
        close(in_fd);
        return 1;
    }

    struct sockaddr_in player = {0};
    player.sin_family = AF_INET;
    player.sin_port = htons(PORT_OUT);
    player.sin_addr.s_addr = inet_addr(LOCALHOST);

    
    // 3. Core Forwarding Loop
    
    unsigned char pkt[BUFFER_SIZE];

    for (;;) {
        ssize_t n = recvfrom(in_fd, pkt, sizeof(pkt), 0, NULL, NULL);
        
        // Drop malformed or incomplete packets early
        if (n < FRAME_SIZE) {
            continue;
        }

        // Direct pass-through to harness player
        sendto(out_fd, pkt, FRAME_SIZE, 0, (struct sockaddr *)&player, sizeof(player));
    }

    // Clean up handles (Unreachable in infinite loop but good practice)
    close(out_fd);
    close(in_fd);
    return 0;
}