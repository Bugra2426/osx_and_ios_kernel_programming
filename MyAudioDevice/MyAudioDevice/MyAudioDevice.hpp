#ifndef MYAUDIODEVICE_HPP
#define MYAUDIODEVICE_HPP
#include <IOKit/audio/IOAudioDevice.h>
#define MyAudioDevice com_bugratest_MyAudioDevice
class MyAudioDevice : public IOAudioDevice
{
    OSDeclareDefaultStructors(MyAudioDevice);
    virtual bool initHardware(IOService *provider) APPLE_KEXT_OVERRIDE;
    bool createAudioEngine();
    static IOReturn volumeChangeHandler(OSObject *target, IOAudioControl *volumeControl, SInt32 oldValue, SInt32 newValue);
    virtual IOReturn volumeChanged(IOAudioControl *volumeControl, SInt32 oldValue, SInt32 newValue);
    static IOReturn outputMuteChangeHandler(OSObject * target, IOAudioControl *muteControl, SInt32 oldValue, SInt32 newValue);
    virtual IOReturn outputMuteChanged(IOAudioControl *muteControl, SInt32 oldValue, SInt32 newValue);
    static IOReturn gainChangeHandler(OSObject *target, IOAudioControl *gainControl, SInt32 oldValue, SInt32 newValue);
    virtual IOReturn gainChanged(IOAudioControl *gainControl, SInt32 oldValue, SInt32 newValue);
    static IOReturn inputMuteChangeHandler(OSObject *target, IOAudioControl *muteControl, SInt32 oldValue, SInt32 newValue);
    virtual IOReturn inputMuteChanged(IOAudioControl *muteControl, SInt32 oldValue, SInt32 newValue);

};

#endif
