# C++ Module Example

Modules:

- zmq-srv - Opens a ZMQ server on port 5556. Can connect to either the ZMQ client, or the DB. The server receives messages and produces 2 random values.
- zmq-client - Opens a ZMQ client that connects to zmq-srv. This is primarily used for testing/debugging
- shmem - Interfaces with the DB using shared memory. Can connect to ZMQ server isntead of Simulink to mimic the simple round-trip example

To compile:
```shell 
g++ zmq-srv.cpp -lzmq -o bin/zmq-srv
g++ zmq-client.cpp -lzmq -o bin/zmq-client
g++ shmem.cpp -o bin/shmem
```

In the following examples, all commands are run from the ARCADE/Connectors/Cpp directory and assumes the DB has been compiled and resides in the ARCADE/DataBroker/Linux directory.

## Simple client / server test
Open a terminal and run:
```shell
./bin/zmq-serv
```
In a second shell run:
```shell
./bin/zmq-client
```

## ZMQ server with DataBroker and Simulink
To run the simple Python example using the zmq-srv instead of the ZMQ python script, ensure input.json is in co-sim mode and using Simulink as the executable.

```json
{
    "Simulator": [
        {
            "executableName": "Simulink",
            "hold_for_dante": "false",
            "co_sim_enable": "true"
        }
    ],
    "cosim": [
        {
            "sync_enable": "true",
            "outputs": "Output_Value_1,Output_Value_2"
        }
    ]
}
```

Next, follow the simple example to load the Simulink model. Instead of running the Python program, execute the following from a shell in the Cpp directory:
```shell
./bin/zmq-shell
```
Finally, start the Simulink simulation.  You should see the values update the same way they updated in the Python example.

## ZMQ server with DataBroker and shmem
To run the simulation using the C++ shmem program instead of Simulink, do the following:

In the Cpp directory, update input.json to use shmem instead of Simulink as shown below.
```json
{
    "Simulator": [
        {
            "executableName": "./bin/shmem",
            "hold_for_dante": "false",
            "co_sim_enable": "true"
        }
    ],
    "cosim": [
        {
            "sync_enable": "true",
            "outputs": "Output_Value_1,Output_Value_2"
        }
    ]
}
```
Start the data broker using the following command:
```shell
../../DataBroker/Linux/DB
```
The data broker should start and wait for an external connection.  It should have an output similar to the output shown below

<details>

```shell
Semaphores Initialized
Flag hold_for_dante = false 
Flag co_sim_enable = true 
Flag sync_enable = true 
Flag realtime_timestep not in config!
Endpoint Initialization Complete
Starting Shm_Interface
Co-Simulation Enabled
Executable Name = ./bin/shmem 
User Control Initializing
***Enter X to stop simulation***

Starting Simulator
Semaphores created by Data_Aggregator
waiting for DA *********************
DA WAITING ON SHMFlag outputs = Output_Value_1,Output_Value_2 
Done waiting for DA
Wait for Semaphore
Entering loop
Semaphore captured
Semaphore captured
Update Points: 2
Publish Points: 2
Timestep Size 0.100000
DA Semaphore captured
Init data written to shared memory
Received from Shm_Interface: PUB = 2, UP = 2, TimeStep = 0.100000
Semaphore captured
Output_Value_1  4.266592 0.000000 sec 
Output_Value_2  16.707166 0.000000 sec 
Input_Value_1 DOUBLE -100000000000000.000000 0.000000 sec 
Input_Value_2 DOUBLE -100000000000000.000000 0.000000 sec 
Output_Value_1  9999999999999999583119736832.000000 0.000000 sec 
Output_Value_2  9999999999999999583119736832.000000 0.000000 sec
```
</details>

In a second shell, start the zmq-server using the following command:
```shell
./bin/zmq-srv
```

You should then see the output of both the zmq-serv and DB windows update with values.