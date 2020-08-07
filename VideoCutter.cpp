//
// Created by nbfynbfy on 2020/8/6.
//

#define FFMPEG_TIME_BASE {1, 1000000000}

#include "VideoCutter.h"
#include "glog/logging.h"
#include "thread"

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

void VideoCutter::work() {
    Check();

    AVCodecContext  *input_pVideoCtx = nullptr;
    AVCodecContext  *output_pVideoCtx = nullptr;
    AVCodecContext  *input_pAudioCtx = nullptr;
    AVFormatContext *input_pFmtContext = nullptr;
    AVFormatContext *output_pFmtContext = nullptr;
    AVRational       output_videoTimeBase;
    AVRational       start_timeBase = AVRational {1,1000};
    VideoConfig_t    input_tVideoConfig;   // 视频参数
    VideoConfig_t    output_tVideoConfig;   // 视频参数
    int64_t          nBaseTimeV = 0;
    int64_t          nBaseTimeA = 0;
    int64_t          input_nBaseTime;      // 取音视频第一个packet的时间最小值
    int              output_nVideoIndex;
    bool             output_bFaststart;
    bool             output_bCombine;


    //读取文件句柄
    if ( InitInputFmtContext(input_pFmtContext) < 0 ) {
        return ;
    }

    //寻找流
    if ( avformat_find_stream_info(input_pFmtContext, nullptr) < 0 ) {
        LOG(ERROR) << "avformat_open_input failed. url = " << input_url_;
        return ;
    }

    input_tVideoConfig.index = av_find_best_stream(input_pFmtContext, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);

    if (input_tVideoConfig.index >= 0) {
        AVStream* st = input_pFmtContext->streams[input_tVideoConfig.index];
        nBaseTimeV = ExtractStartTime(st);
    }

    input_nBaseTime = nBaseTimeV;

    InitParams(input_pFmtContext, input_tVideoConfig);
    output_tVideoConfig = input_tVideoConfig;

    int videoStreamIdx = -1;
    int audioStreamIdx = -1;
    //寻找并打开解码器
    openCodecContext(input_pFmtContext, &videoStreamIdx, AVMEDIA_TYPE_VIDEO, &input_pVideoCtx);
    openCodecContext(input_pFmtContext, &audioStreamIdx, AVMEDIA_TYPE_AUDIO, &input_pAudioCtx);

    const char *preset =  "superfast";
    OpenVideo(output_url_, output_pFmtContext, output_videoTimeBase, output_nVideoIndex, output_bFaststart,
              output_pVideoCtx, input_tVideoConfig.width, input_tVideoConfig.height, 25, 0, 0, preset);

    AVPacket *pPacket = NULL;
    AVFrame *pFrame = NULL;
    AVFrame *pFirstFrame = NULL;
    bool is_start = false;

    while (true) {
        pPacket = av_packet_alloc();
        int ret = av_read_frame(input_pFmtContext, pPacket);

        if (ret < 0) {
            LOG(INFO) << "av_read_frame end or error";
            break;
        }

        if (pPacket->stream_index == videoStreamIdx) {
            pFrame = av_frame_alloc();
            int ret = avcodec_send_packet(input_pVideoCtx, pPacket);
            if (ret < 0) {
                continue;
            }

            ret = avcodec_receive_frame(input_pVideoCtx, pFrame);
            if (ret < 0) {
                LOG(ERROR) << "video avcodec_receive_frame error";
                continue;
            }

            pFrame->pict_type = AV_PICTURE_TYPE_NONE;
            static int64_t  out_pts = 0;
            pFrame->pts = out_pts;
            out_pts += 40;

            LOG(INFO)<<"* * * * * * video * * * * * * * * *";
            LOG(INFO)<<"___height: [" << pFrame->height << "]";
            LOG(INFO)<<"____width: [" << pFrame->width << "]";
            LOG(INFO)<<"pict_type: [" << pFrame->pict_type << "]";
            LOG(INFO)<<"___format: [" << pFrame->format << "]";

            if (!is_start) {
                if (pFrame->pts < start_time_ * 1000) {
                    pFirstFrame = pFrame;
                    pFirstFrame->pts =TimestampRescaleTs(start_time_, start_timeBase,
                                                         input_pFmtContext->streams[input_tVideoConfig.index]->time_base);
                } else {
                    is_start = true;
                    pFirstFrame->pts -= (int64_t) start_time_ * 1000;
                    pFirstFrame->pts = (pFirstFrame->pts > 0)? pFirstFrame->pts : 0;
                    EncodeVideoFrame(output_pVideoCtx, pFirstFrame, output_pFmtContext, output_videoTimeBase, output_bCombine, output_nVideoIndex);
                }
            }
            if (is_start
                && pFrame->pts > start_time_ * 1000
                && pFrame->pts < end_time_ * 1000) {
                pFrame->pts -= (int64_t) start_time_ * 1000;
                pFrame->pts = (pFrame->pts > 0)? pFrame->pts : 0;
                EncodeVideoFrame(output_pVideoCtx, pFrame, output_pFmtContext, output_videoTimeBase, output_bCombine, output_nVideoIndex);
            }
        }

        av_packet_free(&pPacket);
    }
    EncodeVideoFrame(output_pVideoCtx, nullptr, output_pFmtContext, output_videoTimeBase, output_bCombine, output_nVideoIndex);

    if (output_pFmtContext != nullptr) {
        av_write_trailer(output_pFmtContext);
        if ((output_pFmtContext != nullptr) && (0 == (output_pFmtContext->flags & AVFMT_NOFILE))) {
            avio_closep(&output_pFmtContext->pb);
        }

        avformat_free_context(output_pFmtContext);
        output_pFmtContext = nullptr;
    }
}

