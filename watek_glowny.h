#ifndef WATEK_GLOWNY_H
#define WATEK_GLOWNY_H

/* pętla główna aplikacji: zmiany stanów itd */
void mainLoop();
void init_behavior();
void in_lift_behavior();
void in_rooms_behavior();
void finish_state_behavior();
void have_rooms_behavior();
void want_lift_upper_behavior();

#endif
