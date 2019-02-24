//
// Created by mohamed on 07/12/18.
//

#include <stdlib.h>
#include <stdio.h>
#include "train.h"

void station_init(struct station* station) {

    station = malloc(sizeof(struct station));
    station->seatsAvailable = 0;
    station->passengersWaiting = 0;
    station->trainIn = false;

    // initialize the semaphore with zero so that no one will enter until a train has arrived and started
    // signalling that passengers may come
    sem_init(&station->tickets, 0, 0);
    pthread_cond_init(&station->pthreadCondTrainIn, NULL);
    pthread_cond_init(&station->pthreadCondTrainLeave, NULL);


    pthread_mutex_init(&changeStationState, NULL);
    pthread_mutex_init(&permissionToEnter, NULL);

}

void* enter_station(void* station) {

    struct station* param = station;
    int upper = 12, lower = 8;
    int seats = (rand() % (upper - lower + 1)) + lower; // generate a random number of seats between 8 and 12

    // only one train will enter
    pthread_mutex_lock(&permissionToEnter);
    station_load_train(param, seats);
    pthread_mutex_unlock(&permissionToEnter);
    pthread_exit(0);

}

void station_load_train(struct station* station, int count) {

    pthread_t trainId = pthread_self();
    printf("\nTRAIN THREAD =====> train %d is now in the station\n", (int)trainId);

    // update seatsAvailable in station
    pthread_mutex_lock(&changeStationState);
    station->seatsAvailable = count;
    printf("\nTRAIN THREAD =====> seats available after train has just entered: %d\n", station->seatsAvailable);
    /*pthread_mutex_unlock(&changeStationState);

    pthread_mutex_lock(&changeStationState);*/
    station->trainIn = true;
    pthread_mutex_unlock(&changeStationState);
    // signal to passengers who were waiting for a train to come in
    pthread_cond_broadcast(&station->pthreadCondTrainIn);
    printf("TRAIN THREAD =====> train %d signaled for for passengers who were waiting for a train to arrive to come "
           "in\n" ,(int)trainId);


    // signal to waiting passenger threads that more seats became available after updating seatsAvailable in station
    for(int i = 0; i < count; i++) {
        sem_post(&station->tickets);
        printf("TRAIN THREAD =====> train %d has opened its door for a passenger to enter it\n" ,(int)trainId);
        /*int semaphore;
        sem_getvalue(&station->tickets, &semaphore);
        printf("semaphore: %d\n", semaphore);*/
    }

    // wait for train to be fully loaded or there are no passengersWaiting
    pthread_mutex_lock(&changeStationState);
    while((station->passengersWaiting != 0 && station->seatsAvailable != 0)) {
        printf("TRAIN THREAD =====> train %d will now wait to either be filled or there are no passengers are waiting"
               "\n",(int)trainId);
        pthread_cond_wait(&station->pthreadCondTrainLeave, &changeStationState);
    }

    // if the waiting passengers number is zero the semaphore must be reset to represent the correct number of
    // seats available else the semaphore value will be -ve and the next coming train will increment it be however
    // much seats available in it
    station->trainIn = false;
    if(station->passengersWaiting == 0) {
        sem_init(&station->tickets, 0, 0);
    }
    printf("TRAIN THREAD =====> train %d is done waiting\n", (int)trainId);

    printf("TRAIN THREAD =====> train %d has left\n", (int)trainId);
    printf("==================================================================\n");
    pthread_mutex_unlock(&changeStationState);



}

void* station_wait_for_train(void* stationParam) {

    struct station* station = stationParam;
    pthread_t passengerId = pthread_self();
    printf("PASSENGER THREAD =====> passenger %d is now in the station\n", (int)passengerId);

    // indicate that there is a passenger added to the pool of passengers waiting in station
    pthread_mutex_lock(&changeStationState);
    station->passengersWaiting++;
    printf("PASSENGER THREAD =====> passengers waiting: %d\n", station->passengersWaiting);
    pthread_mutex_unlock(&changeStationState);



    // wait if there is no train in the station
    pthread_mutex_lock(&changeStationState);
    while(!station->trainIn) {
        printf("PASSENGER THREAD =====> passenger %d will now wait for a train to signal that it has come\n",
                (int)passengerId);
        pthread_cond_wait(&station->pthreadCondTrainIn, &changeStationState);
    }
    pthread_mutex_unlock(&changeStationState);
    printf("PASSENGER THREAD =====> passenger %d is done waiting\n", (int)passengerId);

    // board train if there are seats available
    printf("PASSENGER THREAD =====> passenger %d will now attempt to board the train\n", (int)passengerId);
    /*int x;
    sem_getvalue(&station->tickets, &x);
    printf("PASSENGER THREAD =====> tickets available semaphore: %d\nPASSENGER THREAD =====>  passenger id: %d\n"
            , x, (int)passengerId);*/
    sem_wait(&station->tickets);

    // only one passenger boards the train at a time

    station_board(station);


    pthread_exit(0);

}

void station_board(struct station* station) {

    pthread_mutex_lock(&changeStationState);
    pthread_t passengerId = pthread_self();
    station->passengersWaiting--;
    station->seatsAvailable--;
    printf("\nPASSENGER THREAD =====> passenger %d has boarded the train\n", (int)passengerId);
    int semaphore;
    sem_getvalue(&station->tickets, &semaphore);
    printf("PASSENGER THREAD =====> passengers waiting: %d\n", station->passengersWaiting);
    printf("PASSENGER THREAD =====> seats available: %d\n", station->seatsAvailable);
    if(station->seatsAvailable == 0 || station->passengersWaiting == 0) {
        //station->seatsAvailable = 0;
        pthread_cond_broadcast(&station->pthreadCondTrainLeave);
    }
    pthread_mutex_unlock(&changeStationState);

}


