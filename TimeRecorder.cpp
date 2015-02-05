//
//  TimeRecorder.cpp
//  LD_Project_xcode
//
//  Created by Chienfu Huang on 2014/8/28.
//  Copyright (c) 2014年 ___HCF___. All rights reserved.
//

#include "TimeRecorder.h"
using namespace std;
TimeRecorder::TimeRecorder()
{
    start=clock();
    cout << "時間記錄器開始" << endl;
}

void TimeRecorder::TREnd ()
{
    finish=clock();
    totaltime=(double)(finish-start)/CLOCKS_PER_SEC;
    cout<<"\n時間記錄結束\n此次執行時間"<<totaltime<<"秒！"<<"\nFPS為："<<1/totaltime<<endl;
}
TimeRecorder::~TimeRecorder()
{
    cout << "ＸＸＸＸＸＸＸＸＸＸＸＸＸＸＸＸＸＸ" << endl;
}