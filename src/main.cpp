#include <iostream>
#include <PcapPlusPlusVersion.h>
#include <PcapLiveDeviceList.h>
#include <PcapFileDevice.h>
#include <PlatformSpecificUtils.h>
#include <boost/program_options.hpp>

namespace opt = boost::program_options;

void sendPackets(pcpp::PcapLiveDevice *dev, pcpp::RawPacketVector &packetVec, unsigned int timeout, bool repeat)
{
    unsigned int counter = 0;
    unsigned int fail = 0;
    unsigned int cycle_number = 0;
    const uint32_t mtu = dev->getMtu();

    do
    {
        for (auto rawPacket : packetVec)
        {
            counter++;
            if (rawPacket->getRawDataLen() > mtu || !dev->sendPacket(*rawPacket))
            {
                fail++;
                continue;
            }
            if (timeout > 0)
                std::cout << "Sending packet #" << counter << ", lenght:"
                          << rawPacket->getRawDataLen()
                          << std::endl;
            PCAP_SLEEP(timeout / 1000);
        }
        if (repeat)
        {
            std::cout << "Cycle #" << cycle_number << " restarted with " << counter - fail << "/" << counter << " successfully sent packets" << std::endl;
            counter = 0;
            fail = 0;
            cycle_number++;
        }
    } while (repeat);

    std::cout << counter - fail << "/" << counter << " packets were successfully sent" << std::endl;

    packetVec.clear();
}

int main(int argc, char *argv[])
{
    std::string interfaceAddress = "";
    std::string filename = "";
    unsigned int timeout = 0;
    bool repeat = false;

    // Setting parameters up
    opt::options_description desc("All options");
    desc.add_options()("input,f", opt::value<std::string>(&filename), "Path to pcap/pcapng file (required value)")("interface,i", opt::value<std::string>(&interfaceAddress), "Name or IPv4 address of interface (required value)")("timeout,t", opt::value<unsigned int>(&timeout)->default_value(0), "Timeout between sending packets")("repeat,r", "Repeat sending cycle or not")("devices,d", "Get devices list")("help,h", "Show help");

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
        if (vm.count("repeat"))
        {
            repeat = true;
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
        return -1;
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
        std::cout << "Cannot find " << interfaceAddress << " interface";
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

    pcpp::RawPacketVector packetVec;

    reader->getNextPackets(packetVec);
    reader->close();

    sendPackets(dev, packetVec, timeout, repeat);

    dev->close();

    return 0;
}
