//
// Created by nbfynbfy on 2020/8/6.
//

#ifndef DEMO_VIDEOCUTTER_H
#define DEMO_VIDEOCUTTER_H

#ifdef __cplusplus
extern "C"{
#endif
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#include "libavutil/opt.h"
#include "libswscale/swscale.h"
#ifdef __cplusplus
};
#endif


class VideoCutter {

private:
    struct VideoConfig_t {
        int index;        // 视频流index
        int width;        // 宽
        int height;       // 高
        int format;       // 数据类型
        int rotation;     // 旋转角度
        int alpha;        // 透明度（vpx）
        AVRational sar;          //
        AVRational time_base;    //
        AVRational frame_rate;   // 帧率
        int64_t duration;
        char name[256];
    };
    char *input_url_;
    char *output_url_;
    int start_time_;//ms
    int end_time_;//ms

public :
    VideoCutter() {
        input_url_ = nullptr;
        output_url_ = nullptr;
        start_time_ = -1;
        end_time_ = -1;
    }

    VideoCutter(char* input_url, char* output_url, int start_time, int end_time) {
        input_url_ = input_url;
        output_url_ = output_url;
        start_time_ = start_time;
        end_time_ = end_time;
    }

    ~VideoCutter() {
        delete (input_url_);
        delete (output_url_);
    }

    void work();

    char *GetInputURL() { return input_url_; }

    void SetInputURL(char *input_url) { input_url_ = input_url; }

    char *GetOutputURL() { return output_url_; }

    void SetOutputURL(char *output_url) { output_url_ = output_url; }

    int GetStartTime() { return start_time_; }

    void SetStartTime(int start_time) { start_time_ = start_time; }

    int GetEndTime() { return end_time_; }

    void SetEndTime(int end_time) { end_time_ = end_time; }

    void SetTime(int start_time, int end_time) { start_time_ = start_time, end_time_ = end_time; }

private:
    void Check();

    //初始化文件读取句柄
    int InitInputFmtContext(AVFormatContext *&input_pFmtContext);

    int64_t ExtractStartTime(AVStream *stream);

    void PacketRescaleTs(AVPacket *pkt, AVRational tbSrc, AVRational tbDst);

    int64_t TimestampRescaleTs(int64_t timestamp, AVRational tbSrc, AVRational tbDst);

    void InitParams(AVFormatContext *&pFmtContext, VideoConfig_t &tVideoConfig);

    int GetRotation(AVStream *st);

    int GetAlpha(AVStream *st);

    void GetSar(AVFormatContext* &pFmtContext, VideoConfig_t &tVideoConfig, AVFrame* frame, int* num, int* den);

    int openCodecContext(const AVFormatContext *pFormatCtx, int *pStreamIndex, enum AVMediaType type, AVCodecContext **ppCodecCtx);

    bool OpenVideo(const char *url, AVFormatContext* &output_pFmtContext, AVRational &output_videoTimeBase, int &output_nVideoIndex,
                   bool &output_bFaststart, AVCodecContext* &pVideoCtx, int w, int h, int fps, int bps, int crf, const char* preset);

    bool InitVideoEncoder(AVCodecContext* &pVideoCtx, int w, int h, int fps, int bps, int crf, const char* preset);

    bool InitWriter(const char* url, AVFormatContext* &output_pFmtContext, AVRational &output_videoTimeBase, int &output_nVideoIndex,
                    bool &output_bFaststart, AVCodecContext* &videoCtx, AVCodecContext* audioCtx);

    void EncodeVideoFrame(AVCodecContext* &videoCtx, AVFrame* frame, AVFormatContext* &output_pFmtContext, AVRational output_videoTimeBase, bool output_bCombine, int output_nVideoIndex);

    int WriteVideoPacket(AVFormatContext* &output_pFmtContext, AVPacket *pkt,AVRational output_videoTimeBase,
                         bool output_bCombine, int output_nVideoIndex);
};
#endif //DEMO_VIDEOCUTTER_H
