//
//  MyAudioEngine.cpp
//  MyAudioDevice
//
//  Created by Bugra Karabudak on 6.07.2023.
//

#include "MyAudioEngine.hpp"
#include <IOKit/IOLib.h>
#include <IOKit/IOTimerEventSource.h>

#define kAudioSampleRate    48000
#define kAudioNumChannels   2
#define kAudioSampleDepth   16
#define kAudioSampleWidth   16
#define kAudioBufferSampleFrames    kAudioSampleRate/2
#define kAudioSampleBufferSize  ( kAudioBufferSampleFrames * kAudioNumChannels * (kAudioSampleDepth / 8) )

#define kAudioInterruptInterval     10000000 // 10ms = (1000 ms / 100 hz).
#define kAudioInterruptHZ           100

#define super IOAudioEngine

OSDefineMetaClassAndStructors(MyAudioEngine, IOAudioEngine)


bool MyAudioEngine::initHardware(IOService *provider)
{
    bool result = false;
    IOAudioSampleRate initialSampleRate;
    IOAudioStream *audioStream;
    IOWorkLoop* workloop = NULL;
    IOLog("MyAudioEngine[%p]::initHardware(%p)\n", this, provider);
    if (!super::initHardware(provider)) {
        goto done;
    }
    fAudioInterruptSource = IOTimerEventSource::timerEventSource(this, interruptOccured);
    if (!fAudioInterruptSource) {
        return false;
    }
    workloop = getWorkLoop();
    if (!workloop) {
        return false;
    }
    if (workloop->addEventSource(fAudioInterruptSource) != kIOReturnSuccess) {
        
        return false;
    }
    initialSampleRate.whole = kAudioSampleRate;
    initialSampleRate.fraction = 0;
    setDescription("Test Audio Device");
    setSampleRate(&initialSampleRate);
    setNumSampleFramesPerBuffer(kAudioBufferSampleFrames);
    setInputSampleLatency(kAudioSampleRate / kAudioInterruptHZ);
    setOutputSampleOffset(kAudioSampleRate / kAudioInterruptHZ);
    fOutputBuffer = (SInt16 *)IOMalloc(kAudioSampleBufferSize);
    if (!fOutputBuffer) {
        goto done;
    }
    fInputBuffer = (SInt16 *)IOMalloc(kAudioSampleBufferSize);
    if (!fInputBuffer) {
        goto done;
    }
    audioStream = createNewAudioStream(kIOAudioStreamDirectionOutput, fOutputBuffer, kAudioSampleBufferSize);
    if(!audioStream) {
        goto done;
    }
    addAudioStream(audioStream);
    audioStream->release();
    audioStream = createNewAudioStream(kIOAudioStreamDirectionInput, fInputBuffer, kAudioSampleBufferSize);
    if(!audioStream) {
        goto done;
    }
    addAudioStream(audioStream);
    audioStream->release();
    result = true;
done:
    return result;
}

void MyAudioEngine::free()
{
    IOLog("MyAudioEngine[%p]::free()\n", this);
    if(fOutputBuffer) {
        IOFree(fOutputBuffer, kAudioSampleBufferSize);
        fOutputBuffer = NULL;
    }
    if (fInputBuffer) {
        IOFree(fInputBuffer, kAudioSampleBufferSize);
        fInputBuffer = NULL;
    }
    if (fAudioInterruptSource) {
        fAudioInterruptSource->release();
        fAudioInterruptSource = NULL;
    }
    super::free();
}
IOAudioStream *MyAudioEngine::createNewAudioStream(IOAudioStreamDirection direction, void *sampleBuffer, UInt32 sampleBufferSize)
{
    IOAudioStream *audioStream;
    audioStream = new IOAudioStream;
    if (audioStream) {
        if (!audioStream->initWithAudioEngine(this, direction, 1)) {
            audioStream->release();
        } else {
            IOAudioSampleRate rate;
            IOAudioStreamFormat format = {
                2,
                kIOAudioStreamSampleFormatLinearPCM,
                kIOAudioStreamNumericRepresentationSignedInt,
                kAudioSampleDepth,
                kAudioSampleWidth,
                kIOAudioStreamAlignmentHighByte,
                kIOAudioStreamByteOrderBigEndian,
                true,
                0
            };
            audioStream->setSampleBuffer(sampleBuffer, sampleBufferSize);
            rate.fraction = 0;
            rate.whole = kAudioSampleRate;
            audioStream->addAvailableFormat(&format, &rate, &rate);
            audioStream->setFormat(&format);
        }
    }
    return audioStream;
}
void MyAudioEngine::stop(IOService *provider)
{
    IOLog("MyAudioEngine[%p]::stop(%p)\n", this, provider);
    
    if (fAudioInterruptSource) {
        fAudioInterruptSource->cancelTimeout();
        getWorkLoop()->removeEventSource(fAudioInterruptSource);
    }
    super::stop(provider);
}

IOReturn MyAudioEngine::performAudioEngineStart()
{
    UInt64 time, timeNS;
    IOLog("MyAudioEngine[%p]::performAudioEngineStart()\n", this);
    fInterruptCount = 0;
    takeTimeStamp(false);
    fAudioInterruptSource->setTimeoutUS(kAudioInterruptInterval / 1000);
    clock_get_uptime(&time);
    absolutetime_to_nanoseconds(time, &timeNS);
    fNextTimeout = timeNS + kAudioInterruptInterval;
    return  kIOReturnSuccess;
}
IOReturn MyAudioEngine::performAudioEngineStop()
{
    IOLog("MyAudioEngine[%p]::performAudioEngineStop()\n", this);
    fAudioInterruptSource->cancelTimeout();
    
    return kIOReturnSuccess;
}