void VideoCutter::Check() {
    if ( input_url_ == nullptr ) {
        LOG(ERROR) << "input_url_ doesn't exist.";
    }

    if ( output_url_ == nullptr) {
        LOG(ERROR) << "output_url_ doesn't exist.";
    }

    if ( start_time_ < 0 ) {
        LOG(ERROR) << "start_time_ doesn't set up or set wrong value.";
    }

    if ( end_time_ < 0) {
        LOG(ERROR) << "end_time_ doesn't set up or set wrong value.";
    }
}

int VideoCutter::InitInputFmtContext(AVFormatContext* &input_pFmtContext) {
    input_pFmtContext = avformat_alloc_context();

    int ret;
    for (int i = 0; i < 3; i++) {
        ret = avformat_open_input(&input_pFmtContext, input_url_, nullptr, nullptr);
        if (ret < 0) {
            usleep(1000);

            LOG(ERROR) << "avformat_open_input failed. url = " << input_url_ << ", ret = " << ret;
            continue;
        } else {
            break;
        }
    }

    return ret;
}

int64_t VideoCutter::ExtractStartTime(AVStream* stream) {
    // The default start time is zero.
    int64_t start_time = 0;

    // First try to use  the |start_time| value as is.
    if (stream->start_time != AV_NOPTS_VALUE) {
        start_time = TimestampRescaleTs(stream->start_time, stream->time_base, FFMPEG_TIME_BASE);
    }

    // Next try to use the first DTS value, for codecs where we know PTS == DTS
    // (excludes all H26x codecs). The start time must be returned in PTS.
    if (stream->first_dts != AV_NOPTS_VALUE &&
        stream->codecpar->codec_id != AV_CODEC_ID_HEVC &&
        stream->codecpar->codec_id != AV_CODEC_ID_H264 &&
        stream->codecpar->codec_id != AV_CODEC_ID_MPEG4) {
        const int64_t first_pts = TimestampRescaleTs(stream->first_dts, stream->time_base, FFMPEG_TIME_BASE);
        if (first_pts < start_time) {
            start_time = first_pts;
        }
    }

    return start_time;
}

void VideoCutter::PacketRescaleTs(AVPacket* pkt, AVRational tbSrc, AVRational tbDst) {
    pkt->dts = TimestampRescaleTs(pkt->dts, tbSrc, tbDst);
    pkt->pts = TimestampRescaleTs(pkt->pts, tbSrc, tbDst);
}

