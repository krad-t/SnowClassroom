//
// Created by krad on 2022/11/15.
//

#ifndef SNOWCLASSROOM_XGDIGRABSCREEN_H
#define SNOWCLASSROOM_XGDIGRABSCREEN_H
#include <iostream>
#include <QThread>
extern "C"{
    #include "libavdevice/avdevice.h"
};


class XGdigrabScreen: public QThread{
public:
    static XGdigrabScreen* Get(unsigned int index = 0);
    virtual int GrabScreen() = 0;
};


#endif //SNOWCLASSROOM_XGDIGRABSCREEN_H
