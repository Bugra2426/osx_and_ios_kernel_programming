//
//  TestDriverInterface.h
//  IOKitClient
//
//  Created by Bugra Karabudak on 24.06.2023.
//

#ifndef TESTDRIVERINTERFACE_H
#define TESTDRIVERINTERFACE_H
typedef struct TimerValue {
    uint64_t    time;
    uint64_t    timebase;
} TimerValue;

enum TimerRequestCode {
    kTestUserClientStartTimer,
    kTestUserClientStopTimer,
    kTestUserClientGetElapsedTimerTime,
    kTestUserClientGetElapsedTimerValue,
    kTestUserClientDelayForMs,
    kTestUserClientDelayForTime,
    kTestUserClientMethodCount
};

#endif /* TestDriverInterface_h */