int64_t VideoCutter::TimestampRescaleTs(int64_t timestamp, AVRational tbSrc, AVRational tbDst) {
//    if (tbDst.den == 1000000000) {
//        return av_rescale_q(timestamp, tbSrc, {1, 1000000}) * 1000;
//    }
    return av_rescale_q(timestamp, tbSrc, tbDst);
}

void VideoCutter::InitParams(AVFormatContext* &pFmtContext, VideoConfig_t &tVideoConfig) {
    if (tVideoConfig.index >= 0) {
        AVStream* st = pFmtContext->streams[tVideoConfig.index];
        tVideoConfig.time_base  = st->time_base;
        tVideoConfig.width      = st->codecpar->width;
        tVideoConfig.height     = st->codecpar->height;
        tVideoConfig.format     = st->codecpar->format;
        tVideoConfig.rotation   = GetRotation(st);
        tVideoConfig.frame_rate = st->r_frame_rate;
        tVideoConfig.duration   = TimestampRescaleTs(st->duration, st->time_base, FFMPEG_TIME_BASE);

        if (st->codecpar->codec_id == AV_CODEC_ID_VP8 || st->codecpar->codec_id == AV_CODEC_ID_VP9) {
            tVideoConfig.alpha = GetAlpha(st);
            if (tVideoConfig.format == AV_PIX_FMT_YUV420P && tVideoConfig.alpha == 1) {
                tVideoConfig.format = AV_PIX_FMT_YUVA420P;
            }
        }

        memset(tVideoConfig.name, 0, 256);
        memcpy(tVideoConfig.name, pFmtContext->iformat->name, strlen(pFmtContext->iformat->name));

        GetSar(pFmtContext, tVideoConfig, nullptr, &tVideoConfig.sar.num, &tVideoConfig.sar.den);
    }
}

int VideoCutter::GetRotation(AVStream *st) {
    AVDictionaryEntry *rotate_tag = av_dict_get(st->metadata, "rotate", nullptr, 0);
    double theta = 0;
    if (rotate_tag && *rotate_tag->value && strcmp(rotate_tag->value, "0")) {
        theta = atof(rotate_tag->value);
    }
    theta -= 360*floor(theta/360 + 0.9/360);
    return static_cast<int>(theta);
}

int VideoCutter::GetAlpha(AVStream *st) {
    int alpha = 0;
    AVDictionaryEntry *alpha_mode = av_dict_get(st->metadata, "alpha_mode", nullptr, 0);
    if (alpha_mode && *alpha_mode->value && strcmp(alpha_mode->value, "0")) {
        alpha = atof(alpha_mode->value);
    }
    return alpha;
}

void VideoCutter::GetSar(AVFormatContext* &pFmtContext, VideoConfig_t &tVideoConfig, AVFrame* frame, int* num, int* den) {
    if (tVideoConfig.index >= 0) {
        AVStream* st = pFmtContext->streams[tVideoConfig.index];
        AVRational sar = av_guess_sample_aspect_ratio(pFmtContext, st, frame);
        *num = sar.num;
        *den = sar.den;
    }
}

int VideoCutter::openCodecContext(const AVFormatContext *pFormatCtx, int *pStreamIndex, enum AVMediaType type, AVCodecContext **ppCodecCtx) {
    int streamIdx = -1;
    // 获取流下标
    for (int i = 0; i < pFormatCtx->nb_streams; i++)
    {
        if (pFormatCtx->streams[i]->codec->codec_type == type)
        {
            streamIdx = i;
            break;
        }
    }
    if (streamIdx == -1)
    {
        printf("find video stream failed!\n");
        exit(-2);
    }
    // 寻找解码器
    AVCodecContext  *pCodecCtx = pFormatCtx->streams[streamIdx]->codec;
    AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (NULL == pCodec)
    {
        printf("avcode find decoder failed!\n");
        exit(-2);
    }

    //打开解码器
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
    {
        printf("avcode open failed!\n");
        exit(-2);
    }
    *ppCodecCtx        = pCodecCtx;
    *pStreamIndex    = streamIdx;

    return 0;
}

