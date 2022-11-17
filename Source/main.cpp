#include <QApplication>
#include <QPushButton>
#include <iostream>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QMultimedia>


#include "Headers/Log.hpp"
#include "Headers/XMediaEncode.h"
#include "Headers/XRtmp.h"
#include "Headers/XAudioRecord.h"

#include "Headers/XGdigrabScreen.h"

#include "Headers/test.h"

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/time.h"
#include "libswscale/swscale.h"
#include <libavutil/avutil.h>
}

using namespace std;
using namespace DEBUG_LOG;


int avError(int errNum) {
    char buf[1024];
    //获取错误信息
    av_strerror(errNum, buf, sizeof(buf));
    cout << " failed! " << buf << endl;
    return -1;
}

static double r2d(AVRational r) {
    return r.num == 0 || r.den == 0 ? 0. : (double) r.num / (double) r.den;
}

int main(int argc, char *argv[]) {

    QApplication a(argc, argv);
    QPushButton *button = new QPushButton("Hello world!", nullptr);
    button->resize(200, 100);
    button->show();

    const char *inUrl = "luna.flv";
    const char *outUrl = "rtmp://localhost:1935/live/livestream";
    /** region ## flv文件推流测试 ## */

//    //初始化所有封装和解封装   flv mp4 mov mp3
////    av_register_all();
//
//    //初始化网络库
//    avformat_network_init();
//
//    //打开文件,解封文件头
//    //////////////////////////////////////////////////////////////////
//    //                   输入流处理部分
//    /////////////////////////////////////////////////////////////////
//    //打开文件，解封装 avformat_open_input
//    //AVFormatContext **ps  输入封装的上下文。包含所有的格式内容和所有的IO。如果是文件就是文件IO，网络就对应网络IO
//    //const char *url  路径
//    //AVInputFormt * fmt 封装器
//    //AVDictionary ** options 参数设置
//    AVFormatContext *ictx = NULL;
//
//    //打开文件，解封文件头
//    int ret = avformat_open_input(&ictx, inUrl, 0, NULL);
//    if (ret < 0) {
//        return avError(ret);
//    }
//    cout << "avformat_open_input success!" << endl;
//    //获取音频视频的信息 .h264 flv 没有头信息
//    ret = avformat_find_stream_info(ictx, 0);
//    if (ret != 0) {
//        return avError(ret);
//    }
//    //打印视频视频信息
//    //0打印所有  inUrl 打印时候显示，
//    av_dump_format(ictx, 0, inUrl, 0);
//
//    //////////////////////////////////////////////////////////////////
//    //                   输出流处理部分
//    /////////////////////////////////////////////////////////////////
//    AVFormatContext * octx = NULL;
//    //如果是输入文件 flv可以不传，可以从文件中判断。如果是流则必须传
//    //创建输出上下文
//    ret = avformat_alloc_output_context2(&octx, NULL, "flv", outUrl);
//    if (ret < 0) {
//        return avError(ret);
//    }
//    cout << "avformat_alloc_output_context2 success!" << endl;
//    //配置输出流
//    //AVIOcontext *pb  //IO上下文
//    //AVStream **streams  指针数组，存放多个输出流  视频音频字幕流
//    //int nb_streams;
//    //duration ,bit_rate
//
//    //AVStream
//    //AVRational time_base
//    //AVCodecParameters *codecpar 音视频参数
//    //AVCodecContext *codec
//    //遍历输入的AVStream
//    for (int i = 0; i < ictx->nb_streams; i++) {
//        AVStream *in_stream = ictx->streams[i];
//        //创建一个新的流到octx中
//        const AVCodec* codec = avcodec_find_decoder(ictx->streams[0]->codecpar->codec_id);
//        AVStream *out = avformat_new_stream(octx, codec);
//        if (!out) {
//            return avError(0);
//        }
//        //复制配置信息 用于mp4 过时的方法
//        //ret=avcodec_copy_context(out->codec, ictx->streams[i]->codec);
//        ret = avcodec_parameters_copy(out->codecpar, ictx->streams[i]->codecpar);
//        if (ret < 0) {
//            return avError(ret);
//        }
//        out->codecpar->codec_tag = 0;
//
//    }
//    av_dump_format(octx, 0, outUrl, 1);
//
//    //////////////////////////////////////////////////////////////////
//    //                   准备推流
//    /////////////////////////////////////////////////////////////////
//
//    //打开IO
//    ret = avio_open(&octx->pb, outUrl, AVIO_FLAG_WRITE);
//    if (ret < 0) {
//        avError(ret);
//    }
//
//    //写入头部信息
//    ret = avformat_write_header(octx, 0);
//    if (ret < 0) {
//        avError(ret);
//    }
//    cout << "avformat_write_header Success!" << endl;
//
//    //推流每一帧数据
//    //int64_t pts  [ pts*(num/den)  第几秒显示]
//    //int64_t dts  解码时间 [P帧(相对于上一帧的变化) I帧(关键帧，完整的数据) B帧(上一帧和下一帧的变化)]  有了B帧压缩率更高。
//    //uint8_t *data
//    //int size
//    //int stream_index
//    //int flag
//    AVPacket avPacket;
//    AVFrame const *avFrame = av_frame_alloc();
//    //获取当前的时间戳  微妙
//    long long startTime = av_gettime();
//    while (true)
//    {
//        ret = av_read_frame(ictx, &avPacket);
////        avcodec_receive_frame(, avFrame);
////        av_frame_get_best_effort_timestamp(avFrame);
//        if (ret < 0) {
//            break;
//        }
//        cout << avPacket.pts << " " << flush;
//        AVRational itime = ictx->streams[avPacket.stream_index]->time_base;
//        AVRational otime = octx->streams[avPacket.stream_index]->time_base;
//        avPacket.pts=av_rescale_q_rnd(avPacket.pts,itime,otime,(AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
//        avPacket.dts = av_rescale_q_rnd(avPacket.dts, itime, otime, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
//        avPacket.duration = av_rescale_q_rnd(avPacket.duration, itime, otime, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
//        avPacket.pos = -1;
//
//        //视频帧推送速度
//        if (ictx->streams[avPacket.stream_index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
//        {
//            //时间基数
//            AVRational tb = ictx->streams[avPacket.stream_index]->time_base;
//            //已经过去的时间
//            long long now = av_gettime() - startTime;
//            long long dts = 0;
//            dts = avPacket.dts * (1000 * 1000 * r2d(tb));
//            cout << dts << " " << now  <<flush<<endl;
//            if(dts>now)
//                av_usleep(dts-now);
//        }
//        ret = av_interleaved_write_frame(octx,&avPacket);
//        //av_usleep(10*1000);
//        if (ret < 0)
//        {
//            return avError(ret);
//        }
//
//    }
    /** endregion */

    /** region ## 封装测试 ## */
//   const char *outUrl = "rtmp://localhost:1935/live/livestream";
//
//    //编码器和像素格式的转换
//    XMediaEncode *me = XMediaEncode::Get(0);
//
//    //封装和推流对象
//    XRtmp *xr = XRtmp::Get(0);
//
//
//    //编码器上下文
//    AVCodecContext *vc = NULL;
//
//    try
//    {
//        //使用opencv打开本地相机
//        cam.open(0);
//        ///1.打开摄像头
//        if (!cam.isOpened())
//        {
//            throw exception();
//        }
//        cout << "cam open success" << endl;
//        int inWidth = cam.get(CAP_PROP_FRAME_WIDTH);
//        int inHeight = cam.get(CAP_PROP_FRAME_HEIGHT);
//        int fps = cam.get(CAP_PROP_FPS);
//        cout << "fps=" << fps << endl;
//        int ret = 0;
//        ///2.初始化输出转化上下文
//        ///3.初始化输出数据结构
//        me->inWidth = inWidth;
//        me->inHeight = inHeight;
//        me->outHeight = inHeight;
//        me->outWidth = inWidth;
//        me->InitScale();
//
//        ///4.初始化编码上下文
//        //a 找到编码器
//        if (!me->InitVideoCodec())
//        {
//            throw exception();
//        }
//
//
//        ///5 封装器和视频流配置
//        xr->Init(outUrl);
//        //b.添加视频流
//        xr->AddStream(me->vc);
//        ///打开rtmp的网络输出io,写入封装头
//        xr->SendHead();
//
//
//        //读取帧
//        for (;;)
//        {
//            ///只做解码,读取视频帧，解码视频帧
//            if (!cam.grab())
//            {
//                continue;
//            }
//            ///yuv转化为rgb
//            if (!cam.retrieve(frame))
//            {
//                continue;
//            }
//            imshow("video", frame);
//            waitKey(1);
//
//            ///rgb to yuv
//            me->inPixSize = frame.elemSize();
//            AVFrame *yuv = me->RGBToYUV((char *)frame.data);
//            //cout << h <<"" <<flush;
//            if (!yuv)
//            {
//                continue;
//            }
//            ///h264编码
//            AVPacket *pack = me->EncodeVideo(yuv);
//            if (!pack)continue;
//            //推流
//            xr->SendFrame(pack);
//        }
//    }
//    catch (exception &ex)
//    {
//        if (cam.isOpened())
//            cam.release();
//        cerr << ex.what() << endl;
//    }
//
/** endregion */

    /** region ## 对象调用测试 ## */


    int sampleRate = 44100;
    int channels = 2;
    int sampleByte = 2;
    int nbSample = 1024;

//1.配置音频录制线程
    XAudioRecord *ar = XAudioRecord::Get();
//    ar->sampleRate = sampleRate;
//    ar->channels = channels;
//    ar->sampleByte = sampleByte;
//    ar->nbSample = nbSample;
    ar->Init();
    ar->start();

//2.配置、初始化解码器
    XMediaEncode *me = XMediaEncode::Get();
    me->channels = ar->channels;
    me->nbSample = ar->nbSample;
    me->sampleRate = ar->sampleRate;
    me->inSampleFmt = XSampleFMT::X_S16;
    me->outSampleFmt = XSampleFMT::X_FLTP;
    //2.1.初始化操作

    if (!me->InitResample()) {
        return -1;
    }
    if (!me->InitAudioCodec()) {
        return -1;
    }
//3.创建RTMP推流
    XRtmp *pRtmp = XRtmp::Get(0);
    //3.1.RTMP初始化
    if (!pRtmp->Init(outUrl)) {
        return -1;
    }
    //a.添加音频流
    int aindex=pRtmp->AddStream(me->ac);
    if (aindex>0) {
        return -1;
    }

    //3.2.打开rtmp网络IO，发送封装头
    if (!pRtmp->SendHead()) {
        return -1;
    }
//4.创建、配置 推流线程(无限循环) 因为不能阻塞主线程的QT程序,需要另起线程进行数据发送
/*************/

    int readSize = me->nbSample*channels*sampleByte;
    char *buf = new char[readSize];
    for (;;)
    {

        //一次读取一帧音频
        XData ad = ar->Pop();
//        XData vd = xv->Pop();
        if (
                ad.size <= 0
//                &&
//                vd.size<=0
        )
        {
            QThread::msleep(1);
            continue;
        }
        if (ad.size > 0)
        {
            //已经读取一帧源数据
            //重采样数据
            AVFrame *pcm = me->Resample(ad.data);
            ad.Drop();
            AVPacket *pkt = me->EncodeAudio(pcm);
            if (pkt)
            {
                //推流
                if (pRtmp->SendFrame(pkt,aindex))
                {
                    cout << "#" << flush;
                }
            }
        }
//        if (vd.size > 0)
//        {
//            AVFrame *yuv = xe->RGBToYUV(vd.data);
//            vd.Drop();
//            AVPacket *pkt = xe->EncodeVideo(yuv);
//            if (pkt)
//            {
//                //推流
//                if (xr->SendFrame(pkt,vindex))
//                {
//                    cout << "@" << flush;
//                }
//            }
//        }

    }
    delete buf;




/*************/





//    test *t = test::Get();
//    t->setAr(ar);
//    t->setXe(me);
//    t->setRtmp(pRtmp);
//    //4.1.开始推音频流
//    t->start();
//5.屏幕录制线程
//    XMediaEncode *xe_v = XMediaEncode::Get();
//    xe_v->InitVideoCodec();
//
//
//    if (!xe_v->InitResample()) {
//        return -1;
//    }
//    if (!xe_v->InitAudioCodec()) {
//        return -1;
//    }


//    XGdigrabScreen *gScreen = XGdigrabScreen::Get();
////    gScreen->start();
//
//
//    QObject::connect(button, &QPushButton::clicked, [&]() {
//        ar->Stop();
//        cout << "Audio record stop" << endl;
//    });

/** endregion */






    return QApplication::exec();
//    return 0;
}