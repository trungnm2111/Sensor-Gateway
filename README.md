# Project Overview

This project is designed to manage sensor data and facilitate communication with a remote server. It includes various components such as logging, queue management, sensor data handling, socket connections, and database interactions.

## Directory Structure

```
├── inc
│   ├── log_manager.h
│   ├── queue_share.h
│   ├── sensor_data_manager.h
│   ├── socket_connection.h
│   └── sql_db.h
├── Makefile
└── src
    ├── log_manager.c
    ├── main.c
    ├── queue_share.c
    ├── sensor_data_manager.c
    ├── socket_connection.c
    └── sql_db.c
```

### inc

- `log_manager.h`: Header file for the logging manager.
- `queue_share.h`: Header file for the queue management.
- `sensor_data_manager.h`: Header file for managing sensor data.
- `socket_connection.h`: Header file for handling socket connections.
- `sql_db.h`: Header file for database interactions.

### src

- `log_manager.c`: Implementation of the logging manager.
- `main.c`: Main entry point of the application.
- `queue_share.c`: Implementation of the queue management.
- `sensor_data_manager.c`: Implementation of sensor data management.
- `socket_connection.c`: Implementation of socket connections.
- `sql_db.c`: Implementation of database interactions.

## Build Instructions

To build the project, use the provided `Makefile`. Run the following command in the project root directory:

```sh
make
```

## Usage

After building the project, you can run the executable to start the application. Ensure that all necessary configurations and dependencies are properly set up.

