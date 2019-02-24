#include <stdio.h>

#include "train.h"


void* spawn_trains(void* numberOfTrains);
void* spawn_passengers(void* numberOfPassengers);

int main() {

    station_init(&station);
    int numberOfTrains = 10, numberOfPassengers = 100;

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_t spawnTrains, spawnPassengers;

    pthread_create(&spawnTrains, &attr, spawn_trains, &numberOfTrains);
    pthread_create(&spawnPassengers, &attr, spawn_passengers, &numberOfPassengers);

    pthread_join(spawnTrains, NULL);

    // if all trains have came and left the station stop passengers from entering
    pthread_cancel(spawnPassengers);


    return 0;
}

void* spawn_trains(void* numberOfTrains) {

    int num = *(int*)numberOfTrains;
    pthread_t trains[num];

    for (int i = 0; i < num; i++) {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&trains[i], &attr, enter_station, &station);
    }

    for (int i = 0; i < num; i++) {
        pthread_join(trains[i], NULL);
    }

    pthread_exit(0);

}

void* spawn_passengers(void* numberOfPassengers) {

    int num = *(int*)numberOfPassengers;
    pthread_t passengers[num];

    for (int i = 0; i < num; i++) {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&passengers[i], &attr, station_wait_for_train, &station);
    }

    for (int i = 0; i < num; i++) {
        pthread_join(passengers[i], NULL);
    }

    pthread_exit(0);

}