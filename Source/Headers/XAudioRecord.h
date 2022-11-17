//
// Created by krad on 2022/11/15.
//

#ifndef SNOWCLASSROOM_XAUDIORECORD_H
#define SNOWCLASSROOM_XAUDIORECORD_H

#include <QThread>
enum XAUDIOTYPE
{
    X_AUDIO_QT
};
struct XData
{
    char *data = NULL;
    int size = 0;
    void Drop()
    {
        if(data)
            delete data;
        data = NULL;
        size = 0;
    }
};
class QAudioInput;
class QIODevice;
class XAudioRecord:public QThread
{
public:
    QAudioInput *input = NULL;

    QIODevice *io = NULL;

    int sampleRate = 44100;//样本率
    int channels = 2;//声道数
    int sampleByte = 2;//样本大小
    int nbSample = 1024;//一帧音频每个通道的样本数量

    static XAudioRecord *Get(XAUDIOTYPE type= X_AUDIO_QT,unsigned char index = 0);

    //调用者清理空间
    virtual XData Pop() = 0;
    //开始录制
    virtual bool Init()=0;

    //停止录制
    virtual void Stop()=0;

    virtual ~XAudioRecord();
protected:
    XAudioRecord();

};



#endif //SNOWCLASSROOM_XAUDIORECORD_H
