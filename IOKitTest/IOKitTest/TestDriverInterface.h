//
//  TestDriverInterface.h
//  IOKitTest
//
//  Created by Bugra Karabudak on 23.06.2023.
//

#ifndef TESTDRIVERINTERFACE_H
#define TESTDRIVERINTERFACE_H

typedef struct TimerValue {
    uint64_t time;
    uint64_t timebase;
    
} TimerValue;

enum TimerRequestCode {
    kTestUserClientStartTimer,
    kTestUserClientStopTimer,
    kTestUserGetElapsedTimer,
    kTestUserGetElapsedTimerValue,
    kTestUserClientDelayForMs,
    kTestUserClientDelayForTime,
    kTestUserClientMethodCount
};

#endif /* TESTDRIVERINTERFACE_H */
