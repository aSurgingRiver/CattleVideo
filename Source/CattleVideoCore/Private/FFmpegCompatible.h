
#pragma once

#if FFMPEG_MAJOR==4

//typedef struct AVChannelCustom {
//    enum AVChannel id;
//    char name[16];
//    void* opaque;
//} AVChannelCustom;

typedef struct AVChannelLayout {
    /**
     * Channel order used in this layout.
     * This is a mandatory field.
     */
    //enum AVChannelOrder order;

    /**
     * Number of channels in this layout. Mandatory field.
     */
    int nb_channels;

    ///**
    // * Details about which channels are present in this layout.
    // * For AV_CHANNEL_ORDER_UNSPEC, this field is undefined and must not be
    // * used.
    // */
    //union {
    //    /**
    //     * This member must be used for AV_CHANNEL_ORDER_NATIVE, and may be used
    //     * for AV_CHANNEL_ORDER_AMBISONIC to signal non-diegetic channels.
    //     * It is a bitmask, where the position of each set bit means that the
    //     * AVChannel with the corresponding value is present.
    //     *
    //     * I.e. when (mask & (1 << AV_CHAN_FOO)) is non-zero, then AV_CHAN_FOO
    //     * is present in the layout. Otherwise it is not present.
    //     *
    //     * @note when a channel layout using a bitmask is constructed or
    //     * modified manually (i.e.  not using any of the av_channel_layout_*
    //     * functions), the code doing it must ensure that the number of set bits
    //     * is equal to nb_channels.
    //     */
    //    uint64_t mask;
    //    /**
    //     * This member must be used when the channel order is
    //     * AV_CHANNEL_ORDER_CUSTOM. It is a nb_channels-sized array, with each
    //     * element signalling the presence of the AVChannel with the
    //     * corresponding value in map[i].id.
    //     *
    //     * I.e. when map[i].id is equal to AV_CHAN_FOO, then AV_CH_FOO is the
    //     * i-th channel in the audio data.
    //     *
    //     * When map[i].id is in the range between AV_CHAN_AMBISONIC_BASE and
    //     * AV_CHAN_AMBISONIC_END (inclusive), the channel contains an ambisonic
    //     * component with ACN index (as defined above)
    //     * n = map[i].id - AV_CHAN_AMBISONIC_BASE.
    //     *
    //     * map[i].name may be filled with a 0-terminated string, in which case
    //     * it will be used for the purpose of identifying the channel with the
    //     * convenience functions below. Otherise it must be zeroed.
    //     */
    //    AVChannelCustom* map;
    //} u;

    ///**
    // * For some private data of the user.
    // */
    //void* opaque;
} AVChannelLayout;

#endif