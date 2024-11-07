
#include "FFmpegCompatible.h"
#include "IFFmpgeLoad.h"
#if FFMPEG_MAJOR==4
AVChannelLayout get_channel_layout(AVCodecContext* ctx) {
	AVChannelLayout out;
	out.nb_channels = ctx->channels;
	return out;
}
AVChannelLayout get_channel_layout(AVFrame* frame) {
	AVChannelLayout out;
	out.nb_channels = frame->channels;
	return out;
}
int av_channel_layout_copy(AVChannelLayout* dst, const AVChannelLayout* src) {
	dst->nb_channels = src->nb_channels;	
	//dst->order = src->order;
	//dst->u = src->u;
	//dst->opaque = src->opaque;
	return 1;
}
int av_channel_layout_compare(const AVChannelLayout* a, const  AVChannelLayout* b) {
	return a->nb_channels == b->nb_channels ? 0 : 1 ;
}
int swr_alloc_set_opts2(struct SwrContext** ps,
	const AVChannelLayout* out_ch_layout, enum AVSampleFormat out_sample_fmt, int out_sample_rate,
	const AVChannelLayout* in_ch_layout, enum AVSampleFormat  in_sample_fmt, int  in_sample_rate,
	int log_offset, void* log_ctx) {
	*ps = FFmpegCall(swr_alloc_set_opts)(*ps,
		out_ch_layout->nb_channels,   out_sample_fmt,  out_sample_rate,
		  in_ch_layout->nb_channels,   in_sample_fmt,   in_sample_rate,
		 log_offset,  log_ctx);
	return *ps ? 0 : -1;
}
#else
AVChannelLayout get_channel_layout(AVCodecContext* CodecContext) {
	return CodecContext->ch_layout;
}
AVChannelLayout get_channel_layout(AVFrame* frame) {
	return frame->ch_layout;
}
#endif
