## Introduction

This project consists of a driver system for the ns-3 network simulator. It consists of 3 groups of files: library (`lib-*.cc`) files for common functionality, the main runner (`main.cc`), and a sample program (`sample-wifi-mesh.cc`).

### Library

ns-3 is a complex piece of software with many twiddly knobs. The library aims to reduce some code reuse and abstract away just a *little* of this complexity.

The file names are rather self-explanatory, see inline comments/documentation for details.

### Main

Has stubbed ability to dispatch to samples to make running the whole ensemble easier.

### Sample Wifi Mesh

This is a sample program for running ns-3 and outputting information in the shared documentation format. It creates a wifi network with 5 nodes randomly distributed in a 500x500 meter square. The nodes operate in 802.11 adhoc mode and use OLSR to calculate routes. Running the sample helps show how packets move between layers and the delays/latency between these layers and 802.11 wifi.
