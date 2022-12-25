#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    // Parse the command-line arguments
    if (argc < 6) {
        std::cerr << "Usage: " << argv[0] << " <mode> <local_ip> <local_port> <destination_ip> <destination_port> <sequence_number_limit>" << std::endl;
        return 1;
    }
    char mode = argv[1][0];
    std::string local_ip = argv[2];
    int local_port = std::stoi(argv[3]);
    std::string destination_ip = argv[4];
    int destination_port = std::stoi(argv[5]);
    int sequence_number_limit = 0;
    if (argc > 6)
        sequence_number_limit = std::stoi(argv[6]);

    // Create a TCP socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    // Bind the socket to the local IP and port
    sockaddr_in local_addr;
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = inet_addr(local_ip.c_str());
    local_addr.sin_port = htons(local_port);
    if (bind(sock, (sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
        std::cerr << "Failed to bind socket" << std::endl;
        return 1;
    }

    if (mode == 's') {
        // Connect to the destination IP and port
        sockaddr_in destination_addr;
        destination_addr.sin_family = AF_INET;
        destination_addr.sin_addr.s_addr = inet_addr(destination_ip.c_str());
        destination_addr.sin_port = htons(destination_port);
        
        int flag = 1;
        setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
        
        if (connect(sock, (sockaddr*)&destination_addr, sizeof(destination_addr)) < 0) {
            std::cerr << "Failed to connect to destination" << std::endl;
            return 1;
        }

        // Send packets with sequence numbers from 0 to the limit
        for (int i = 0; i < sequence_number_limit; i++) {
            // Construct the packet
            std::string packet = std::to_string(i) + ": packet " + std::to_string(i);
            packet[packet.length()] = 0;
            // Send the packet
            send(sock, packet.c_str(), packet.length(), 0);
            
            std::cout << "send message: " << packet << std::endl;
            usleep(5);
        }
    } else if (mode == 'r') {
        // Listen for incoming connections
        if (listen(sock, 1) < 0) {
            std::cerr << "Failed to listen for incoming connections" << std::endl;
            return 1;
        }
        // Accept an incoming connection
        sockaddr_in sender_addr;
        socklen_t sender_addr_len = sizeof(sender_addr);
        int sender_sock = accept(sock, (sockaddr*)&sender_addr, &sender_addr_len);
        if (sender_sock < 0) {
            std::cerr << "Failed to accept incoming connection" << std::endl;
            return 1;
        }

        // Receive packets indefinitely
        while (true) {
            // Receive a packet
            char buffer[1024] = { 0 };
            int bytes_received = recv(sender_sock, buffer, 1024, 0);
            if (bytes_received < 0) {
                std::cerr << "Failed to receive packet" << std::endl;
                continue;
            }
            
            if (bytes_received == 0) {
                std::cout << "Received packet with size = 0" << std::endl;
                close(sender_sock);
                break;
            }
            // Extract the sequence number and message from the packet
            std::string packet(buffer, bytes_received);
            int sequence_number = std::stoi(packet.substr(0, packet.find(':')));
            std::string message = packet.substr(packet.find(':') + 1);
            // Print the received packet
            std::cout << "Received packet with sequence number " << sequence_number << ": " << message << std::endl;
        }
    } else {
        std::cerr << "Invalid mode" << std::endl;
        return 1;
    }

    return 0;
}

/*

g++ -o tcp_sender_receiver tcp_sender_receiver.cpp
./tcp_sender_receiver s 192.168.10.10 1234 192.168.10.20 2345 10000
./tcp_sender_receiver r 192.168.10.10 1234 192.168.10.20 2345

*/
