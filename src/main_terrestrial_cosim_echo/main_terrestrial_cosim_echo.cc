#include <iostream>

#include "ns3/basic-simulation.h"
#include "ns3/topology-ptop.h"
#include "ns3/tcp-optimizer.h"
#include "ns3/arbiter-ecmp-helper.h"
#include "ns3/ipv4-arbiter-routing-helper.h"
#include "ns3/fmu-attached-device-factory.h"
#include "ns3/device-client-factory.h"

using namespace std;
using namespace ns3;

string 
client_send_callback(uint64_t from, uint64_t to) {
    stringstream sstr;
    sstr << "Hello from client " << from << " to device " << to << "!";
    return sstr.str();
}

void 
client_receive_callback(string payload, uint64_t from, uint64_t to) {
    cout << "[Client " << from << "] Received message from device " << to << ": " << payload << endl;
}

string
fmu_do_step_callback(Ptr<RefFMU> fmu, uint64_t nodeId, const string& payload, const double& time, const double& commStepSize) {
    cout << "[Device " << nodeId << "] Received message: " << payload << endl;

    // Call the default callback implementation for stepping the FMU model.
    FmuAttachedDevice::defaultDoStepCallbackImpl(fmu, nodeId, payload, time, commStepSize);

    stringstream sstr;
    sstr << "Value of 'x' is " << fmu->getRealValue("x");
    return sstr.str();
}

int main(int argc, char *argv[]) {

    // No buffering of printf
    setbuf(stdout, nullptr);

    // Retrieve run directory
    CommandLine cmd;
    std::string run_dir = "";
    cmd.Usage("Usage: ./waf --run=\"main_terrestrial_cosim_echo --run_dir='<path/to/run/directory>'\"");
    cmd.AddValue("run_dir",  "Run directory", run_dir);
    cmd.Parse(argc, argv);
    if (run_dir.compare("") == 0) {
        printf("Usage: ./waf --run=\"main_terrestrial_cosim_echo --run_dir='<path/to/run/directory>'\"");
        return 0;
    }

    // Load basic simulation environment
    Ptr<BasicSimulation> basicSimulation = CreateObject<BasicSimulation>(run_dir);

    // Read point-to-point topology, and install routing arbiters
    Ptr<TopologyPtop> topology = CreateObject<TopologyPtop>(basicSimulation, Ipv4ArbiterRoutingHelper());
    ArbiterEcmpHelper::InstallArbiters(basicSimulation, topology);

    // Optimize TCP
    TcpOptimizer::OptimizeUsingWorstCaseRtt(basicSimulation, topology->GetWorstCaseRttEstimateNs());

    // Schedule communication between clients and devices.
    DeviceClientFactory deviceClientFactory(basicSimulation, topology, 
        MakeCallback(&client_send_callback), 
        MakeCallback(&client_receive_callback)); // Requires enable_device_clients=true

    // Add FMU-attached devices
    FmuAttachedDeviceFactory fmuDeviceFactory(basicSimulation, topology, 
        MakeCallback(&fmu_do_step_callback)); // Requires enable_fmu_attached_devices=true
    
    // Run simulation
    basicSimulation->Run();

    // Write device communication results
    deviceClientFactory.WriteResults();

    // Finalize the simulation
    basicSimulation->Finalize();

    return 0;

}
