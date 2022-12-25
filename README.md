# sender_receiver
A simple code for tcp/udp sender and receiver with c++ language, single client and server only.

# Build
Please run `build` to build `tcp_sender_receiver`,`udp_sender_receiver`.

# Usage
```
./tcp_sender_receiver <mode> <local_ip> <local_port> <destination_ip> <destination_port> [<sequence_number_limit>]
./udp_sender_receiver <mode> <local_ip> <local_port> <destination_ip> <destination_port> [<sequence_number_limit>]

# <mode>                      s/r: Choice run with sender mode or receiver mode.
# <local_ip>                  The local static IP address that you want to bind with.
# <local_port>                The local port number that you want to bind with.
# <destnation_ip>             The destination static IP address that you want to connect to.
# <destination_port>          The destination port number that you want to connect to.
# [<sequence_number_limit>]   For sender only, assign the limitation of the seqence number starting from 0.
```

# Example

```

# Please notice that you must run the receiver first instead of the sender.

#  The communication flow of following example.
#  sender                   receiver
#  192.168.50.51:1234   ->  192.168.50.55:4567

# TCP version
./tcp_sender_receiver r 192.168.50.55 4567 192.168.50.51 1234         # run with receiver mode.
./tcp_sender_receiver s 192.168.50.51 1234 192.168.50.55 4567 10000   # run with sender mode, sending sequence number from 0 to 9999

# UDP version
./udp_sender_receiver r 192.168.50.55 4567 192.168.50.51 1234         # run with receiver mode.
./udp_sender_receiver s 192.168.50.51 1234 192.168.50.55 4567 10000   # run with sender mode, sending sequence number from 0 to 9999

```
