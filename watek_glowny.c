#include "main.h"
#include "watek_glowny.h"

void initialization() {
    pthread_mutex_lock(&rooms_mutex);
    pthread_mutex_lock(&lift_mutex);

    println("[%d] MUTEXES BLOCKED \n", rank);
}

void mainLoop(){
    srandom(rank);
    waiting_for_ack_reset();
    initialization();
    
    while (!end) {

        switch(stan){

            case init:{
                init_behavior();
                break;
            }
            case have_rooms:{
                have_rooms_behavior();
                break;
            }
            case in_lift:{
                in_lift_behavior();
                break;
            }
            case want_lift_upper:{
                want_lift_upper_handler();
                break;
            }
            case finish_state:{
                finish_state_behavior();
                break;
            }
            default: break;
        }
        
        }
}

void init_behavior(){
    my_received_ack_reset();
    want_rooms = (rand() % (ROOMS_NUMBER))+1;
    send_message(WANT_ROOMS, stan);
    //println("[%d] STAN:: %s chce %d pomieszczen \n",rank, state_strings[stan], want_rooms);
    //oczekiwanie na odblokowanie po otrzymaniu odpowiedniej liczby ack
    pthread_mutex_lock(&rooms_mutex);
    sleep(SEC_IN_STATE);
    //println("%d AFTER ROOM_MUTEX\n", rank);
    changeState(have_rooms);
}

void have_rooms_behavior() {
    send_message(WANT_LIFT, stan);

    pthread_mutex_lock(&lift_mutex);
    sleep(SEC_IN_STATE);
    changeState(in_lift);
}

void in_lift_behavior(){
    sleep(SEC_IN_STATE);

    println("Having fun in lift!!\n");
    //println("STAN:: %s \n", state_strings[stan]);

    free_my_lift();
    sleep(SEC_IN_STATE);
    free_my_rooms();
    my_received_ack_reset();
    changeState(want_lift_upper);
}

void want_lift_upper_handler() {
    send_message(WANT_LIFT, stan);

    println("[%d] CHCE WROCIC WINDA", rank);

    pthread_mutex_lock(&lift_mutex);
    sleep(SEC_IN_STATE);
    free_my_lift();
    changeState(finish_state);
}

void finish_state_behavior(){
    println("**** STAN:: %s ****\n",state_strings[stan]);
    sleep(SEC_IN_STATE);
    sleep(SEC_IN_STATE);
    changeState(init);
}