#include "main.h"

MPI_Datatype MPI_PAKIET_T;
pthread_t threadKom, threadMon; 

int lamport_clock=0;
int my_rooms = 0;
int my_messages_lamport_clocks[] = {[0 ... 1] = 0};
int my_received_ack[] = {[0 ... 1] = 0};

/////////////////////////////////////////////////
                  /////////////

// Inicjalizacja zmiennych z main.h
pthread_mutex_t stateMut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lamport_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t my_messages_lamport_clocks_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lift_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t rooms_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t my_received_ack_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t waiting_for_ack_mutex = PTHREAD_MUTEX_INITIALIZER;

state_t stan = init;
const char* message_strings[] = {"WANT_ROOMS","WANT_ROOMS_ACK", "WANT_LIFT", "WANT_LIFT_ACK"};
const char* state_strings[] = {"init", "have_rooms", "in_lift", "finish_state"};

f_w handlers[4] = {
    [WANT_ROOMS] = want_rooms_handler,
    [WANT_ROOMS_ACK] = want_rooms_ack_handler,
    [WANT_LIFT] = want_lift_handler,
    [WANT_LIFT_ACK] =  want_lift_ack_handler,
};

extern void mainLoop(void);
extern void inicjuj(int *argc, char ***argv);
extern void finalizuj(void);

int main(int argc, char **argv)
{
    /* Tworzenie wątków, inicjalizacja itp */
    inicjuj(&argc,&argv); // tworzy wątek komunikacyjny w "watek_komunikacyjny.c"
    mainLoop();          // w pliku "watek_glowny.c"
    finalizuj();
    return 0;
}

void send_message(message_t message, state_t state){
    packet_t packet;
    packet.ts = get_increased_lamport_clock();

    if(message==WANT_LIFT)
        my_messages_lamport_clocks[0] = packet.ts;
    else if (message == WANT_ROOMS)
        my_messages_lamport_clocks[1] = packet.ts;

    for(int i=0; i<size; i++){
        if(i!=rank){
            sendPacket(&packet, i, message);
            debug("SENDING_REQUEST: %d in %s sends %s to %d\n",rank, state_strings[state], message_strings[message], i);
        }
    }
}

void sendPacket(packet_t *pkt, int destination, message_t message){
    int freepkt=0;
    if (pkt==0) { pkt = malloc(sizeof(packet_t)); freepkt=1;}
    pkt->src = rank;
    pkt->dst = destination;
    int tag = convert_message_to_int(message);
    MPI_Send( pkt, 1, MPI_PAKIET_T, destination, tag, MPI_COMM_WORLD);
    if (freepkt) free(pkt);
}

int convert_message_to_int(message_t message){
    switch (message){
        case WANT_ROOMS:
            return 0;
        case WANT_ROOMS_ACK:
            return 1;
        case WANT_LIFT:
            return 2;
        case WANT_LIFT_ACK:
            return 3;

        default: return -1;
    }
}


void changeState( state_t newState )
{
    pthread_mutex_lock(&stateMut);
    if (end) { 
        pthread_mutex_unlock(&stateMut);
        return;
    }
    stan = newState;
    pthread_mutex_unlock(&stateMut);
}

int get_increased_lamport_clock(){
    pthread_mutex_lock(&lamport_mutex);
        lamport_clock ++;
        int clk = lamport_clock;
    pthread_mutex_unlock(&lamport_mutex);
    return clk;
}

int get_my_messages_lamport_clocks(int index){
    pthread_mutex_lock(&my_messages_lamport_clocks_mutex);
        int clk = my_messages_lamport_clocks[index];
    pthread_mutex_unlock(&my_messages_lamport_clocks_mutex);
    return clk;
}

int get_my_received_ack(int index){
    pthread_mutex_lock(&my_received_ack_mutex);
        int count = my_received_ack[index];
    pthread_mutex_unlock(&my_received_ack_mutex);
    return count;
}

