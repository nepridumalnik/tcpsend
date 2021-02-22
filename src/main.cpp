#include <iostream>
#include "PcapLiveDeviceList.h"
#include "PcapFileDevice.h"
#include "boost/program_options.hpp"

namespace opt = boost::program_options;

int main(int argc, char *argv[])
{
    std::string interfaceAddress = "";
    std::string filename = "";

    // Setting parameters up
    opt::options_description desc("All options");
    desc.add_options()("input,i", opt::value<std::string>(&filename), "Path to pcap/pcapng file")("interface,f", opt::value<std::string>(&interfaceAddress), "Path to pcap/pcapng file")("devices,d", "Get devices list")("help,h", "Show help");

    // Handling parameters
    try
    {
        // Parsing parameters
        opt::variables_map vm;
        opt::store(opt::parse_command_line(argc, argv, desc), vm);
        opt::notify(vm);

        // Check for optional parameters
        if (vm.count("help"))
        {
            std::cout << desc << std::endl;
            return 0;
        }

        if (vm.count("devices"))
        {
            std::cout << "Avialable devices list:" << std::endl;
            const std::vector<pcpp::PcapLiveDevice *> devicesList = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDevicesList();
            for (int i = 0; i < devicesList.size(); i++)
            {
                std::cout << "Device #" << i << ", " << devicesList[i]->getName() << ", address " << devicesList[i]->getIPv4Address().toString() << std::endl;
            }
            std::cout << "Paste IP address with param --interface" << std::endl;
            return 0;
        }

        // Check for critical parameters
        if (!vm.count("input") && !vm.count("interface"))
        {
            throw "input pcap/pcapng file and interface are both not specified";
        }
        if (!vm.count("input"))
        {
            throw "input pcap/pcapng file is not specified";
        }
        if (!vm.count("interface"))
        {
            throw "interface`s IP address is not specified";
        }
    }
    catch (const char *error)
    {
        std::cout << "Error: " << error << std::endl;
        std::cout << "Use --help or -h for help" << std::endl;
        return -1;
    }
    catch (opt::reading_file::error &error)
    {
        std::cout << "Error: " << error.what() << std::endl;
        std::cout << "Use --help or -h for help" << std::endl;
        return -2;
    }

    pcpp::IFileReaderDevice *reader = pcpp::IFileReaderDevice::getReader(filename.c_str());
    if (!reader)
    {
        std::cout << "Cannot determine reader for file type" << std::endl;
        return -1;
    }
    if (!reader->open())
    {
        std::cout << "Cannot open file for reading" << std::endl;
        return -1;
    }
    else
        std::cout << "File " << filename << " opened for reading" << std::endl;
    pcpp::PcapLiveDevice *dev = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp(interfaceAddress.c_str());
    if (!dev)
    {
        std::cout << "Cannot find interface with IPv4 address of " << interfaceAddress;
        return -1;
    }
    else
    {
        if (!dev->open())
        {
            std::cout << "Cannot open device" << std::endl;
            return -1;
        }
    }

    pcpp::RawPacket rawPacket;
    unsigned int counter = 0;
    while (reader->getNextPacket(rawPacket))
    {
        if (!dev->sendPacket(rawPacket))
        {
            std::cout << "Couldn't send packet #" << counter << std::endl;
            return -1;
        }
        counter++;
    }

    std::cout << counter << " packets were successfully sent" << std::endl;

    reader->close();
    dev->close();

    return 0;
}
