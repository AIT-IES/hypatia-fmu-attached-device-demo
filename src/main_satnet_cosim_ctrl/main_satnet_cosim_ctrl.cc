#include "ns3/basic-simulation.h"
#include "ns3/topology-satellite-network.h"
#include "ns3/tcp-optimizer.h"
#include "ns3/arbiter-single-forward-helper.h"
#include "ns3/ipv4-arbiter-routing-helper.h"
#include "ns3/gsl-if-bandwidth-helper.h"
#include "ns3/fmu-attached-device-factory.h"
#include "ns3/device-client-factory.h"

#include "device-controller.h"
#include "device-logic.h"

using namespace std;
using namespace ns3;


int 
main(int argc, char *argv[]) {

    // No buffering of printf
    setbuf(stdout, nullptr);

    // Retrieve run directory
    CommandLine cmd;
    string run_dir = "";
    cmd.Usage("Usage: ./waf --run=\"main_satnet_cosim_ctrl --run_dir='<path/to/run/directory>'\"");
    cmd.AddValue("run_dir",  "Run directory", run_dir);
    cmd.Parse(argc, argv);
    if (run_dir.compare("") == 0) {
        printf("Usage: ./waf --run=\"main_satnet_cosim_ctrl --run_dir='<path/to/run/directory>'\"");
        return 0;
    }

    // Load basic simulation environment
    Ptr<BasicSimulation> basicSimulation = CreateObject<BasicSimulation>(run_dir);

    // Setting socket type
    Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::" + basicSimulation->GetConfigParamOrFail("tcp_socket_type")));

    // Optimize TCP
    TcpOptimizer::OptimizeBasic(basicSimulation);

    // Read topology, and install routing arbiters
    Ptr<TopologySatelliteNetwork> topology = CreateObject<TopologySatelliteNetwork>(basicSimulation, Ipv4ArbiterRoutingHelper());
    ArbiterSingleForwardHelper arbiterHelper(basicSimulation, topology->GetNodes());
    GslIfBandwidthHelper gslIfBandwidthHelper(basicSimulation, topology->GetNodes());

    string ctrlResultsFilename = basicSimulation->GetLogsDir() + "/" + "controller.csv";
    DeviceController ctrl(ctrlResultsFilename);

    // Schedule communication between clients and devices.
    DeviceClientFactory deviceClientFactory(basicSimulation, topology, 
        MakeCallback(&DeviceController::send_payload, &ctrl), 
        MakeCallback(&DeviceController::receive_payload, &ctrl)); // Requires enable_device_clients=true

    // Add FMU-attached devices
    FmuAttachedDeviceFactory fmuDeviceFactory(basicSimulation, topology, 
        MakeCallback(&DeviceLogic::init), 
        MakeCallback(&DeviceLogic::do_step)); // Requires enable_fmu_attached_devices=true

    // Run simulation
    basicSimulation->Run();

    // Write device communication results
    deviceClientFactory.WriteResults();

    // Collect utilization statistics
    topology->CollectUtilizationStatistics();

    // Finalize the simulation
    basicSimulation->Finalize();

    return 0;
}
