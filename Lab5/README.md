## Project Structure

```text
src
 ├── crc.c/.h           -- Contains utilities for generating CRC checksums
 ├── gbn_receiver.c/.h  -- The GBN receiving subroutine, already completed for you
 ├── gbn_sender.c/.h    -- *** TODO: Your responsibility for this lab ***
 ├── pkt_utils.c/.h     -- Definitions & utilities you will want to use for building and corrupting packets
 ├── udp_receiver.c     -- Entry point for receiver/server program. Hands off execution to the gbn_receiver
 └── udp_sender.c       -- Entry point for sender/client program. Hands off execution to your gbn_sender
```

## Getting Started

First, take plenty of time to *read* through the provided code, especially `crc.h` and `pkt_utils.h`. You don't necessarily need to understand the intricacies of the implementation, but you should feel comfortable with the usage and structure of the project. Reading code takes time.

### Compiling and Running

A makefile is included which compiles both the `bin/sender` and `bin/receiver` binaries. You may then run the receiver and sender. The below example uses the port 5000, though you may use any un-privileged port.

```shell
make
bin/receiver 5000
bin/sender 127.0.0.1 5000 0.01
```

If successful, you should see the sender transmit a packet, and the receiver respond with an ACK or NAK.
Once ready, you may begin developing your solution in `sender_subroutine.c`.

## Helpful Notes

- When receiving a `NAK`, don't think too much into the received packet printed out by the receiver. After all, it was a _corrupted_ packet.

## Above and Beyond

For your own learning or curiosity, you may consider the following questions and additions:

- Properly add network byte order translation functions for our defined packets, e.g. htonpkt and ntohpkt. The CRC checksum is the important part!
- Add retransmission timeouts alongside error introduction for ACKs/NAKs
- Upgrade to selective repeat ARQ
- Modify the crc code to use a different CRC scheme (<https://reveng.sourceforge.io/crc-catalogue/16.htm>)