bool VideoCutter::OpenVideo(const char *url, AVFormatContext* &output_pFmtContext, AVRational &output_videoTimeBase, int &output_nVideoIndex,
                            bool &output_bFaststart, AVCodecContext* &pVideoCtx, int w, int h, int fps, int bps, int crf, const char* preset) {
    bool ret = InitVideoEncoder(pVideoCtx, w,h,fps,bps,crf,preset);
    if (!ret) {
        return false;
    }

    ret = InitWriter(url, output_pFmtContext, output_videoTimeBase, output_nVideoIndex, output_bFaststart, pVideoCtx, nullptr);
    if (!ret) {
        return false;
    }
    return true;
}

bool VideoCutter::InitVideoEncoder(AVCodecContext* &pVideoCtx, int w, int h, int fps, int bps, int crf, const char* preset) {
    ////////////////////////////////////
    // 初始化编码器
    AVCodec *codec = avcodec_find_encoder_by_name("libx264");
    if (codec == nullptr) {
        LOG(ERROR) << "avcodec_find_encoder_by_name failed";
        codec = avcodec_find_encoder(AV_CODEC_ID_H264);
        if (codec == nullptr) {
            LOG(ERROR) << "avcodec_find_encoder(AV_CODEC_ID_H264) failed";
            return false;
        }
    }

    int thread_count = std::thread::hardware_concurrency();

    pVideoCtx = avcodec_alloc_context3(codec);
    pVideoCtx->profile = FF_PROFILE_H264_MAIN;
    pVideoCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    pVideoCtx->width = w;
    pVideoCtx->height = h;
#if defined(MIX_ANDROID) || defined(MIX_IOS)
    m_pVideoCtx->gop_size = 50;
#else
    pVideoCtx->gop_size = 10000;
    pVideoCtx->keyint_min = 10000;
#endif
    pVideoCtx->framerate = (AVRational) {fps, 1};
    pVideoCtx->time_base = (AVRational) {1, fps};
    pVideoCtx->max_b_frames = 0;
    pVideoCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    //const char* preset = "superfast";
//    av_opt_set(m_pVideoCtx->priv_data, "crf", std::to_string(crf).c_str() , 0);
    av_opt_set(pVideoCtx->priv_data, "preset", preset, 0);
    av_opt_set(pVideoCtx->priv_data, "tune", "zerolatency", 0);
    av_opt_set(pVideoCtx->priv_data, "threads", "auto", 0);

    // 设置码率限制
    if (bps != 0) {
        pVideoCtx->bit_rate = bps;
        pVideoCtx->rc_max_rate = bps;
        pVideoCtx->rc_buffer_size = bps >> 2;
    }

    DLOG(INFO) << "InitVideoEncoder w=" << w << " h=" << h
               << " fps=" << fps << " bps=" << bps
               << " preset=" << preset << " thread_count=" << thread_count;
    if (avcodec_open2(pVideoCtx, codec, nullptr) < 0) {
        LOG(ERROR) << "avcodec_open2 failed";
        return false;
    }
    ////////////////////////////////////
    return true;
}

