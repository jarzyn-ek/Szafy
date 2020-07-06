#include "main.h"

//funkcje - reakcje na otrzymanie konkretnych typow komunikatow

void want_lift_handler(packet_t *packet){
	int my_request_clock = get_my_messages_lamport_clocks(0);

	if ((size<=LIFTS) || ((get_state()!= have_rooms && get_state() != want_lift_upper) || my_request_clock > packet->ts || (my_request_clock == packet->ts && rank > packet->src))) {
		packet->ts = get_increased_lamport_clock();
		sendPacket(packet,packet->src,WANT_LIFT_ACK);
	} else {
		waiting_for_ack[packet->src].ts = packet->ts;
		waiting_for_ack[packet->src].src = packet->src;
		waiting_for_ack[packet->src].data = packet->data;
		waiting_for_ack[packet->src].dst = packet->dst;
	}
}

void want_lift_ack_handler(packet_t *packet){
	if(get_state()==have_rooms || get_state()==want_lift_upper){
		my_received_ack_increase(0);
		int my_ack_count = get_my_received_ack(0);
		if (my_ack_count >= size - LIFTS){
			pthread_mutex_unlock(&lift_mutex);
		}
	}
}

void want_rooms_ack_handler(packet_t* packet) {

	if (get_state() == init) {

		my_received_ack_increase(1);
		println("MAM %d ROOMS_ACK\n", get_my_received_ack(1));
		check_rooms(packet->src,packet->number_of_rooms);
	}
}

void free_rooms_handler(packet_t* packet) {
	if (get_state()==init) {
		check_rooms(packet->src,0);
	}
}

void want_rooms_handler(packet_t* packet) {
	int my_request_clock = get_my_messages_lamport_clocks(1);

	if (get_state()!=init || (my_request_clock > packet->ts || my_request_clock <= 0) || (my_request_clock == packet->ts && rank > packet->src)) {
		packet->number_of_rooms = my_rooms;
		packet->ts = get_increased_lamport_clock();
		sendPacket(packet, packet->src, WANT_ROOMS_ACK);
	} else {
		waiting_for_rooms_ack[packet->src].ts = packet->ts;
		waiting_for_rooms_ack[packet->src].src = packet->src;
		waiting_for_rooms_ack[packet->src].data = packet->data;
		waiting_for_rooms_ack[packet->src].dst = packet->dst;
		waiting_for_rooms_ack[packet->src].number_of_rooms = my_rooms;
		//println("WANT_ROOMS_HANDLER->NOT SEND NOW: [%d] to [%d] \n", rank, packet->src);
	}

}

int check_rooms(int src,int rooms) {
	int my_ack_count = get_my_received_ack(1);
	reserved_rooms_array[src] = rooms;
	int reserved_rooms = count_reserved_rooms();
	if (my_ack_count >= size-1 && reserved_rooms <= ROOMS_NUMBER-want_rooms) {
		my_rooms = want_rooms;
		want_rooms = 0;
		reset_reserved(reserved_rooms_array);
		reset_rooms_ack();
		pthread_mutex_unlock(&rooms_mutex);
		//println("HANDLER:: %d process has %d rooms\n", rank, my_rooms);
	}
}

int count_reserved_rooms() {
	int reserved_rooms = 0;
	for (int i=0; i<size; i++) {
		reserved_rooms += reserved_rooms_array[i];
	}
	//println("[%d] RESERVED_ROOMS: %d\n\n\n",rank, reserved_rooms);
	return reserved_rooms;
}


