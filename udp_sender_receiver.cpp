#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>



int main(int argc, char* argv[]) {
    // Parse the command-line arguments
    if (argc < 6) {
        std::cerr << "Usage: " << argv[0] << " <mode> <local_ip> <local_port> <destination_ip> <destination_port>" << std::endl;
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

    // Create a UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
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
        // Set the destination IP and port
        sockaddr_in destination_addr;
        destination_addr.sin_family = AF_INET;
        destination_addr.sin_addr.s_addr = inet_addr(destination_ip.c_str());
        destination_addr.sin_port = htons(destination_port);

        // Send packets indefinitely
        for (int sequence_number = 0; sequence_number < sequence_number_limit; ++sequence_number) {
            // Construct the packet data
            std::string packet_data = std::to_string(sequence_number) + ": This is packet " + std::to_string(sequence_number) + " in the sequence.";
            // Send the packet
            sendto(sock, packet_data.c_str(), packet_data.size(), 0, (sockaddr*)&destination_addr, sizeof(destination_addr));
            std::cout << "Sent packet with sequence number " << sequence_number << std::endl;
        }
    } else if (mode == 'r') {
	// Receive packets indefinitely
        while (true) {
            // Receive a packet
            char buffer[1024];
            sockaddr_in sender_addr;
            socklen_t sender_addr_len = sizeof(sender_addr);
            int bytes_received = recvfrom(sock, buffer, 1024, 0, (sockaddr*)&sender_addr, &sender_addr_len);
            if (bytes_received < 0) {
                std::cerr << "Failed to receive packet" << std::endl;
                continue;
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

g++ -o udp_sender_receiver udp_sender_receiver.cpp
./udp_sender_receiver s 192.168.10.10 1234 192.168.10.20 2345 10000
./udp_sender_receiver r 192.168.10.10 1234 192.168.10.20 2345

*/
