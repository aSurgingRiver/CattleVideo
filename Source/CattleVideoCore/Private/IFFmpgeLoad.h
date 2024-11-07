// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ffmpegfun.h"
/**
 * 
 */
class IFFmpgeLoad
{
public:
	virtual ~IFFmpgeLoad()=default;
public:
	static IFFmpgeLoad* get();
	//virtual void LoadCEF3Modules() = 0;
	//virtual void UnloadCEF3Modules() = 0;
};

namespace cattlevideo {
#define FFmpegDef(name) f_##name v_##name
	FFmpegDef(av_strerror);
	FFmpegDef(av_frame_unref);
	FFmpegDef(av_frame_free);
	FFmpegDef(av_frame_alloc);
	FFmpegDef(av_frame_clone);
	FFmpegDef(av_gettime_relative);
	FFmpegDef(av_hwframe_transfer_data);
	FFmpegDef(av_hwdevice_ctx_create);
	FFmpegDef(av_dict_free);
	FFmpegDef(av_dict_set);
	FFmpegDef(av_dict_get);
	FFmpegDef(av_channel_layout_copy);
	FFmpegDef(av_channel_layout_compare);
	FFmpegDef(av_samples_alloc_array_and_samples);
	FFmpegDef(av_freep);
	FFmpegDef(av_samples_alloc);
	FFmpegDef(av_image_fill_arrays);
	FFmpegDef(av_log_set_level);
	FFmpegDef(av_log_set_callback);
	FFmpegDef(av_log_get_level);
	FFmpegDef(avcodec_receive_frame);
	FFmpegDef(avcodec_send_packet);
	FFmpegDef(av_packet_unref);
	FFmpegDef(av_packet_ref);
	FFmpegDef(av_packet_free);
	FFmpegDef(av_packet_alloc);
	FFmpegDef(av_packet_clone);
	FFmpegDef(avcodec_find_decoder);
	FFmpegDef(avcodec_alloc_context3);
	FFmpegDef(avcodec_free_context);
	FFmpegDef(avcodec_parameters_to_context);
	FFmpegDef(avcodec_get_hw_config);
	FFmpegDef(avcodec_open2);
	FFmpegDef(avformat_alloc_context);
	FFmpegDef(avformat_open_input);
	FFmpegDef(avformat_close_input);
	FFmpegDef(av_format_inject_global_side_data);
	FFmpegDef(av_find_best_stream);
	FFmpegDef(avformat_find_stream_info);
	FFmpegDef(av_dump_format);
	FFmpegDef(avformat_seek_file);
	FFmpegDef(avformat_network_init);
	FFmpegDef(av_read_frame);
	FFmpegDef(swr_alloc);
	FFmpegDef(swr_init);
	FFmpegDef(swr_free);
	FFmpegDef(swr_alloc_set_opts2);
	FFmpegDef(swr_alloc_set_opts);
	FFmpegDef(swr_convert);
	FFmpegDef(sws_getCachedContext);
	FFmpegDef(sws_freeContext);
	FFmpegDef(sws_scale);
	FFmpegDef(avdevice_register_all);
}

#if !PLATFORM_ANDROID
#define FFmpegCall(name) cattlevideo::v_##name
#else
#define FFmpegCall(name) name
#endif

//#define FFmpegCall(name) name

AVChannelLayout get_channel_layout(AVCodecContext*);
AVChannelLayout get_channel_layout(AVFrame* );
#if FFMPEG_MAJOR==4
int av_channel_layout_copy(AVChannelLayout* ,const AVChannelLayout*);
int av_channel_layout_compare(const AVChannelLayout* , const AVChannelLayout*);
int swr_alloc_set_opts2(struct SwrContext** ps,
	const AVChannelLayout* out_ch_layout, enum AVSampleFormat out_sample_fmt, int out_sample_rate,
	const AVChannelLayout* in_ch_layout, enum AVSampleFormat  in_sample_fmt, int  in_sample_rate,
	int log_offset, void* log_ctx);
#endif