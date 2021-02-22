#include <iostream>
#include "PcapLiveDeviceList.h"
#include "PcapFileDevice.h"
#include "boost/program_options.hpp"

namespace opt = boost::program_options;

int main(int argc, char *argv[])
{
    opt::options_description desc("All options");
    std::string filename = "";

    desc.add_options()("input,i", opt::value<std::string>(&filename), "Path to pcap/pcapng file")("help,h", "Show help");

    opt::variables_map vm;

    opt::store(opt::parse_command_line(argc, argv, desc), vm);

    opt::notify(vm);

    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
        return 0;
    }

    return 0;

    std::string interfaceIPAddr = "192.168.1.74";
    pcpp::IFileReaderDevice *reader = pcpp::IFileReaderDevice::getReader(filename.c_str());
    if (!reader)
    {
        printf("Cannot determine reader for file type\n");
        return -1;
    }
    if (!reader->open())
    {
        printf("Cannot open file for reading\n");
        return -1;
    }
    else
        printf("File \"%s\" opened for reading\n", filename.c_str());
    pcpp::PcapLiveDevice *dev = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp(interfaceIPAddr.c_str());
    if (!dev)
    {
        printf("Cannot find interface with IPv4 address of '%s'\n", interfaceIPAddr.c_str());
        return -1;
    }
    else
    {
        if (!dev->open())
        {
            printf("Cannot open device\n");
            return -1;
        }
        else
            printf("Network device initialized\n");
    }

    pcpp::RawPacket rawPacket;
    int counter = 0;
    while (reader->getNextPacket(rawPacket))
    {
        printf("Captured file #%d, size %d >>", ++counter, rawPacket.getFrameLength());
        if (!dev->sendPacket(rawPacket))
        {
            printf("Couldn't send packet #%d\n", counter);
            return -1;
        }
        else
            printf(" send succeed!\n");
    }

    reader->close();
    dev->close();

    return 0;
}