int get_state(){
    pthread_mutex_lock(&stateMut);
        int state = stan;
    pthread_mutex_unlock(&stateMut);
    return state;
}

void my_received_ack_increase(int index){
    pthread_mutex_lock(&my_received_ack_mutex);
        my_received_ack[index]++;
    pthread_mutex_unlock(&my_received_ack_mutex);
}

void reset_reserved(int* array) {
    for (int i = 0; i < size; i++) {
        array[i] = 0;
    }
}


// void pyrkon_escapees_number_increase(){
//     pthread_mutex_lock(&pyrkon_escapees_number_mutex);
//         pyrkon_escapees_number++;
//     pthread_mutex_unlock(&pyrkon_escapees_number_mutex);
// }

// void pyrkon_escapees_number_reset(){
//     pthread_mutex_lock(&pyrkon_escapees_number_mutex);
//         pyrkon_escapees_number=0;
//     pthread_mutex_unlock(&pyrkon_escapees_number_mutex);
// }

void my_messages_lamport_clocks_reset(){
    pthread_mutex_lock(&my_messages_lamport_clocks_mutex);
        memset(my_messages_lamport_clocks, 0, 2*sizeof(int));
    pthread_mutex_unlock(&my_messages_lamport_clocks_mutex);
}

void my_received_ack_reset(){
    pthread_mutex_lock(&my_received_ack_mutex);
        memset(my_received_ack, 0, 2*sizeof(int)); 
    pthread_mutex_unlock(&my_received_ack_mutex);
}

void reset_rooms_ack() {
    pthread_mutex_lock(&my_received_ack_mutex);
        my_received_ack[1] = 0; 
    pthread_mutex_unlock(&my_received_ack_mutex);
}

void waiting_for_ack_reset(){
    pthread_mutex_lock(&waiting_for_ack_mutex);
    waiting_for_ack = malloc(size * sizeof(packet_t));
    waiting_for_rooms_ack = malloc(size * sizeof(packet_t));
    for(int i=0; i<size; i++){
        waiting_for_ack[i].ts = -111;
        waiting_for_rooms_ack[i].ts = -111;
    }
    pthread_mutex_unlock(&waiting_for_ack_mutex);
}

void lamport_clock_reset(){
    pthread_mutex_lock(&lamport_mutex);
        lamport_clock=0;
    pthread_mutex_unlock(&lamport_mutex);
}

void free_my_lift(){
    for (int i = 0; i<size; i++){
        if(waiting_for_ack[i].ts!=-111){
            packet_t packet = waiting_for_ack[i];
            packet.ts = get_increased_lamport_clock();
            debug("HANDLER-external:: %d in %s sends %s to %d with clock %d\n", rank, state_strings[get_state()], message_strings[WANT_LIFT_ACK], packet.src, packet.ts);
            sendPacket(&packet,packet.src,WANT_LIFT_ACK);
        }
    }
}

void free_my_rooms() {
        for (int i = 0; i<size; i++){
            if(waiting_for_rooms_ack[i].ts!=-111){
                packet_t packet = waiting_for_rooms_ack[i];
                packet.ts = get_increased_lamport_clock();
                packet.number_of_rooms = my_rooms;
                debug("HANDLER-external:: %d in %s sends %s number of rooms: %d to %d with clock %d\n", rank, state_strings[get_state()], message_strings[WANT_ROOMS_ACK], packet.number_of_rooms, packet.src, packet.ts);
                sendPacket(&packet,packet.src,WANT_ROOMS_ACK);
        }
    }
}

void set_my_messages_lamport_clocks(int index, int value){
    pthread_mutex_lock(&my_messages_lamport_clocks_mutex);
        my_messages_lamport_clocks[index]=value;
    pthread_mutex_unlock(&my_messages_lamport_clocks_mutex);
}

void set_my_received_ack(int index, int value){
    pthread_mutex_lock(&my_received_ack_mutex);
        my_received_ack[index]=value;
    pthread_mutex_unlock(&my_received_ack_mutex);
}