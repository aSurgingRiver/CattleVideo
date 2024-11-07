

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

#include "IFFmpgeLoad.h"
