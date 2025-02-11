#ifndef DEVICE_LOGIC_H
#define DEVICE_LOGIC_H

#include "ns3/fmu-attached-device.h"

#include <string>

struct DeviceLogic {

    static void
    init(
        ns3::Ptr<ns3::RefFMU> fmu, 
        uint64_t nodeId, 
        const std::string& modelIdentifier, 
        const double& startTime
    );

    static std::string
    do_step(
        ns3::Ptr<ns3::RefFMU> fmu, 
        uint64_t nodeId, 
        const std::string& payload, 
        const double& time,
        const double& commStepSize
    );

};

#endif // DEVICE_LOGIC_H