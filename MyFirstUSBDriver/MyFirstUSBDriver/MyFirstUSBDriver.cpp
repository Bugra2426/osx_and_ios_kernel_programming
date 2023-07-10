#include "MyFirstUSBDriver.hpp"
#include <IOKit/IOLib.h>
#include <IOKit/usb/IOUSBHostInterface.h>
#include <USBDriverKit/IOUSBHostInterface.h>
#define super IOService


OSDefineMetaClassAndStructors(MyFirstUSBDriver, IOService)

void logendpoint(const EndpointDescriptor *endpoint)
{
    uint8_t epNumber = StandardUSB::getEndpointNumber(endpoint);
    uint8_t epType = StandardUSB::getEndpointType(endpoint);
    uint8_t epDirection = StandardUSB::getEndpointDirection(endpoint);
    IOLog("Endpoint #%d",epNumber);
    IOLog("--> Type: ");
    switch (epType) {
        case kEndpointTypeControl:
            IOLog("kEndpointTypeControl");
            break;
        case kEndpointTypeIsochronous:
            IOLog("kEndpointTypeIsochronous");
            break;
        case kEndpointTypeBulk:
            IOLog("kEndpointTypeBulk");
            break;
        case kEndpointTypeInterrupt:
            IOLog("kEndpointTypeInterrupt");
            break;
    }
        
    IOLog("--> Direction: ");

    switch (epDirection) {
        case kEndpointDirectionOut:
            IOLog("OUT (kEndpointDirectionOut) ");
            break;
        case kEndpointDirectionIn:
            IOLog("IN (kEndpointDirectionIn) ");
            break;
        case kEndpointDirectionUnknown:
            IOLog("Unknown (kEndpointDirectionUnknown)");
            break;
    }
    IOLog("\n");
}

bool MyFirstUSBDriver::init(OSDictionary *propTable)
{
    IOLog("com_bugratest_MyFirstUSBDriver::init\n");
    return super::init(propTable);
}

IOService* MyFirstUSBDriver::probe(IOService *provider, SInt32 *score)
{
    IOLog("com_bugratest_MyFirstUSBDriver::probe\n");
    return super::probe(provider, score);

}

bool MyFirstUSBDriver::attach(IOService *provider)
{
    IOLog("com_bugratest_MyFirstUSBDriver::attach\n");
    return super::attach(provider);
}


void MyFirstUSBDriver::detach(IOService *provider)
{
    IOLog("com_bugratest_MyFirstUSBDriver::detach\n");
    return super::detach(provider);

}

bool MyFirstUSBDriver::start(IOService *provider)
{
    IOUSBHostInterface *interface;
    IOLog("com_bugratest_MyFirstUSBDriver::start\n");
    if (!super::start(provider)) {
        return false;
    }
    OSDynamicCast(IOUSBHostInterface, provider);
    if (!interface) {
            IOLog("com_bugratest_MyFirstUSBDriver::start -> provider not an IOUSBHostInterface\n");
            return false;
        }
    const StandardUSB::ConfigurationDescriptor *configDesc = interface->getConfigurationDescriptor();
    const StandardUSB::InterfaceDescriptor *ifaceDesc = interface->getInterfaceDescriptor();
    if (!configDesc || !ifaceDesc) {
        IOLog("com_bugratest_MyFirstUSBDriver::start -> descriptors not found\n");
        return false;
    }
    const EndpointDescriptor *ep = NULL;
    while ((ep = StandardUSB::getNextEndpointDescriptor(configDesc, ifaceDesc, ep))) {
        logendpoint(ep);
    }
    return true;

    
}

void MyFirstUSBDriver::stop(IOService *provider)
{
    IOLog("com_bugratest_MyFirstUSBDriver::stop\n");
    return super::stop(provider);

}

bool MyFirstUSBDriver::terminate(IOOptionBits options)
{
    IOLog("com_bugratest_MyFirstUSBDriver::terminate\n");
    return super::terminate(options);

}
