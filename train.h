//
// Created by mohamed on 07/12/18.
//

#ifndef TRAIN_STATION_TRAIN_H
#define TRAIN_STATION_TRAIN_H

#include <stdbool.h>
#include <semaphore.h>
#include <pthread.h>

struct station {

    int passengersWaiting;
    int seatsAvailable;
    bool trainIn;
    sem_t tickets;
    pthread_cond_t pthreadCondTrainIn;
    pthread_cond_t pthreadCondTrainLeave;

};

pthread_mutex_t permissionToEnter;
pthread_mutex_t changeStationState;
struct station station;


/* When a train thread invokes this function when it enters the station through a mutex
 * This function will declare that a train has arrived with a specific number of seats
 * and will return when those seats are filled or there are no passengers waiting */
void station_load_train(struct station* station, int count);

/* This function will notify the train that a passenger has entered by decrementing station.seatsAvailable
 * and decrementing station.passengersWaiting */
void station_board(struct station* station);

/* This function will be called by each passenger thread to wait for an available train to board it */
void* station_wait_for_train(void* stationParam);

/* This function will be called by each thread and will let the threads that called it to call
 * station_load_train() one at a time through permissionToEnter mutex*/
void* enter_station(void* station);

/* This function will initialize an instance of the station*/
void station_init(struct station* station);

#endif //TRAIN_STATION_TRAIN_H
