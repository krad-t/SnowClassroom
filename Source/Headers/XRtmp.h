//
// Created by krad on 2022/11/15.
//

#ifndef SNOWCLASSROOM_XRTMP_H
#define SNOWCLASSROOM_XRTMP_H

class AVCodecContext;
class AVPacket;
class XRtmp
{
public:
    static XRtmp *Get(unsigned char index = 0);
    //初始化封装器上下文
    virtual bool Init(const char *url) = 0;

    //添加视频或者音频流
    virtual int AddStream(const AVCodecContext *c) = 0;

    //打开rtmp网络IO，发送封装头
    virtual bool SendHead() = 0;

    //rtmp 帧推流
    virtual bool SendFrame(AVPacket *pack,int streamIndex = 0) = 0;
    ~XRtmp();
protected:
    XRtmp();
};



#endif //SNOWCLASSROOM_XRTMP_H
