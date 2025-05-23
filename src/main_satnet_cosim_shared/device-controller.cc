#include "ns3/simulator.h"
#include "ns3/abort.h"
#include "ns3/exp-util.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <sstream>

#include "device-controller.h"

using namespace std;
using namespace ns3;

DeviceController::DeviceController(
    uint64_t node_id_measure,
    uint64_t node_id_actuate,
    std::string res_filename
) : m_nodeIdMeasure(node_id_measure),
    m_nodeIdActuate(node_id_actuate),
    m_resFilename(res_filename)
{
    remove_file_if_exists(m_resFilename);
}

string 
DeviceController::send_payload(
    uint64_t from, 
    uint64_t to
) {
    boost::property_tree::ptree payload;
    payload.add("control.id", to);

    if (to == m_nodeIdMeasure)
    {
        payload.add("control.measurement.name", "x");
        payload.add("control.measurement.name", "k");
    } 
    else if (to == m_nodeIdActuate)
    {
        if (m_deviceState >= 0.5) {
            payload.add("control.actuate.target.name", "k");
            payload.add("control.actuate.target.value", -0.5);
        } else if (m_deviceState <= -0.5) {
            payload.add("control.actuate.target.name", "k");
            payload.add("control.actuate.target.value", 0.5);
        }
    }

    ostringstream os;
    try {
        boost::property_tree::write_xml(os, payload);
    } catch (const boost::property_tree::xml_parser_error& e) {
        NS_FATAL_ERROR ("Error writing XML: " << e.what());
    }
    return os.str();
}

void 
DeviceController::receive_payload(
    string payload, 
    uint64_t from, 
    uint64_t to
) {
    if (to == m_nodeIdMeasure) {
        try {
            // Convert message string to stream.
            istringstream stream(payload);

            // Create new property tree and parse message stream.
            boost::property_tree::ptree data;
            boost::property_tree::read_xml(stream, data);

            // Retrieve measurements.
            double t = data.get<double>("device.measurement.x.time");
            double x = data.get<double>("device.measurement.x.value");

            // Retrieve device ID.
            uint64_t deviceId = data.get<uint64_t>("device.id");
            NS_ABORT_MSG_UNLESS(deviceId == to, "Inconsistent device ID");

            // Store device state (for later use in callback for sending data).
            m_deviceState = x;

            // Write to CSV output file.
            write_to_csv({"time", "x"}, {t, x});
        } catch (const boost::property_tree::xml_parser_error& e) {
            NS_FATAL_ERROR ("Error parsing XML: " << e.what());
        } catch (const boost::property_tree::ptree_error& e) {
            NS_FATAL_ERROR ("Property Tree Error: " << e.what());
        }
    }
}


void 
DeviceController::write_to_csv(
    const vector<string>& varnames, 
    const vector<double>& data) const
{
    // Open the file in append mode.
    ofstream file(m_resFilename, ios::app);

    if (!file.is_open()) { NS_FATAL_ERROR ("Failed to open file: " << m_resFilename); }

    // If the file is empty, write the column names.
    file.seekp(0, ios::end);
    if (0 == file.tellp()) {
        // Names of all other columns.
        for (size_t i = 0; i < varnames.size(); ++i) {
            file << varnames[i];
            if (i < varnames.size() - 1) { file << ","; }
        }
        file << "\n";
    }

    // Write all other rows.
    for (size_t i = 0; i < varnames.size(); ++i)
    {
        file << data[i];
        if (i < varnames.size() - 1) { file << ","; }
    }
    file << "\n";

    // Close the file.
    file.close();
}
