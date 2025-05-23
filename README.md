# Examples: FMU-attached Devices for Hypatia

## About

Examples for running Hypatia with [ns-3](https://www.nsnam.org/) application layer models that use a [Functional Mock-up Unit](https://fmi-standard.org/) (FMU) for Co-Simulation (FMI 2.0) to compute its internal state.
This package is primarily intended to work with the ns-3 simulator provided by the [Hpyatia](https://github.com/snkas/hypatia) simulator.
However, it can also be used with standard ns-3 distrubtions.

## Prerequisites

1. Start from a [clean Hypatia installation](https://github.com/snkas/hypatia?tab=readme-ov-file#getting-started).
2. Install the module for [FMU-attached Devices for Hypatia](https://github.com/AIT-IES/hypatia-fmu-attached-device?tab=readme-ov-file#quick-start-ubuntu-2004)

## Run the Examples

### Simple Example: Echo

In this example, a client sends data to an FMU-attached device via a LEO network and awaits the response.
The behavior of the client and the FMU-attached device is implemented via simple callback functions.
When running the simulation, you should see log messages similar to the following:

```
[Device 1252] Received message: Hello from client 1170 to device 1252!
[Client 1170] Received message from device 1252: Value of 'x' is 0.0316975
```

The config files for running the satellite network example are in folder [`demo_satnet_run_dir`](./demo_satnet_run_dir/).
The sources for the main executable are in folder [`src/main_satnet_cosim_echo`](./src/main_satnet_cosim_echo)
The results from the simulation (log files, CSV output from FMU) will be written to folder `demo_satnet_run_dir/logs_ns3`

Run the example from the command line:
``` bash 
./main_satnet_cosim_echo <hypatia-root-dir>
```

**NOTE**: The path to the Hypatia root directory can be relative to the current working directory or absolute

### Advanced Example: Controller

In this example, a client uses a simple hysteresis controller to control the state of an FMU-attached device via a LEO network.
The behavior of the client and the FMU-attached device is implemented via dedicated classes (`DeviceController` and `DeviceLogic`).
Both the client and the device use XML-formatted messages for communication.

![results from controller example](./img/results-ctrl.png)

The config files for running the example are in folder [`demo_satnet_run_dir`](./demo_satnet_run_dir/).
The sources for the main executable are in folder [`src/main_satnet_cosim_ctrl`](./src/main_satnet_cosim_ctrl)
The results from the simulation (log files, CSV output from FMU and controller) will be written to folder `demo_satnet_run_dir/logs_ns3`

Run the example from the command line:
``` bash 
./main_satnet_cosim_ctrl <hypatia-root-dir>
```

**NOTE**: The path to the Hypatia root directory can be relative to the current working directory or absolute

### Advanced Example: Sensor + Actuator + Controller

In this example, a client uses a simple hysteresis controller to control the state of an FMU-attached device via a LEO network.
In contrast to the previous example, the state of the FMU is read from one FMU-attached device (sensor), whereas the control inputs are applied by another FMU-attached device (actuator).
The sensor and the actuator are devices at different network nodes, but they are attached to the same FMU.  
The behavior of the client and the FMU-attached devices is implemented via dedicated classes (`DeviceController` and `DeviceLogic`).
Both the client and the devices use XML-formatted messages for communication (same as in the previous example).

The config files for running the example are in folder [`demo_satnet_shared_run_dir`](./demo_satnet_shared_run_dir/).
The sources for the main executable are in folder [`src/main_satnet_cosim_shared`](./src/main_satnet_cosim_shared)
The results from the simulation (log files, CSV output from FMU and controller) will be written to folder `demo_satnet_shared_run_dir/logs_ns3`

Run the example from the command line:
``` bash 
./main_satnet_cosim_shared <hypatia-root-dir>
```

**NOTE**: The path to the Hypatia root directory can be relative to the current working directory or absolute

### Simple Example: Echo (terrestrial network)

In this example, a client sends data to an FMU-attached device via a terrestrial network and awaits the response.
The behavior of the client and the FMU-attached device is implemented via simple callback functions.
When running the simulation, you should see log messages similar to the following:

```
[Device 598] Received message: Hello from client 498 to device 598!
[Client 498] Received message from device 598: Value of 'x' is 1.9022
```

The config files for running the terrestrial network example are in folder [`demo_terrestrial_run_dir`](./demo_terrestrial_run_dir/).
The sources for the main executable are in folder [`src/main_terrestrial_cosim_echo`](./src/main_terrestrial_cosim_echo)
The results from the simulation (log files, CSV output from FMU) will be written to folder `demo_terrestrial_run_dir/logs_ns3`

Run the example from the command line:
``` bash 
./main_terrestrial_cosim_echo <hypatia-root-dir>
```

**NOTE**: The path to the Hypatia root directory can be relative to the current working directory or absolute
