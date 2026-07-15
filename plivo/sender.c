#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define FRAME_SIZE 164  /* 4-byte seq + 160-byte payload */
#define LOCALHOST "127.0.0.1"
#define PORT_IN 47010
#define PORT_OUT 47001
#define DROP_MITIGATION_INTERVAL 20

int main(void) {
    // -------------------------------------------------------------------------
    // 1. Setup Input Socket (Listening for frames from harness source)
    // -------------------------------------------------------------------------
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
        perror("bind 47010 failed");
        close(in_fd);
        return 1;
    }

    // -------------------------------------------------------------------------
    // 2. Setup Output Socket & Target Destination (Hostile Relay)
    // -------------------------------------------------------------------------
    int out_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (out_fd < 0) {
        perror("Failed to create output socket");
        close(in_fd);
        return 1;
    }

    struct sockaddr_in relay = {0};
    relay.sin_family = AF_INET;
    relay.sin_port = htons(PORT_OUT);
    relay.sin_addr.s_addr = inet_addr(LOCALHOST);

    // -------------------------------------------------------------------------
    // 3. Core Processing & Redundancy Transmission Loop
    // -------------------------------------------------------------------------
    unsigned char frame[FRAME_SIZE];
    int counter = 0;

    for (;;) {
        ssize_t n = recvfrom(in_fd, frame, sizeof(frame), 0, NULL, NULL);
        
        // Skip incomplete or malformed packets early
        if (n < FRAME_SIZE) {
            continue;
        }

        // Primary Transmission: Send the first copy of the current frame
        sendto(out_fd, frame, FRAME_SIZE, 0, (struct sockaddr *)&relay, sizeof(relay));

        counter++;

        // Secondary Transmission: Send duplicate to mitigate flaky drops.
        // Skips 1 out of 20 frames to stay within the 2.0x total bandwidth limit.
        if (counter % DROP_MITIGATION_INTERVAL != 0) {
            sendto(out_fd, frame, FRAME_SIZE, 0, (struct sockaddr *)&relay, sizeof(relay));
        }
    }

    // Clean up handles (Unreachable but ensures theoretical safety)
    close(out_fd);
    close(in_fd);
    return 0;
}