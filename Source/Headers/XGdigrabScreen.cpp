//
// Created by krad on 2022/11/15.
//

#include "XGdigrabScreen.h"

extern "C" {
#include "libavutil/time.h"
}

using namespace std;

class CXGdigrabScreen : public XGdigrabScreen {
private:
    AVInputFormat *ifmt;
    AVFormatContext *ictx;

    AVFormatContext *octx;
    const char *outUrl = "rtmp://localhost:1935/live/livestream";

    static double r2d(AVRational r) {
        return r.num == 0 || r.den == 0 ? 0. : (double) r.num / (double) r.den;
    }

protected:
    void run() override {
        GrabScreen();
        int ret;
        ret = avformat_alloc_output_context2(&octx, NULL, "bmp", outUrl);
        if (ret < 0) {
            cout << "alloc_output_context2 fail" << endl;
        }

        for (int i = 0; i < ictx->nb_streams; i++) {
            AVStream *in_stream = ictx->streams[i];
            //创建一个新的流到octx中
            const AVCodec *codec = avcodec_find_decoder(ictx->streams[0]->codecpar->codec_id);
            AVStream *out = avformat_new_stream(octx, codec);

//            ictx->streams[0]->codecpar->width = 640;
//            ictx->streams[0]->codecpar->height = 480;

            if (!out) {
                cout << "avformat_new_stream fail" << endl;
            }
            //复制配置信息 用于mp4 过时的方法
            //ret=avcodec_copy_context(out->codec, ictx->streams[i]->codec);
            ret = avcodec_parameters_copy(out->codecpar, ictx->streams[i]->codecpar);
            if (ret < 0) {
                cout << "avcodec_parameters_copy fail" << endl;
            }

            out->codecpar->codec_tag = 0;

        }
        av_dump_format(octx, 0, outUrl, 1);

        ret = avio_open(&octx->pb, outUrl, AVIO_FLAG_WRITE);
        if (ret < 0) {
            cout << "avio_open fail" << endl;
        }

        ret = avformat_write_header(octx, 0);
        if (ret < 0) {
            cout << "avformat_write_header Fail!\t" << ret << endl;
        } else {
            cout << "avformat_write_header Success!\t" << ret << endl;
        }

//推流每一帧数据
        /**
        *    //int64_t pts  [ pts*(num/den)  第几秒显示]
        *    //int64_t dts  解码时间 [P帧(相对于上一帧的变化) I帧(关键帧，完整的数据) B帧(上一帧和下一帧的变化)]  有了B帧压缩率更高。
        *    //uint8_t *data
        *    //int size
        *    //int stream_index
        *    //int flag
        **/
        AVPacket avPacket;
        AVFrame const *avFrame = av_frame_alloc();
        //获取当前的时间戳  微妙
        long long startTime = av_gettime();
        while (true) {
            ret = av_read_frame(ictx, &avPacket);
            if (ret == 0) { cout << "av_read_frame success" << endl; } else { cout << "av_read_frame fail" << endl; }

            //        avcodec_receive_frame(, avFrame);
            //        av_frame_get_best_effort_timestamp(avFrame);
            if (ret < 0) {
                break;
            }
            cout << avPacket.pts << " " << flush;
            AVRational itime = ictx->streams[avPacket.stream_index]->time_base;
            AVRational otime = octx->streams[avPacket.stream_index]->time_base;
            avPacket.pts = av_rescale_q_rnd(avPacket.pts, itime, otime,
                                            (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            avPacket.dts = av_rescale_q_rnd(avPacket.dts, itime, otime,
                                            (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            avPacket.duration = av_rescale_q_rnd(avPacket.duration, itime, otime,
                                                 (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            avPacket.pos = -1;

            //视频帧推送速度
            if (ictx->streams[avPacket.stream_index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                //时间基数
                AVRational tb = ictx->streams[avPacket.stream_index]->time_base;
                //已经过去的时间
                long long now = av_gettime() - startTime;
                long long dts = 0;
                dts = avPacket.dts * (1000 * 1000 * r2d(tb));
                cout << dts << " " << now << flush << endl;
                if (dts > now)
                    av_usleep(dts - now);
            }
            ret = av_interleaved_write_frame(octx, &avPacket);
            //av_usleep(10*1000);
            if (ret < 0) {
                cout << "have no frame to send" << endl;
            }

        }
    }

public:
    CXGdigrabScreen() = default;

    int GrabScreen() override {
        avdevice_register_all();
        //分配上下文
        ictx = avformat_alloc_context();

//        AVInputFormat *ifmt=av_find_input_format("vfwcap");
//        avformat_open_input(&pFormatCtx, 0, ifmt,NULL);

        //Use gdigrab
        AVDictionary *options = NULL;
        //Set some options
        //grabbing frame rate  帧率设置
        av_dict_set(&options, "framerate", "10", 0);
        //The distance from the left edge of the screen or desktop  位置设置
        //av_dict_set(&options,"offset_x","20",0);
        //The distance from the top edge of the screen or desktop
        //av_dict_set(&options,"offset_y","40",0);` XW
        //Video frame size. The default is to capture the full screen  尺寸设置
        av_dict_set(&options, "video_size", "640x480", 0);

        ifmt = av_find_input_format("gdigrab");
        if (avformat_open_input(&ictx, "desktop", ifmt, &options) != 0) {
            printf("Couldn't open input stream.（无法打开输入流）\n");
            return -1;
        }
    }
};


XGdigrabScreen *XGdigrabScreen::Get(unsigned int index) {
    static CXGdigrabScreen gdigrabScreen[255];
    return &gdigrabScreen[index];
}