UInt32 MyAudioEngine::getCurrentSampleFrame()
{
    UInt32 periodCount = (UInt32) fInterruptCount % (kAudioInterruptHZ/2);
    UInt32 sampleFrame = periodCount * (kAudioSampleRate / kAudioInterruptHZ);
    return sampleFrame;
}

IOReturn MyAudioEngine::performFormatChange(IOAudioStream *audioStream, const IOAudioStreamFormat *newFormat, const IOAudioSampleRate *newSampleRate)
{
    IOLog("MyAudioEngine[%p]::peformFormatChange(%p, %p, %p)\n", this, audioStream, newFormat, newSampleRate);
    IOReturn result = kIOReturnSuccess;
    
    // Since we only allow one format, we only need to be concerned with sample rate changes
    // In this case, we only allow 2 sample rates - 44100 & 48000, so those are the only ones
    // that we check for
    if (newSampleRate) {
        switch (newSampleRate->whole) {
            case kAudioSampleRate:
                result = kIOReturnSuccess;
                break;
            default:
                result = kIOReturnUnsupported;
                IOLog("/t Internal Error - unknown sample rate selected.\n");
                break;
        }
    }
    
    return result;
}

IOReturn MyAudioEngine::clipOutputSamples(const void *mixBuf, void *sampleBuf, UInt32 firstSampleFrame, UInt32 numSampleFrames, const IOAudioStreamFormat *streamFormat, IOAudioStream *audioStream)
{
    UInt32 sampleIndex, maxSampleIndex;
    float *floatMixBuf;
    SInt16 *outputBuf;
    
    floatMixBuf = (float *)mixBuf;
    outputBuf = (SInt16 *)sampleBuf;
    
    maxSampleIndex = (firstSampleFrame + numSampleFrames) * streamFormat->fNumChannels;
    
    for (sampleIndex = (firstSampleFrame * streamFormat->fNumChannels); sampleIndex < maxSampleIndex; sampleIndex++) {
        float inSample;
        
        inSample = floatMixBuf[sampleIndex];
        
        if (inSample > 1.0) {
            inSample = 1.0f;
        } else if (inSample < -1.0) {
            inSample = -1.0f;
        }
        
        // Scale the -1.0 to 1.0 range to the appropriate scale for signed 16-bit samples and then
        // convert to SInt16 and store in the hardware sample buffer
        if (inSample >= 0) {
            outputBuf[sampleIndex] = (SInt16) (inSample * 32767.0);
        } else {
            outputBuf[sampleIndex] = (SInt16) (inSample * 32768.0);
        }
    }
    return kIOReturnSuccess;
}

IOReturn MyAudioEngine::convertInputSamples(const void *sampleBuf, void *destBuf, UInt32 firstSampleFrame, UInt32 numSampleFrames, const IOAudioStreamFormat *streamFormat, IOAudioStream *audioStream)
{
    UInt32 numSamplesLeft;
    float *floatDestBuf;
    SInt16 *inputBuf;
    floatDestBuf = (float *)destBuf;
    inputBuf = &(((SInt16 *)sampleBuf)[firstSampleFrame * streamFormat->fNumChannels]);
    numSamplesLeft = numSampleFrames * streamFormat->fNumChannels;
    while (numSamplesLeft > 0) {
        SInt16 inputSample;
        inputSample = *inputBuf;
        if (inputSample >= 0) {
            *floatDestBuf = inputSample / 32767.0f;
            
        } else {
            *floatDestBuf = inputSample / 32768.0f;
        }
        ++inputBuf;
        ++floatDestBuf;
        --numSamplesLeft;
        
    }
    return kIOReturnSuccess;
}
void MyAudioEngine::interruptOccured(OSObject *owner, IOTimerEventSource *sender)
{
    UInt64 thisTimeNS;
    uint64_t time;
    SInt64 diff;
    
    MyAudioEngine *audioEngine = (MyAudioEngine *)owner;
    
    if (audioEngine) {
        audioEngine->handleAudioInterrupt();
    }
    if (!sender) {
        return;
    }
    
    clock_get_uptime(&time);
    absolutetime_to_nanoseconds(time, &thisTimeNS);
    diff = ((SInt64)audioEngine->fNextTimeout - (SInt64)thisTimeNS);
    
    sender->setTimeoutUS((UInt32)(((SInt64)kAudioInterruptInterval + diff) / 1000));
    audioEngine->fNextTimeout += kAudioInterruptInterval;
}

void MyAudioEngine::handleAudioInterrupt()
{
    UInt32 bufferPosition = fInterruptCount % (kAudioInterruptHZ / 2);
    UInt32 samplesBytesPerInterrupt = (kAudioSampleRate / kAudioInterruptHZ) * (kAudioSampleWidth/8) * kAudioNumChannels;
    UInt32 byteOffsetInBuffer = bufferPosition * samplesBytesPerInterrupt;
    
    UInt8 *inputBuf = (UInt8 *)fInputBuffer + byteOffsetInBuffer;
    UInt8 *outputBuf = (UInt8 *)fOutputBuffer + byteOffsetInBuffer;
    
    // Copy samples from the output buffer to the input buffer.
    bcopy(outputBuf, inputBuf, samplesBytesPerInterrupt);
    // Tell the buffer to wrap
    if (bufferPosition == 0) {
        takeTimeStamp();
        // Wrap
    }
    
    fInterruptCount++;
}