bool VideoCutter::InitWriter(const char* url, AVFormatContext* &output_pFmtContext, AVRational &output_videoTimeBase, int &output_nVideoIndex,
                             bool &output_bFaststart, AVCodecContext* &videoCtx, AVCodecContext* audioCtx) {
    int ret = avformat_alloc_output_context2(&output_pFmtContext, nullptr, nullptr, url);
    if (ret < 0) {
        LOG(ERROR) << "avformat_alloc_output_context2 failed";
        return false;
    }

    if (videoCtx) {
        AVStream *stream = avformat_new_stream(output_pFmtContext, nullptr);
        stream->codecpar->codec_tag = 0;
        ret = avcodec_parameters_from_context(stream->codecpar, videoCtx);
        if (ret < 0) {
            LOG(ERROR) << "avcodec_parameters_from_context failed";
            return false;
        }
        output_nVideoIndex = stream->index;

        if (output_bFaststart) {
            av_opt_set(output_pFmtContext->priv_data, "movflags", "faststart", 0);
        }

        output_videoTimeBase = (AVRational) {1, 12800};
        stream->time_base = output_videoTimeBase;
    }

//    if (audioCtx) {
//        AVStream *stream = avformat_new_stream(m_pFmtContext, nullptr);
//        ret = avcodec_parameters_from_context(stream->codecpar, audioCtx);
//        if (ret < 0) {
//            LOG(ERROR) << "avcodec_parameters_from_context failed";
//            return false;
//        }
//        m_nAudioIndex = stream->index;
//        m_audioTimeBase = (AVRational) {1, audioSampleRate};
//        stream->time_base = m_audioTimeBase;
//    }
    ret = avio_open(&output_pFmtContext->pb, url, AVIO_FLAG_READ_WRITE);
    if (ret < 0) {
        LOG(ERROR) << "avio_open failed";
        return false;
    }

    ret = avformat_write_header(output_pFmtContext, nullptr);
    if (ret < 0) {
        LOG(ERROR) << "avformat_write_header failed,ret=" << ret;
        return false;
    }
    LOG(INFO) << "Output:" << url;
    return true;
}

void VideoCutter::EncodeVideoFrame(AVCodecContext* &videoCtx, AVFrame* frame, AVFormatContext* &output_pFmtContext, AVRational output_videoTimeBase,
                                   bool output_bCombine, int output_nVideoIndex) {
    if (videoCtx == nullptr) {
        return;
    }

    int64_t pts = 0;
    if (frame) {
        pts = frame->pts;
        LOG(INFO)<<"pts: [" << frame->pts << "]";
    }
//    av_frame_free(&frame);
//    frame = av_frame_alloc();
//    frame->width = 1280;
//    frame->height = 720;
//    frame->format = AV_PIX_FMT_YUV420P;  // AV_PIX_FMT_YUV420P / AV_PIX_FMT_RGBA
//    av_frame_get_buffer(frame, 32);
//    frame->pts = pts;

    int ret = avcodec_send_frame(videoCtx, frame);
    if (ret < 0) {
        DLOG(INFO) << "video avcodec_send_frame failed" << ret << " " << av_err2str(ret);
        av_frame_free(&frame);
        return;
    }
    av_frame_free(&frame);

    AVPacket* output_packet = nullptr;
    while (ret >= 0) {
        output_packet = av_packet_alloc();
        ret = avcodec_receive_packet(videoCtx, output_packet);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return;
        } else if (ret < 0) {
            LOG(ERROR) << "video acodec_receive_packet failed " << ret;
            return;
        }

        WriteVideoPacket(output_pFmtContext, output_packet, output_videoTimeBase, output_bCombine, output_nVideoIndex);
        av_packet_free(&output_packet);
    }
}

int VideoCutter::WriteVideoPacket(AVFormatContext* &output_pFmtContext, AVPacket *pkt,AVRational output_videoTimeBase,
                                  bool output_bCombine, int output_nVideoIndex) {
    int64_t last_video_dts_;
    LOG(INFO) << "1 video av_interleaved_write_frame  " << pkt->dts << " " << pkt->pts;

    PacketRescaleTs(pkt, (AVRational) {1, 1000}, output_videoTimeBase);

    if (output_bCombine && last_video_dts_ != AV_NOPTS_VALUE && pkt->dts <= last_video_dts_) {
        LOG(INFO) << "filter packet dts=" << pkt->dts << " last_video_dts=" << last_video_dts_;
        return 0;
    }
    last_video_dts_ = pkt->dts;
    pkt->stream_index = output_nVideoIndex;
    LOG(INFO) << "2 video av_interleaved_write_frame  " << pkt->dts << " " << pkt->pts;

    int ret = av_interleaved_write_frame(output_pFmtContext, pkt);
    if (ret < 0) {
        LOG(ERROR) << "video av_interleaved_write_frame failed " << ret;
    }

    return ret;
//    return 0;
}
