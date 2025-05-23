#include "ns3/basic-simulation.h"
#include "ns3/topology-satellite-network.h"
#include "ns3/tcp-optimizer.h"
#include "ns3/arbiter-single-forward-helper.h"
#include "ns3/ipv4-arbiter-routing-helper.h"
#include "ns3/gsl-if-bandwidth-helper.h"
#include "ns3/fmu-shared-device-factory.h"
#include "ns3/device-client-factory.h"

#include "device-controller.h"
#include "device-logic.h"

using namespace std;
using namespace ns3;

namespace {
    template<class T>
    T getSetElementByIndex(const std::set<T>& s, size_t i) {
        return *next(s.begin(), i);
    }
}

int 
main(int argc, char *argv[]) {

    // No buffering of printf
    setbuf(stdout, nullptr);

    // Retrieve run directory
    CommandLine cmd;
    string run_dir = "";
    cmd.Usage("Usage: ./waf --run=\"main_satnet_cosim_shared --run_dir='<path/to/run/directory>'\"");
    cmd.AddValue("run_dir",  "Run directory", run_dir);
    cmd.Parse(argc, argv);
    if (run_dir.compare("") == 0) {
        printf("Usage: ./waf --run=\"main_satnet_cosim_shared --run_dir='<path/to/run/directory>'\"");
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

    std::string config_shared_endpoints = basicSimulation->GetConfigParamOrFail("example_shared_devices");
    std::set<int64_t> shared_endpoints = parse_set_positive_int64(config_shared_endpoints);
    
    NS_ABORT_MSG_UNLESS(2 == shared_endpoints.size(), "This example expects exactly 2 endpoints attached to a shared FMU.");
    int64_t nodeIdMeasure = getSetElementByIndex(shared_endpoints, 0);
    int64_t nodeIdActuate = getSetElementByIndex(shared_endpoints, 1);

    string ctrlResultsFilename = basicSimulation->GetLogsDir() + "/" + "controller.csv";
    DeviceController ctrl(nodeIdMeasure, nodeIdActuate, ctrlResultsFilename);

    // Schedule communication between clients and devices.
    DeviceClientFactory deviceClientFactory(basicSimulation, topology, 
        MakeCallback(&DeviceController::send_payload, &ctrl), 
        MakeCallback(&DeviceController::receive_payload, &ctrl)); // Requires enable_device_clients=true

    // Add devices attached to a shared FMU.
    FmuSharedDeviceFactory fmuDeviceFactory(basicSimulation, topology, 
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
