//
// Created by krad on 2022/11/15.
//
#include "test.h"
#include "Log.hpp"
using namespace DEBUG_LOG;
class Ctest:public test{
public:
    void run() override {

        int i = 0;
        while (true)
        {
            int readSize = xe->nbSample*ar->channels*ar->sampleByte;
            char *buf = new char[readSize];
            Log("readSize:",readSize,"nums:",i++);
            //一次读取一帧音频
            if (ar->input->bytesReady() < readSize)
            {
                QThread::msleep(1);
                continue;
            }
            int size = 0;
            while (size != readSize)
            {
                int len = ar->io->read(buf + size, readSize - size);
                if (len < 0)break;
                size += len;
            }
            if (size != readSize)continue;
            QThread::msleep(50);
            //已经读取一帧源数据
            //重采样数据
            AVFrame *pcm = xe->Resample(buf);
            //pts运算
            //nb_sample/sample_rate =一帧音频的秒数
            //time_base pts=sec * timebase.den
            AVPacket *pkt=xe->EncodeAudio(pcm);

            if (!pkt)continue;
            //cout << pkt->size << " " << flush;
            //推流
            rtmp->SendFrame(pkt);
    //        if (ret == 0)
    //        {
    //            cout << "#" << flush;
    //        }
        }
    }

    void setAr(XAudioRecord *ar) {
        this->ar = ar;
    }

    void setXe(XMediaEncode *xe) {
        this->xe = xe;
    }

    void setRtmp(XRtmp *rtmp) {
        this->rtmp = rtmp;
    }

};

test* test::Get() {
    return new Ctest();
}

test::test() {

}

test::~test() {

}

