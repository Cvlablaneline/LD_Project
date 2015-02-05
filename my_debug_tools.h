//
//  my_debug_tools.h
//  LD_Project_xcode
//
//  Created by Chienfu Huang on 2014/11/28.
//  Copyright (c) 2014年 ___HCF___. All rights reserved.
//

#ifndef __LD_Project_xcode__my_debug_tools__
#define __LD_Project_xcode__my_debug_tools__

#include <iostream>
#include<highgui.h>

using namespace std;

class dbugTool_Wait {
private:
    int Acount;
    int Ncount;
    bool enable;
public:
    dbugTool_Wait();
    dbugTool_Wait(int num);
    dbugTool_Wait(int num,bool status);
    int touch();
    void stop ();
    void start ();
    bool status ();
};


#endif /* defined(__LD_Project_xcode__my_debug_tools__) */
