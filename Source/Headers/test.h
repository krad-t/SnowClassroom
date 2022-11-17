//
// Created by krad on 2022/11/15.
//

#ifndef SNOWCLASSROOM_TEST_H
#define SNOWCLASSROOM_TEST_H

#include <QThread>
#include "XAudioRecord.h"
#include "XMediaEncode.h"
#include "XRtmp.h"
#include <QAudioInput>
extern "C"{
#include "libavutil/frame.h"
}
class test:public QThread{
protected:
    XAudioRecord *ar;
    XMediaEncode *xe;
    XRtmp *rtmp;
public:
    test();
public:
    static test* Get();

    virtual void setAr(XAudioRecord *ar) = 0;

    virtual void setXe(XMediaEncode *xe) = 0;

    virtual void setRtmp(XRtmp *rtmp) = 0;

    virtual ~test();
};
#endif //SNOWCLASSROOM_TEST_H
