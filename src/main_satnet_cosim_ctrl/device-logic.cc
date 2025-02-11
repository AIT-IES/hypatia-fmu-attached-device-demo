#include "ns3/abort.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/optional/optional.hpp>

#include "device-logic.h"

using namespace std;
using namespace ns3;

void
DeviceLogic::init(
    Ptr<RefFMU> fmu,
    uint64_t nodeId,
    const string& modelIdentifier,
    const double& startTime
) {
    fmippStatus status = fmippFatal;

    // Instantiate the FMU model.
    status = fmu->instantiate(modelIdentifier + to_string(nodeId), 0., false, false);
    NS_ABORT_MSG_UNLESS(status == fmippOK, "instantiation of FMU failed");

    // Change the value of parameter 'k' in FMU model.
    status = fmu->setValue("k", 0.5);
    NS_ABORT_MSG_UNLESS(status == fmippOK, "Setting value of parameter 'k' in FMU model failed");

    // Initialize the FMU model with the specified tolerance.
    status = fmu->initialize(startTime, false, numeric_limits<double>::max());
    NS_ABORT_MSG_UNLESS(status == fmippOK, "initialization of FMU failed");
}

string
DeviceLogic::do_step(
    Ptr<RefFMU> fmu, 
    uint64_t nodeId,
    const string& payload,
    const double& time,
    const double& commStepSize
) {
    // Call the default callback implementation for stepping the FMU model.
    FmuAttachedDevice::defaultDoStepCallbackImpl(fmu, nodeId, payload, time, commStepSize);

    vector<string> measurementRequests;

    try {
        // Convert message string to stream.
        istringstream stream(payload);

        // Create new property tree and parse message stream.
        boost::property_tree::ptree data;
        boost::property_tree::read_xml(stream, data);

        // Retrieve device ID.
        uint64_t deviceId = data.get<uint64_t>("control.id");
        NS_ABORT_MSG_UNLESS(deviceId == nodeId, "Inconsistent device ID");

        boost::optional<boost::property_tree::ptree&> measurementNode = data.get_child_optional("control.measurement");
        if (measurementNode)
        {
            for (const auto& node: *measurementNode) {
                if (node.first == "name") {
                    measurementRequests.push_back(node.second.get_value<string>());
                }
            }
        }

        boost::optional<boost::property_tree::ptree&> targetNode = data.get_child_optional("control.actuate");
        if (targetNode)
        {
            for (const auto& node: *targetNode) {
                if (node.first == "target") {
                    string targetName = node.second.get<string>("name");
                    double targetValue = node.second.get<double>("value");

                    fmippStatus status = fmu->setValue(targetName, targetValue);
                    NS_ABORT_MSG_UNLESS(status == fmippOK, "Setting value of parameter '" + targetName + "' in FMU model failed");
                }
            }
        }
    } catch (const boost::property_tree::xml_parser_error& e) {
        NS_FATAL_ERROR ("Error parsing XML: " << e.what());
    } catch (const boost::property_tree::ptree_error& e) {
        NS_FATAL_ERROR ("Property Tree Error: " << e.what());
    }

    ostringstream os;
    try {
        boost::property_tree::ptree tree;
        tree.put("device.id", nodeId);

        for (const auto& mr: measurementRequests) {
            double value = fmu->getRealValue(mr);
            tree.put("device.measurement." + mr + ".time", time);
            tree.put("device.measurement." + mr + ".value", value);
        }

        boost::property_tree::write_xml(os, tree);
    } catch (const boost::property_tree::xml_parser_error& e) {
        NS_FATAL_ERROR ("Error writing XML: " << e.what());
    }

    return os.str();
}
