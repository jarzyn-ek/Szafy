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
            case in_lift:{
                in_lift_behavior();
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
    println("STAN:: %s \n", state_strings[stan]);
    //sleep(SEC_IN_STATE);

    want_rooms = (rand() % (ROOMS_NUMBER));
    send_message(WANT_ROOMS, stan);

    //oczekiwanie na odblokowanie po otrzymaniu odpowiedniej liczby ack
    pthread_mutex_lock(&rooms_mutex);

    println("%d AFTER ROOM_MUTEX\n", rank);

    send_message(WANT_LIFT, stan);

    pthread_mutex_lock(&lift_mutex);

    changeState(in_lift);
}

void in_lift_behavior(){
    sleep(SEC_IN_STATE);

    int num = (rand() % (100 - 0 + 1));
    println("Having fun in lift!!\n");
    if (num%8==0) {
        println("STAN:: %s \n", state_strings[stan]);
        changeState(finish_state);
    }
}

void finish_state_behavior(){
    println("STAN:: %s \n",state_strings[stan]);
    sleep(SEC_IN_STATE);

    // pthread_mutex_lock(&lift_mutex);

    free_my_lift();
    // send_message(OUT_OF_LIFT, stan);

    sleep(SEC_IN_STATE);
    changeState(init);
}