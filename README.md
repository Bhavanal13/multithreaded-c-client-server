# Multithreaded C Client-Server

A multithreaded client-server application developed in C to demonstrate
TCP socket communication, concurrent client handling, and thread
synchronization.

## Features

- TCP client-server communication
- Socket programming in C
- Multiple client connections
- Multithreaded client handling
- Mutex-based synchronization
- Request and response communication
- Error handling for socket operations

## Concepts Covered

- Client-Server Architecture
- TCP/IP Networking
- Socket Programming
- Multithreading
- Mutex Synchronization
- Concurrent Client Handling
- Inter-Process Communication

## Technologies

- C
- Linux
- TCP/IP
- POSIX Sockets
- Threads
- Mutex

## Architecture

The server creates a TCP socket, binds it to a port, listens for
incoming connections, and accepts client connections.

Each client creates a socket and connects to the server. The client
then sends requests and receives responses from the server.

```text
              ┌───────────────┐
              │     Server    │
              │               │
              │ TCP Socket    │
              │     │         │
              └─────┼─────────┘
                    │
          ┌─────────┼─────────┐
          │         │         │
       Client 1  Client 2  Client 3
          │         │         │
       Thread 1  Thread 2  Thread 3
