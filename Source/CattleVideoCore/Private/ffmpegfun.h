#pragma once

extern "C" {
#include "libavutil/adler32.h"
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#include "libavutil/time.h"
#include "libavutil/opt.h"
#include "libavutil/bprint.h"
#include "libavutil/timestamp.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
}
#include "FFmpegCompatible.h"

#define ToLocalFunc(name) f_##name
// avutil
typedef int (*ToLocalFunc(av_strerror))(int errnum, char* errbuf, size_t errbuf_size);
typedef void (*ToLocalFunc(av_frame_unref))(AVFrame* frame);
typedef void (*ToLocalFunc(av_frame_free))(AVFrame** frame);
typedef AVFrame* (*ToLocalFunc(av_frame_alloc))();
typedef AVFrame* (*ToLocalFunc(av_frame_clone))(AVFrame*);
typedef int64_t(*ToLocalFunc(av_gettime_relative))();
typedef int (*ToLocalFunc(av_hwframe_transfer_data))(AVFrame*, const AVFrame*, int);
typedef int (*ToLocalFunc(av_hwdevice_ctx_create))(AVBufferRef**, enum AVHWDeviceType, const char*, AVDictionary*, int);
typedef void (*ToLocalFunc(av_dict_free))(AVDictionary**);
typedef int (*ToLocalFunc(av_dict_set))(AVDictionary**, const char*, const char*, int);
typedef AVDictionaryEntry* (*ToLocalFunc(av_dict_get))(const AVDictionary*, const char*, const AVDictionaryEntry*, int);
typedef int (*ToLocalFunc(av_channel_layout_copy))(AVChannelLayout*, const AVChannelLayout*);
typedef int (*ToLocalFunc(av_channel_layout_compare))(const AVChannelLayout*, const AVChannelLayout*);
typedef int (*ToLocalFunc(av_samples_alloc_array_and_samples))(uint8_t***, int*, int, int, enum AVSampleFormat, int);
typedef void (*ToLocalFunc(av_freep))(void* ptr);
typedef int (*ToLocalFunc(av_samples_alloc))(uint8_t** audio_data, int* linesize, int nb_channels, int nb_samples, enum AVSampleFormat sample_fmt, int align);
typedef int (*ToLocalFunc(av_image_fill_arrays))(uint8_t* d[4], int l[4], const uint8_t*, enum AVPixelFormat, int, int, int);
typedef void (*ToLocalFunc(av_log_set_level))(int level);
typedef void (*callback)(void*, int, const char*, va_list);
typedef void (*ToLocalFunc(av_log_set_callback))(callback);
typedef int (*ToLocalFunc(av_log_get_level))(void);
// av_log ·ÏÆú
// avcodec
typedef int (*ToLocalFunc(avcodec_receive_frame))(AVCodecContext* avctx, AVFrame* frame);
typedef int (*ToLocalFunc(avcodec_send_packet))(AVCodecContext* avctx, const AVPacket* avpkt);
typedef void (*ToLocalFunc(av_packet_unref))(AVPacket* pkt);
typedef void (*ToLocalFunc(av_packet_ref))(AVPacket* pkt);
typedef void (*ToLocalFunc(av_packet_free))(AVPacket** pkt);
typedef AVPacket* (*ToLocalFunc(av_packet_alloc))(void);
typedef AVPacket* (*ToLocalFunc(av_packet_clone))(const AVPacket* src);
typedef const AVCodec* (*ToLocalFunc(avcodec_find_decoder))(enum AVCodecID id);
typedef AVCodecContext* (*ToLocalFunc(avcodec_alloc_context3))(const AVCodec*);
typedef void (*ToLocalFunc(avcodec_free_context))(AVCodecContext** avctx);
typedef int (*ToLocalFunc(avcodec_parameters_to_context))(AVCodecContext*, const AVCodecParameters*);
typedef const AVCodecHWConfig* (*ToLocalFunc(avcodec_get_hw_config))(const AVCodec* codec, int index);
typedef int (*ToLocalFunc(avcodec_open2))(AVCodecContext* avctx, const AVCodec* codec, AVDictionary** options);

// avformat
typedef AVFormatContext* (*ToLocalFunc(avformat_alloc_context))(void);
typedef int (*ToLocalFunc(avformat_open_input))(AVFormatContext**, const char*, const AVInputFormat*, AVDictionary**);
typedef void (*ToLocalFunc(avformat_close_input))(AVFormatContext** s);
typedef void (*ToLocalFunc(av_format_inject_global_side_data))(AVFormatContext* s);
typedef int (*ToLocalFunc(av_find_best_stream))(AVFormatContext*, enum AVMediaType, int, int, const AVCodec**, int);
typedef int (*ToLocalFunc(avformat_find_stream_info))(AVFormatContext*, AVDictionary**);
typedef void (*ToLocalFunc(av_dump_format))(AVFormatContext*, int, const char*, int);
typedef int (*ToLocalFunc(avformat_seek_file))(AVFormatContext*, int, int64_t, int64_t, int64_t, int);
typedef int (*ToLocalFunc(avformat_network_init))(void);
typedef int (*ToLocalFunc(av_read_frame))(AVFormatContext* s, AVPacket* pkt);

// swresample
typedef struct SwrContext* (*ToLocalFunc(swr_alloc))(void);
typedef int (*ToLocalFunc(swr_init))(struct SwrContext* s);
typedef void (*ToLocalFunc(swr_free))(struct SwrContext** s);
typedef int (*ToLocalFunc(swr_alloc_set_opts2))(struct SwrContext**,
	const AVChannelLayout*, enum AVSampleFormat, int,
	const AVChannelLayout*, enum AVSampleFormat, int,
	int, void*);
typedef struct SwrContext* (*ToLocalFunc(swr_alloc_set_opts))(struct SwrContext*,
	int64_t, enum AVSampleFormat, int,
	int64_t, enum AVSampleFormat, int,
	int, void*);
typedef int (*ToLocalFunc(swr_convert))(struct SwrContext*, uint8_t**, int, const uint8_t**, int);
// swscale
typedef struct SwsContext* (*ToLocalFunc(sws_getCachedContext))(struct SwsContext*,
	int srcW, int srcH, enum AVPixelFormat srcFormat,
	int dstW, int dstH, enum AVPixelFormat dstFormat,
	int flags, SwsFilter* srcFilter,
	SwsFilter* dstFilter, const double* param);
typedef void (*ToLocalFunc(sws_freeContext))(struct SwsContext* swsContext);
typedef int (*ToLocalFunc(sws_scale))(struct SwsContext* c, const uint8_t* const srcSlice[],
	const int srcStride[], int srcSliceY, int srcSliceH,
	uint8_t* const dst[], const int dstStride[]);

//
typedef void (*ToLocalFunc(avdevice_register_all))(void);