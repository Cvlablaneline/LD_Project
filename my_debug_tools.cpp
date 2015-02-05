//
//  my_debug_tools.cpp
//  LD_Project_xcode
//
//  Created by Chienfu Huang on 2014/11/28.
//  Copyright (c) 2014年 ___HCF___. All rights reserved.
//

#include "my_debug_tools.h"

dbugTool_Wait::dbugTool_Wait()
{
    Acount = 1;
    Ncount = 0;
    enable = false;
    cout << "--- dbugTool_Wait initialization is complete ---\n";
}
dbugTool_Wait::dbugTool_Wait(int num)
{
    Acount = num;
    Ncount = 0;
    enable = false;
    cout << "--- dbugTool_Wait initialization is complete ---\n";
}
dbugTool_Wait::dbugTool_Wait(int num,bool status)
{
    Acount = num;
    Ncount = 0;
    enable = status;
    cout << "--- dbugTool_Wait initialization is complete ---\n";
}

int dbugTool_Wait::touch()
{
    if (enable == true)
    {
        Ncount++;
        if (Ncount >= Acount)
        {
            cvWaitKey();
            Ncount = 0;
            return Acount;
        }
        return Ncount;
    }
    else
    {
        cout << "--- dbugTool_Wait was not start ---\n";
        return -1;
    }
}
void dbugTool_Wait::stop ()
{
    enable = false;
    cout << "--- dbugTool_Wait was stopped ---\n";
}
void dbugTool_Wait::start ()
{
    enable = true;
    cout << "--- dbugTool_Wait was started ---\n";
}
bool dbugTool_Wait::status ()
{
    return enable;
}