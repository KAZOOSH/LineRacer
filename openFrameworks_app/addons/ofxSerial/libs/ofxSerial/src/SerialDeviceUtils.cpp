//
// Copyright (c) 2010 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:    MIT
//


#include "ofx/IO/DeviceFilter.h"
#include "ofx/IO/RegexPathFilter.h"
#include "ofx/IO/SerialDeviceUtils.h"
#include "ofx/IO/PathFilterCollection.h"
#include "Poco/Exception.h"
#include "serial/serial.h"


namespace ofx {
namespace IO {


SerialDeviceInfo::SerialDeviceInfo(const std::string& port,
                                   const std::string& description,
                                   const std::string& hardwareId):
    _port(port),
    _description(description),
    _hardwareId(hardwareId)
{
}
    
    
SerialDeviceInfo::~SerialDeviceInfo()
{
}
    

std::string SerialDeviceInfo::port() const
{
    return _port;
}
    

const std::string& SerialDeviceInfo::getPort() const
{
    return _port;
}
    

std::string SerialDeviceInfo::description() const
{
    return _description;
}


const std::string& SerialDeviceInfo::getDescription() const
{
    return _description;
}

    
std::string SerialDeviceInfo::hardwareId() const
{
    return _hardwareId;
}


const std::string& SerialDeviceInfo::getHardwareId() const
{
    return _hardwareId;
}


SerialDeviceInfo::DeviceList SerialDeviceUtils::listDevices(const std::string& regexPattern,
                                                            int regexOptions,
                                                            bool regexStudy)
{
    std::vector<SerialDeviceInfo> devices;

    std::unique_ptr<Poco::RegularExpression> pRegex = nullptr;

    if (!regexPattern.empty())
    {
        try
        {
            pRegex = std::make_unique<Poco::RegularExpression>(regexPattern,
                                                               regexOptions,
                                                               regexStudy);
        }
        catch (const Poco::RegularExpressionException& exception)
        {
            ofLogError("SerialDeviceUtils::listDevices") << exception.displayText();
        }
    }

    auto ports = serial::list_ports();

    for (const auto& portInfo: serial::list_ports())
    {
        if (pRegex == nullptr || (pRegex != nullptr && pRegex->match(portInfo.port)))
        {
            devices.push_back(SerialDeviceInfo(portInfo.port,
                                               portInfo.description,
                                               portInfo.hardware_id));
        }
    }

    std::sort(devices.begin(), devices.end(), sortDevices);

    return devices;
}


bool SerialDeviceUtils::sortDevices(const SerialDeviceInfo& device0,
                                    const SerialDeviceInfo& device1)
{
    int score0 = 0;
    int score1 = 0;

    // Larger scores mean a given device will show up earlier in the list.

    // Give points for not being Bluetooth ports.
    score0 += !ofIsStringInString(device0.port(), "Bluetooth") ? 1 : 0;
    score1 += !ofIsStringInString(device1.port(), "Bluetooth") ? 1 : 0;

    // Give points for being a USB driver on Linux.
    score0 += ofIsStringInString(device0.port(), "ttyUSB") ? 1 : 0;
    score1 += ofIsStringInString(device1.port(), "ttyUSB") ? 1 : 0;

    // Give extra points for being a 2303 driver.
    score0 += ofIsStringInString(device0.port(), "2303") ? 1 : 0;
    score1 += ofIsStringInString(device1.port(), "2303") ? 1 : 0;

    // Give extra points for being an FTDI driver.
    score0 += ofIsStringInString(device0.description(), "FTDI") ? 1 : 0;
    score1 += ofIsStringInString(device1.description(), "FTDI") ? 1 : 0;

    // Give extra points for being an FTDI driver.
    score0 += ofIsStringInString(device0.port(), "usbserial") ? 1 : 0;
    score1 += ofIsStringInString(device1.port(), "usbserial") ? 1 : 0;

    // Give extra points to Arduino devices.
    score0 += ofIsStringInString(device0.port(), "usbmodem") ? 2 : 0;
    score1 += ofIsStringInString(device1.port(), "usbmodem") ? 2 : 0;

    // Give extra points to Arduino devices.
    score0 += ofIsStringInString(device0.description(), "Arduino") ? 3 : 0;
    score1 += ofIsStringInString(device1.description(), "Arduino") ? 3 : 0;

    // If the scores are equal in the end, use standard sorting on the port.
    if (score0 == score1)
    {
        return device0.port() < device1.port();
    }
    else
    {
        return score0 > score1;
    }
}


} } // namespace ofx::IO
