#include "main.h"

//funkcje - reakcje na otrzymanie konkretnych typow komunikatow

void want_lift_handler(packet_t *packet){
	//debug("HANDLER:: %d asks %d about entering Pyrkon\n",packet->src, packet->dst);
	int my_request_clock = get_my_messages_lamport_clocks(0);

	// my_request_clock = -1 oznacza ignorowanie requestow
	//my_request_clock = 0 oznacza ze nie ubiegam sie
	//jesli size==pyrkon_tickets to ostatni nie dostanie zadnej zgody i itak bedzie czekal wiec uwzgledniam go tu
	if ((size<=LIFTS) || ((my_request_clock!=-1) && (my_request_clock==0 || my_request_clock > packet->ts || (my_request_clock == packet->ts && rank > packet->src) ))){
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
	if(get_state()==init){
		//debug("HANDLER:: %d in state %s got ack to enter Pyrkon %d from %d\n", rank, state_strings[get_state()], packet->pyrkon_number, packet->src);
		my_received_ack_increase(0);
		println("[%d] size-LIFTS = %d\n",rank, size-LIFTS);
		int my_ack_count = get_my_received_ack(0);
		if (my_ack_count >= size - LIFTS){
			println("HANDLER:: %d has %d/%d permissions\n",rank, my_ack_count, size)
			set_my_messages_lamport_clocks(0,-1); //w windzie ignorujemy requesty wejscia do windy
			pthread_mutex_unlock(&lift_mutex);
		}
	}
}

void want_rooms_ack_handler(packet_t* packet) {
	my_received_ack_increase(1);
	int my_ack_count = get_my_received_ack(1);
	reserved_rooms_array[packet->src] = packet->number_of_rooms;
	int reserved_rooms = 0;
	if (my_ack_count == size-1) {
		for (int i=0; i<size; i++) {
			reserved_rooms += reserved_rooms_array[i];
		}
		if (reserved_rooms <= ROOMS_NUMBER-want_rooms) {
			my_rooms = want_rooms;
			want_rooms = 0;
			reset_reserved(reserved_rooms_array);
			reset_rooms_ack();
			set_my_messages_lamport_clocks(1,-1);
			pthread_mutex_unlock(&rooms_mutex);
			println("HANDLER:: %d process has %d rooms\n", rank, my_rooms);
		}
	}
}

void want_rooms_handler(packet_t* packet) {
	int my_request_clock = get_my_messages_lamport_clocks(1);

	//oczekuję, ale jestem dalej w kolejce|............
	//|...............|..............|. nie oczekuję.........|..............
	//|...............|..............|.........|.............|....oczekuję z tym samym miejscem w kolejce ale mam wyższy rank.|..
	//|...............|..............|.........|.............|.................................|..............................|.
	//|...............v..............|.........v.............|.................................v..............................|.
	if ((my_request_clock > packet->ts || my_request_clock <= 0) || (my_request_clock == packet->ts && rank > packet->src)) {
		packet->number_of_rooms = my_rooms;
		packet->ts = get_increased_lamport_clock();
		sendPacket(packet, packet->src, WANT_ROOMS_ACK);
	} else {
		waiting_for_rooms_ack[packet->src].ts = packet->ts;
		waiting_for_rooms_ack[packet->src].src = packet->src;
		waiting_for_rooms_ack[packet->src].data = packet->data;
		waiting_for_rooms_ack[packet->src].dst = packet->dst;
	}

}

void exit_rooms_handler(packet_t* packet) {

}

// void init_handler(packet_t* packet) {

// }

// void in_lift_handler(packet_t* packet) {

// }

void finish_handler(packet_t* packet) {
	
}

