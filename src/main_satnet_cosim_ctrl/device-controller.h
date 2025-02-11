#ifndef DEVICE_CONTROLLER_H
#define DEVICE_CONTROLLER_H

#include <fstream>
#include <string>
#include <vector>
#include <map>

class DeviceController {
public:

    DeviceController(
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

    std::map<uint64_t, double> m_deviceState;
    std::string m_resFilename;
};

#endif // DEVICE_CONTROLLER_H