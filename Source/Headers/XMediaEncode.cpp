//
// Created by krad on 2022/11/15.
//

#include "XMediaEncode.h"
#include <iostream>

extern "C"
{
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include "libswresample/swresample.h"
}
#pragma comment(lib,"swscale.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"swresample.lib")
using namespace std;

#if defined WIN32 || defined _WIN32
#include <windows.h>
#endif
//获取CPU数量
static int XGetCpuNum()
{
#if defined WIN32 || defined _WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);

    return (int)sysinfo.dwNumberOfProcessors;
#elif defined __linux__
    return (int)sysconf(_SC_NPROCESSORS_ONLN);
#elif defined __APPLE__
	int numCPU = 0;
	int mib[4];
	size_t len = sizeof(numCPU);

	// set the mib for hw.ncpu
	mib[0] = CTL_HW;
	mib[1] = HW_AVAILCPU;  // alternatively, try HW_NCPU;

						   // get the number of CPUs from the system
	sysctl(mib, 2, &numCPU, &len, NULL, 0);

	if (numCPU < 1)
	{
		mib[1] = HW_NCPU;
		sysctl(mib, 2, &numCPU, &len, NULL, 0);

		if (numCPU < 1)
			numCPU = 1;
	}
	return (int)numCPU;
#else
	return 1;
#endif
}

class CXMediaEncode :public XMediaEncode
{
public:
    void Close()
    {
        if (vsc)
        {
            sws_freeContext(vsc);
            vsc = NULL;
        }
        if (yuv)
        {
            av_frame_free(&yuv);
        }
        if (asc)
        {
            swr_free(&asc);
        }
        if (vc)
        {
            avcodec_free_context(&vc);
        }
        if (vc)
        {
            avcodec_free_context(&vc);
        }
        if (pcm)
        {
            av_frame_free(&pcm);
        }
        vpts = 0;
        apts = 0;
        av_packet_unref(&vpack);
        av_packet_unref(&apack);
    }
    bool InitVideoCodec()
    {
        ///4.初始化编码上下文
        //a 找到编码器
        AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_H264);
        if (!codec)
        {
            cout << "Can't find h264 encoder!";
            return false;
        }
        //b 创建编码器上下文
        vc = avcodec_alloc_context3(codec);
        if (!vc)
        {
            cout<<"avcodec_alloc_context3 failed!"<<endl;
        }
        //c 配置编码器参数
        vc->flags |= AV_CODEC_FLAG_GLOBAL_HEADER; //全局参数

        cout << "cpunum=" << XGetCpuNum() << endl;
        vc->thread_count = XGetCpuNum();

        vc->codec_id = codec->id;
        vc->bit_rate = 50 * 1024 * 8; //压缩后每秒视频的bit位大小  200kB
        vc->width = outWidth;
        vc->height = outHeight;
        vc->time_base = { 1,fps };
        vc->framerate = { fps,1 };

