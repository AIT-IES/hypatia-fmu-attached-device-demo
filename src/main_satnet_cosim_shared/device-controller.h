#ifndef DEVICE_CONTROLLER_H
#define DEVICE_CONTROLLER_H

#include <fstream>
#include <string>
#include <vector>
#include <map>

class DeviceController {
public:

    DeviceController(
        uint64_t node_id_measure,
        uint64_t node_id_actuate,
        std::string res_filename
    );

    std::string
    send_payload(
        uint64_t from, 
        uint64_t to
    );

    void
    receive_payload(
        std::string payload, 
        uint64_t from, 
        uint64_t to
    );

private:

    void
    write_to_csv(
        const std::vector<std::string>& varnames, 
        const std::vector<double>& data
    ) const;

    uint64_t m_nodeIdMeasure;
    uint64_t m_nodeIdActuate;
    double m_deviceState;
    std::string m_resFilename;
};

#endif // DEVICE_CONTROLLER_H