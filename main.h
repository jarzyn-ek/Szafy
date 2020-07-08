#ifndef GLOBALH
#define GLOBALH

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

//delay w danym stanie
#define SEC_IN_STATE 2

#define LIFTS 2
#define ROOMS_NUMBER 4

extern int rank,size;
extern pthread_t threadKom, threadMon; 
extern MPI_Datatype MPI_PAKIET_T;

#define FIELD_NUM 5

typedef struct packet_t{
    int ts;       /* timestamp (zegar lamporta */
    int src;      /* pole nie przesyłane, ale ustawiane w main_loop */
    int data;     /* przykładowe pole z danymi; można zmienić nazwę na bardziej pasującą */
    int dst;
    int number_of_rooms;
} packet_t;

struct packet_t* waiting_for_ack;  
struct packet_t* waiting_for_rooms_ack;
int reserved_rooms_array[4];

int want_rooms;

// PO DOPISANIU NOWEGO stanu KONIECZNA AKTUALIZACJA TABLICY STATE_STRINGS
// W PLIKU MAIN.C
extern const char* state_strings[];
typedef enum {init, have_rooms, in_lift, in_rooms, want_lift_upper, finish_state} state_t;
extern state_t stan;
extern int my_rooms;

// typy wiadomości 
// PO DOPISANIU NOWEGO TYPU KONIECZNA AKTUALIZACJA TABLICY MESSAGE_STRINGS
// W PLIKU MAIN.C ORAZ W FUNKCJI CONVERT_MESSAGE_TO_INT W MAIN.C
extern const char* message_strings[];
typedef enum {WANT_ROOMS, WANT_ROOMS_ACK, WANT_LIFT, WANT_LIFT_ACK, FREE_ROOMS} message_t;

typedef void (*f_w)(packet_t *);
extern f_w handlers[5];

//handlersy z pliku handlersow 
// PO DOPISANIU NOWEGO HANDLERA KONIECZNA AKTUALIZACJA TABLICY HANDLERÓW
// W PLIKU MAIN.C
extern void want_lift_handler(packet_t *packet);
extern void want_rooms_handler(packet_t *packet);
extern void want_lift_ack_handler(packet_t *packet);
extern void want_rooms_ack_handler(packet_t *packet);
extern void free_rooms_handler(packet_t* packet);
extern int check_rooms(int src,int rooms);
extern int count_reserved_rooms();

extern volatile char end;
extern int lamport_clock;

//do przechowywania zegarow z czasem wyslania wlasnych żądań 
extern int my_messages_lamport_clocks[2];
//do przechowywania ilosci uzyskanych ack
extern int my_received_ack[2];

//zamki na globalne wartosci
extern pthread_mutex_t stateMut;
extern pthread_mutex_t lamport_mutex;
extern pthread_mutex_t my_messages_lamport_clocks_mutex;
extern pthread_mutex_t my_received_ack_mutex;
extern pthread_mutex_t lift_mutex;
extern pthread_mutex_t rooms_mutex;
extern pthread_mutex_t waiting_for_ack_mutex;

void send_message(message_t message, state_t state);

void sendPacket(packet_t *pkt, int destination, message_t message);
void changeState( state_t state);
int convert_message_to_int(message_t message);

int get_increased_lamport_clock();
int get_my_messages_lamport_clocks(int index);
int get_my_received_ack(int index);
int get_state();

void free_my_lift();
void free_my_rooms();
void send_rooms_ack();
void my_received_ack_increase(int index);


void my_messages_lamport_clocks_reset();
void my_received_ack_reset();
void waiting_for_ack_reset();
void reset_rooms_ack();
void lamport_clock_reset();

void set_my_messages_lamport_clocks(int index, int value);
void set_my_received_ack(int index, int value);


void reset_reserved(int* array);


                  /////////////
/////////////////////////////////////////////////

#ifdef DEBUG
#define debug(FORMAT, ...) printf("%c[%d;%dm [%d][%d]: " FORMAT "%c[%d;%dm\n", 27, (1 + (rank / 7)) % 2, 31 + (6 + rank) % 7, rank, lamport_clock, ##__VA_ARGS__, 27, 0, 37);
#else
#define debug(...) ;
#endif

#define P_WHITE printf("%c[%d;%dm",27,1,37);
#define P_BLACK printf("%c[%d;%dm",27,1,30);
#define P_RED printf("%c[%d;%dm",27,1,31);
#define P_GREEN printf("%c[%d;%dm",27,1,33);
#define P_BLUE printf("%c[%d;%dm",27,1,34);
#define P_MAGENTA printf("%c[%d;%dm",27,1,35);
#define P_CYAN printf("%c[%d;%d;%dm",27,1,36);
#define P_SET(X) printf("%c[%d;%dm",27,1,31+(6+X)%7);
#define P_CLR printf("%c[%d;%dm",27,0,37);

#define println(FORMAT, ...) printf("%c[%d;%dm [%d]: " FORMAT "%c[%d;%dm\n",  27, (1+(rank/7))%2, 31+(6+rank)%7, rank, ##__VA_ARGS__, 27,0,37);


#endif