        //画面组的大小，多少帧一个关键帧
        vc->gop_size = 50;
        //设置不需要b帧
        vc->max_b_frames = 0;
        //设置像素格式
        vc->pix_fmt = AV_PIX_FMT_YUV420P;
        //d 打开编码器上下文
        int ret = avcodec_open2(vc, 0, 0);
        if (ret != 0)
        {
            char buf[1024] = { 0 };
            av_strerror(ret, buf, sizeof(buf) - 1);
            cout << buf << endl;
            return false;
        }
        cout << "avcodec_open2 success!" << endl;
        return true;
    }


    bool InitAudioCodec()
    {
        ///4 初始化音频编码器
        if (!CreateCodec(AV_CODEC_ID_AAC))
        {
            return false;
        }

        //音频的参数
        ac->bit_rate = 40000;
        ac->sample_rate = sampleRate;
        ac->sample_fmt = AV_SAMPLE_FMT_FLTP;
        ac->channels = channels;
        ac->channel_layout = av_get_default_channel_layout(channels);
        //打开编码器
        return OpenCodec(&ac);
    }
    AVPacket *EncodeVideo(AVFrame *frame)
    {
        av_packet_unref(&vpack);
        frame->pts = vpts;
        vpts++;
        int ret = avcodec_send_frame(vc, frame);
        if (ret != 0)
        {
            return NULL;
        }

        ret = avcodec_receive_packet(vc, &vpack);
        if (ret != 0 || vpack.size < 0)
        {
            return NULL;
        }
        return &vpack;
    }

    AVPacket *EncodeAudio(AVFrame *frame)
    {
        pcm->pts = apts;
        apts += av_rescale_q(pcm->nb_samples, { 1,sampleRate }, ac->time_base);

        int ret = avcodec_send_frame(ac, pcm);
        if (ret != 0)return NULL;

        av_packet_unref(&apack);
        ret = avcodec_receive_packet(ac, &apack);
        //cout << "avcodec_receive_packet   " << ret << endl;
        if (ret != 0)return NULL;
        return &apack;
    }
    bool InitScale()
    {
        ///2.初始化格式转换上下文
        vsc = sws_getCachedContext(vsc,
                //源宽、高、像素格式
                                   inWidth, inHeight, AV_PIX_FMT_BGR24,
                //目标宽、高、像素格式
                                   outWidth, outHeight, AV_PIX_FMT_YUV420P,
                                   SWS_BICUBIC,  //尺寸变化使用的算法
                                   0, 0, 0
        );

        if (!vsc)
        {
            cout << "sws_getCachedContext failed";
            return false;
        }

        ///3.输出的数据结构
        yuv = av_frame_alloc();
        yuv->format = AV_PIX_FMT_YUV420P;
        yuv->width = inWidth;
        yuv->height = inHeight;
        yuv->pts = 0;
        //分配yuv空间
        int ret = av_frame_get_buffer(yuv, 32);
        if (ret != 0)
        {
            char buf[1024] = { 0 };
            av_strerror(ret, buf, sizeof(buf) - 1);
            throw exception();
        }
        return true;
    }

    bool InitResample()
    {
        asc = NULL;
        asc = swr_alloc_set_opts(asc,
                                 av_get_default_channel_layout(channels), (AVSampleFormat)outSampleFmt, sampleRate,               //输出格式
                                 av_get_default_channel_layout(channels), (AVSampleFormat)inSampleFmt, sampleRate,//输入格式
                                 0, 0);
        if (!asc)
        {
            cout << "swr_alloc_set_opts failed!" << endl;
            return false;
        }
        int ret = swr_init(asc);
        if (ret != 0)
        {
            char err[1024] = { 0 };
            av_strerror(ret, err, sizeof(err) - 1);
            cout << err << endl;
            return false;
        }
        cout << "音频重采样 上下文初始化成功" << endl;

        ///3 音频重采样输出空间分配
        pcm = av_frame_alloc();
        pcm->format = outSampleFmt;
        pcm->channels = channels;
        pcm->channel_layout = av_get_default_channel_layout(channels);
        pcm->nb_samples = nbSample;//一帧音频一通道的采样数量
        ret = av_frame_get_buffer(pcm, 0); //给pcm分配存储空间
        if (ret != 0)
        {
            char err[1024] = { 0 };
            av_strerror(ret, err, sizeof(err) - 1);
            cout << err << endl;
            return false;
        }
        return true;
    }
    AVFrame *RGBToYUV(char *rgb)
    {
        ///rgb to yuv
        //3.初始化输入的数据结构
        uint8_t *indata[AV_NUM_DATA_POINTERS] = { 0 };
        //indata[0] bgrbgrbgr
        //plane  indata[0] bbbbb indata[1]ggggg indata[2] rrrrr
        indata[0] = (uint8_t*)rgb;
        int insize[AV_NUM_DATA_POINTERS] = { 0 };
        //一行(宽)数据的字节数
        insize[0] = inWidth*inPixSize;
        int h = sws_scale(vsc, indata, insize, 0, inHeight,//源数据
                          yuv->data, yuv->linesize);
        if (h <= 0)
        {
            return NULL;
        }

        return yuv;
    }
    AVFrame* Resample(char *data)
    {
        const uint8_t *indata[AV_NUM_DATA_POINTERS] = { 0 };
        indata[0] = (uint8_t *)data;
        //已经读取一帧源数据
        //重采样数据
        int len = swr_convert(asc, pcm->data, pcm->nb_samples,//输出参数,输出存储地址，样本数
                              indata, pcm->nb_samples
        );
        if (len <= 0)
        {
            return NULL;
        }
        return pcm;
    }
private:
    bool OpenCodec(AVCodecContext **c)
    {
        int ret = avcodec_open2(*c, 0, 0);
        if (ret != 0)
        {
            char err[1024] = { 0 };
            av_strerror(ret, err, sizeof(err) - 1);
            cout << err << endl;
            avcodec_free_context(c);
            return false;
        }
        cout << "avcodec_open2 success!" << endl;
        return true;
    }
    bool CreateCodec(AVCodecID cid)
    {
        ///4 初始化编码器
        AVCodec *codec = avcodec_find_encoder(cid);
        if (!codec)
        {
            cout << "avcodec_find_encoder failed!" << endl;
            return false;
        }
        //音频编码器上下文
        ac = avcodec_alloc_context3(codec);
        if (!ac)
        {
            cout << "avcodec_alloc_context3 failed!" << endl;
            getchar();
            return false;
        }
        cout << "avcodec_alloc_context3 success!" << endl;

        ac->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        ac->thread_count = XGetCpuNum();
        return true;
    }
    SwsContext *vsc = NULL;         //像素格式转换上下文
    AVFrame *yuv = NULL;            //输出的yuv
    SwrContext *asc = NULL;         //音频重采样上下文
    AVPacket vpack = {0};           //视频帧
    AVPacket apack = { 0 };			//音频帧
    AVFrame *pcm = NULL;            //重采样输出的pcm
    int vpts = 0;
    int apts = 0;
};

XMediaEncode *XMediaEncode::Get(unsigned char index)
{
    static bool isFirst = true;
    if (isFirst)
    {
        //注册所有的编解码器
//        avcodec_register_all();
        isFirst = false;
    }
    static CXMediaEncode cxm[255];
    return &cxm[index];
}

XMediaEncode::XMediaEncode()
{
}


XMediaEncode::~XMediaEncode()
{
}