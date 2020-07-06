#ifndef HANDLERS_H
#define HANDLERS_H

void want_lift_handler(packet_t* packet);
void want_lift_ack_handler(packet_t *packet);
void want_rooms_handler(packet_t* packet);
void want_rooms_ack_handler(packet_t* packet);
void free_rooms_handler(packet_t* packet)
int check_rooms(int src,int rooms);
int count_reserved_rooms();

#endif