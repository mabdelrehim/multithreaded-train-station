NAME: MOHAMED ELSAYED ABDELREHIM

TRAIN STATION SIMULATOR

** The program starts by spawning threads for passengers and trains on different threads this way the arrival of
passengers and trains to the station is more randomized. Each train thread will call a function called enter station in
which all train threads that called this function will be assigned a random number of seats between 8 and 12 and will
take turn in calling the function station_load_train. Also each passenger thread will call the function
station_wait_for_train. Once all train threads have returned the program halts and discards the creation of new
passenger threads

** station_load_train:
When a train thread enters this function it will signal that it has arrived by updating the seats available count in
the station, setting a boolean, and broadcasting to all passengers who were waiting for a train to arrive through a
condition variable. (needless to say that updating the state of the station variable is done through a mutex)
After doing that the train will keep signaling a semaphore for whatever number of seats available it has times.
This semaphore functions as a ticket kiosk; no one will be allowed to board the train unless there are tickets available
and if there are no more tickets all passengers waiting at that kiosk will keep waiting until a new train will come and
issue more tickets. After issuing tickets the train threads will lock the mutex and check if there are no more seats
available or no passengers waiting, if not it will unlock that mutex and and keep waiting until someone signals that
condition variable; however if yes it's important that while the mutex is still locked the train un-sets the train in
boolean and if there are no passengers waiting reset the semaphore to its beginning state. This is important
because if the train will not wait and will leave immediately no passenger thread should gain access to the variables in
station and especially the seats available variable. Resetting the semaphore if there are no passengers waiting is
important because of a train comes and leaves and the passengers in that station did not fill that train there will
remain extra tickets at the kiosk (the semaphore) and when the next train issues some tickets the number of tickets
will be larger that the number of its seats available. This case will not happen if there are passengers waiting in the
station because the semaphore will be a negative value.

** station_wait_for_train
When a passenger thread enters this function it will first increase the number of passengers waiting and then check if
there is a train in the station. If yes, it will proceed to the tickets kiosk (the sem_wait) else it will wait for a
train to come and broadcast that it has come. At the kiosk if there are tickets (semaphore >= 0) it will proceed to
station board else it will keep waiting at the kiosk (blocked by the sem_wait) until a new train comes and issues some
new tickets (sem_signals the semaphore).

** station_board
If a train is in the station with an available seats (checks done in station_wait_for_train) it will be allowed to
proceed and call station_board. In that function it will decrement the passengers waiting and the seats available
to signal to the train that the passenger has boarded. The passenger thread will check whether there are no remaining
seats or no remaining passengers in the station. If one of those conditions is true it will signal to the train thread
that it may leave now.



