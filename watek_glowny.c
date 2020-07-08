#include "main.h"
#include "watek_glowny.h"

void initialization() {
    my_rooms = 0;
    want_rooms = 0;
    pthread_mutex_lock(&rooms_mutex);
    pthread_mutex_lock(&lift_mutex);
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
            case in_rooms:{
                in_rooms_behavior();
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

    println("**** STAN:: %s ****\n",state_strings[stan]);
    my_received_ack_reset();
    want_rooms = (rand() % (ROOMS_NUMBER))+1;
    send_message(WANT_ROOMS, stan);
    pthread_mutex_lock(&rooms_mutex);
    send_rooms_ack();
    sleep(SEC_IN_STATE);
    changeState(have_rooms);

}

void have_rooms_behavior() {

    println("**** STAN:: %s ****\n",state_strings[stan]);
    send_message(WANT_LIFT, stan);
    pthread_mutex_lock(&lift_mutex);
    sleep(SEC_IN_STATE);
    changeState(in_lift);

}

void in_lift_behavior(){

    println("**** STAN:: %s ****\n",state_strings[stan]);
    sleep(SEC_IN_STATE);
    println("Having fun in lift!!\n");
    free_my_lift();
    changeState(in_rooms);
}

void in_rooms_behavior() {

    println("**** STAN:: %s ****\n",state_strings[stan]);
    sleep(SEC_IN_STATE);
    my_received_ack_reset();
    changeState(want_lift_upper);
}

void want_lift_upper_handler() {

    println("**** STAN:: %s ****\n",state_strings[stan]);
    send_message(WANT_LIFT, stan);
    println("CHCE WROCIC WINDA \n");
    pthread_mutex_lock(&lift_mutex);
    //
    free_my_rooms();
    //
    println("Having fun in lift!!\n");
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