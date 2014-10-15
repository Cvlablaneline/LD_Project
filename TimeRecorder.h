//
//  TimeRecorder.h
//  LD_Project_xcode
//
//  Created by Chienfu Huang on 2014/8/28.
//  Copyright (c) 2014年 ___HCF___. All rights reserved.
//

#ifndef __LD_Project_xcode__TimeRecorder__
#define __LD_Project_xcode__TimeRecorder__

#include <iostream>

class TimeRecorder
{
public:
    clock_t start,finish;
    double totaltime;
    TimeRecorder();
    void TREnd ();
    ~TimeRecorder();
};
#endif /* defined(__LD_Project_xcode__TimeRecorder__) */
