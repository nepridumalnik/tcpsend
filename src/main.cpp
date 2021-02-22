#include "stdlib.h"
#include "PcapLiveDeviceList.h"
#include "PcapFileDevice.h"

int main(int argc, char *argv[])
{
    char *filename = "../../captured2.pcapng";
    std::string interfaceIPAddr = "192.168.1.74";
    pcpp::IFileReaderDevice *reader = pcpp::IFileReaderDevice::getReader(filename);
    if (!reader)
    {
        printf("Cannot determine reader for file type\n");
        exit(1);
    }
    if (!reader->open())
    {
        printf("Cannot open file for reading\n");
        exit(1);
    }
    else
        printf("File \"%s\" opened for reading\n", filename);
    pcpp::PcapLiveDevice *dev = pcpp::PcapLiveDeviceList::getInstance().getPcapLiveDeviceByIp(interfaceIPAddr.c_str());
    if (!dev)
    {
        printf("Cannot find interface with IPv4 address of '%s'\n", interfaceIPAddr.c_str());
        exit(1);
    }
    else
    {
        if (!dev->open())
        {
            printf("Cannot open device\n");
            exit(1);
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
            exit(1);
        }
        else
            printf(" send succeed!\n");
    }

    reader->close();
    dev->close();

    return 0;
}