#pragma once

/* Sends the 26 bytes of the alphabet over the socket described by sockfd to
 * dest. Also, introduces bit error at the rate of ber. */
void gbn_receive(int sockfd);