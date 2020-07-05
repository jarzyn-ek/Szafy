#ifndef HANDLERS_H
#define HANDLERS_H


// void want_pyrkon_ticket_handler(packet_t *packet);
// void want_pyrkon_ticket_ack_handler(packet_t *packet);
// void want_pyrkon_exit_handler(packet_t *packet);

void want_lift_handler(packet_t* packet);
void want_lift_ack_handler(packet_t *packet);
void want_rooms_handler(packet_t* packet);
void want_rooms_ack_handler(packet_t* packet);
void reserved_rooms_handler(packet_t* packet);

#endif