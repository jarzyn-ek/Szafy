1. Uruchomienie:
-> make clear
-> make run
2. Działanie:
Mamy następujące stany:
init -----> have_rooms -----> in_lift ----> want_lift_upper -----> finish_state

A) W stanie INIT proces wysyła wiadomość WANT_ROOMS pozostałym procesom i w przypadku
otrzymania od nich (wszystkich pozostałych) wiadomości WANT_ROOMS_ACK oraz ilości
wolnych pokojów odpowiedniej dla procesu, wchodzi do stanu HAVE_ROOMS.

B) W stanie HAVE_ROOMS wysyła wiadomość WANT_LIFT i w przypadku, kiedy otrzyma liczbę
ACK większą lub równą liczba_procesów-liczba_wind, wchodzi do windy, a tym samym do stanu
IN_LIFT.

C) W stanie IN_LIFT przebywa chwilę, następnie zwalnia windę, czeka chwilę, zwalnia pokoje,
resetuje otrzymane wiadomości ACK, przechodzi do stanu WANT_LIFT_UPPER.

D) W stanie WANT_LIFT_UPPER wysyła wiadomość WANT_LIFT, po otrzymaniu odpowiedniej liczby 
ACK czeka jeszcze chwilę, następnie zwalnia windę i zmienia stan na FINISH_STATE.

E) W FINISH_STATE czeka chwilę, a następnie przechodzi do stanu INIT.

3. Handlery:

--WANT_ROOMS
 - Odpowiadamy WANT_ROOMS_ACK jeżeli nie ubiegamy się o pokoje (też, kiedy je mamy),
lub ubiegamy się, ale jesteśmy dalej w kolejce. W przeciwnym wypadku WANT_ROOMS_ACK 
odpowiemy dopiero przy zwalnianiu swoich pokojów.

--WANT_ROOMS_ACK
 - Reagujemy na wiadomość tylko w przypadku, kiedy jesteśmy w stanie INIT, czyli stara-
my się o pokoje. Zwiększamy liczbę otrzymanych ACK, a z pakietu pobieramy informację
o zajmowanych przez proces pokojach. Jeśli liczba wolnych pokojów jest odpowiednia -
zajmujemy je. Jeśli nie, a uzbieraliśmy od WSZYSTKICH procesów ACK na pokoje, czekamy
na wiadomość FREE_ROOMS.

--FREE_ROOMS
 - Wiadomość przydatna w momencie, kiedy mamy pełen komplet ACK na pokoje (czyli jesteśmy
pierwszym w kolejce procesem do zajęcia zasobów w postaci pokoju), ale wolnych pokojów
nie ma wystarczającej ilości. Wtedy aktualizujemy tablicę zajętych pokojów i jeśli 
mamy odpowiednią liczbę wolnych - zajmujemy je. Jeśli nie - czekamy na więcej wiadomości
FREE_ROOMS z nowymi zwolnionymi pokojami.

--WANT_LIFT
 - Jeśli nie ubiegamy się o windę, lub ubiegamy, ale jesteśmy dalej w kolejce to odpowiadamy
WANT_LIFT_ACK. W przeciwnym wypadku odpowiemy WANT_LIFT_ACK dopiero po zwolnieniu windy.

--WANT_LIFT_ACK
 - Jeśli ubiegamy się o windę, to zwiększamy liczbę otrzymanych ACK i sprawdzamy, czy jest
wystarczająca, żeby zająć windę.
