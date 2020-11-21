/*
 * Newtek NDI input
 * Copyright (c) 2017 Kisliak Yury
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "libavformat/avformat.h"
#include "libavformat/internal.h"
#include "libavutil/opt.h"
#include "libavutil/imgutils.h"

#include "libndi_newtek_common.h"

<<<<<<< HEAD
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <dlfcn.h>

static NDIlib_v4 *p_NDILib = NULL;

struct NDIContext
{
=======
struct NDIContext {
>>>>>>> f98229d13e11418c1ad1f9836ffe2788f6b1b863
    const AVClass *cclass;

    /* Options */
    int find_sources;
    int64_t wait_sources;
    int allow_video_fields;
    char *extra_ips;

    /* Runtime */
    NDIlib_recv_create_t *recv;
    NDIlib_find_instance_t ndi_find;

    /* Streams */
    AVStream *video_st, *audio_st;
};

static int ndi_set_video_packet(AVFormatContext *avctx, NDIlib_video_frame_t *v, AVPacket *pkt)
{
    int ret;
    struct NDIContext *ctx = avctx->priv_data;

    ret = av_new_packet(pkt, v->yres * v->line_stride_in_bytes);
    if (ret < 0)
        return ret;

    pkt->dts = pkt->pts = av_rescale_q(v->timecode, NDI_TIME_BASE_Q, ctx->video_st->time_base);
    pkt->duration = av_rescale_q(1, (AVRational){v->frame_rate_D, v->frame_rate_N}, ctx->video_st->time_base);

<<<<<<< HEAD
    av_log(avctx, AV_LOG_DEBUG, "%s: pkt->dts = pkt->pts = %" PRId64 ", duration=%" PRId64 ", timecode=%" PRId64 "\n",
           __func__, pkt->dts, pkt->duration, v->timecode);

    pkt->flags |= AV_PKT_FLAG_KEY;
    pkt->stream_index = ctx->video_st->index;
=======
    av_log(avctx, AV_LOG_DEBUG, "%s: pkt->dts = pkt->pts = %"PRId64", duration=%"PRId64", timecode=%"PRId64"\n",
        __func__, pkt->dts, pkt->duration, v->timecode);

    pkt->flags         |= AV_PKT_FLAG_KEY;
    pkt->stream_index   = ctx->video_st->index;
>>>>>>> f98229d13e11418c1ad1f9836ffe2788f6b1b863

    memcpy(pkt->data, v->p_data, pkt->size);

    return 0;
}

static int ndi_set_audio_packet(AVFormatContext *avctx, NDIlib_audio_frame_t *a, AVPacket *pkt)
{
    int ret;
    struct NDIContext *ctx = avctx->priv_data;

    NDIlib_audio_frame_interleaved_16s_t dst;

    ret = av_new_packet(pkt, 2 * a->no_samples * a->no_channels);
    if (ret < 0)
        return ret;

    pkt->dts = pkt->pts = av_rescale_q(a->timecode, NDI_TIME_BASE_Q, ctx->audio_st->time_base);
    pkt->duration = av_rescale_q(1, (AVRational){a->no_samples, a->sample_rate}, ctx->audio_st->time_base);

<<<<<<< HEAD
    av_log(avctx, AV_LOG_DEBUG, "%s: pkt->dts = pkt->pts = %" PRId64 ", duration=%" PRId64 ", timecode=%" PRId64 "\n",
           __func__, pkt->dts, pkt->duration, a->timecode);

    pkt->flags |= AV_PKT_FLAG_KEY;
=======
    av_log(avctx, AV_LOG_DEBUG, "%s: pkt->dts = pkt->pts = %"PRId64", duration=%"PRId64", timecode=%"PRId64"\n",
        __func__, pkt->dts, pkt->duration, a->timecode);

    pkt->flags       |= AV_PKT_FLAG_KEY;
>>>>>>> f98229d13e11418c1ad1f9836ffe2788f6b1b863
    pkt->stream_index = ctx->audio_st->index;

    dst.reference_level = 0;
    dst.p_data = (short *)pkt->data;
<<<<<<< HEAD
    p_NDILib->NDIlib_util_audio_to_interleaved_16s(a, &dst);
=======
    NDIlib_util_audio_to_interleaved_16s(a, &dst);
>>>>>>> f98229d13e11418c1ad1f9836ffe2788f6b1b863

    return 0;
}

static int ndi_find_sources(AVFormatContext *avctx, const char *name, NDIlib_source_t *source_to_connect_to)
{
    int j = AVERROR(ENODEV);
    unsigned int n, i;
    struct NDIContext *ctx = avctx->priv_data;
    const NDIlib_source_t *ndi_srcs = NULL;
<<<<<<< HEAD
    const NDIlib_find_create_t find_create_desc = {.show_local_sources = true,
                                                   .p_groups = NULL,
                                                   .p_extra_ips = ctx->extra_ips};

    if (!ctx->ndi_find)
        ctx->ndi_find = p_NDILib->find_create(&find_create_desc);
    if (!ctx->ndi_find)
    {
=======
    const NDIlib_find_create_t find_create_desc = { .show_local_sources = true,
        .p_groups = NULL, .p_extra_ips = ctx->extra_ips };

    if (!ctx->ndi_find)
        ctx->ndi_find = NDIlib_find_create2(&find_create_desc);
    if (!ctx->ndi_find) {
>>>>>>> f98229d13e11418c1ad1f9836ffe2788f6b1b863
        av_log(avctx, AV_LOG_ERROR, "NDIlib_find_create failed.\n");
        return AVERROR(EIO);
    }

    while (1)
    {
        int f, t = ctx->wait_sources / 1000;
        av_log(avctx, AV_LOG_DEBUG, "Waiting for sources %d miliseconds\n", t);
<<<<<<< HEAD
        f = p_NDILib->find_wait_for_sources(ctx->ndi_find, t);
=======
        f = NDIlib_find_wait_for_sources(ctx->ndi_find, t);
>>>>>>> f98229d13e11418c1ad1f9836ffe2788f6b1b863
        av_log(avctx, AV_LOG_DEBUG, "NDIlib_find_wait_for_sources returns %d\n", f);
        if (!f)
            break;
    };

<<<<<<< HEAD
    ndi_srcs = p_NDILib->find_get_current_sources(ctx->ndi_find, &n);
=======
    ndi_srcs = NDIlib_find_get_current_sources(ctx->ndi_find, &n);
>>>>>>> f98229d13e11418c1ad1f9836ffe2788f6b1b863

    if (ctx->find_sources)
        av_log(avctx, AV_LOG_INFO, "Found %d NDI sources:\n", n);

<<<<<<< HEAD
    for (i = 0; i < n; i++)
    {
        if (ctx->find_sources)
            av_log(avctx, AV_LOG_INFO, "\t'%s'\t'%s'\n", ndi_srcs[i].p_ndi_name, ndi_srcs[i].p_ip_address);

        if (!strcmp(name, ndi_srcs[i].p_ndi_name))
        {
=======
    for (i = 0; i < n; i++) {
        if (ctx->find_sources)
            av_log(avctx, AV_LOG_INFO, "\t'%s'\t'%s'\n", ndi_srcs[i].p_ndi_name, ndi_srcs[i].p_ip_address);

        if (!strcmp(name, ndi_srcs[i].p_ndi_name)) {
>>>>>>> f98229d13e11418c1ad1f9836ffe2788f6b1b863
            *source_to_connect_to = ndi_srcs[i];
            j = i;
        }
    }

    return j;
}

static int ndi_read_header(AVFormatContext *avctx)
{
    int ret;
    NDIlib_recv_create_t recv_create_desc;
<<<<<<< HEAD
    const NDIlib_tally_t tally_state = {.on_program = true, .on_preview = false};
    struct NDIContext *ctx = avctx->priv_data;

#ifdef _WIN32
    av_log(avctx, AV_LOG_ERROR, "Not impemented yet.\n");
#else

    char ndi_path[255] = NDILIB_LIBRARY_NAME;

    void *hNDILib = dlopen(ndi_path, RTLD_LOCAL | RTLD_LAZY);
    const NDIlib_v4 *(*NDIlib_v4_load)(void) = NULL;

    if (hNDILib)
    {
        *((void **)&NDIlib_v4_load) = dlsym(hNDILib, "NDIlib_v4_load");
    }

    if (!NDIlib_v4_load)
    {
        if (hNDILib)
        {
            dlclose(hNDILib);
        }

        av_log(avctx, AV_LOG_ERROR, "Please re-install the NewTek NDI Runtime to use this application.\n");
        return 0;
    }

#endif

    p_NDILib = NDIlib_v4_load();

    if (!p_NDILib->initialize())
    {
=======
    const NDIlib_tally_t tally_state = { .on_program = true, .on_preview = false };
    struct NDIContext *ctx = avctx->priv_data;

    if (!NDIlib_initialize()) {
>>>>>>> f98229d13e11418c1ad1f9836ffe2788f6b1b863
        av_log(avctx, AV_LOG_ERROR, "NDIlib_initialize failed.\n");
        return AVERROR_EXTERNAL;
    }

    /* Find available sources. */
    ret = ndi_find_sources(avctx, avctx->url, &recv_create_desc.source_to_connect_to);
<<<<<<< HEAD
    if (ctx->find_sources)
    {
=======
    if (ctx->find_sources) {
>>>>>>> f98229d13e11418c1ad1f9836ffe2788f6b1b863
        return AVERROR_EXIT;
    }
    if (ret < 0)
        return ret;

    /* Create receiver description */
    recv_create_desc.color_format = NDIlib_recv_color_format_e_UYVY_RGBA;
    recv_create_desc.bandwidth = NDIlib_recv_bandwidth_highest;
    recv_create_desc.allow_video_fields = ctx->allow_video_fields;

    /* Create the receiver */
<<<<<<< HEAD
    ctx->recv = p_NDILib->recv_create_v3(&recv_create_desc);
    if (!ctx->recv)
    {
=======
    ctx->recv = NDIlib_recv_create(&recv_create_desc);
    if (!ctx->recv) {
>>>>>>> f98229d13e11418c1ad1f9836ffe2788f6b1b863
        av_log(avctx, AV_LOG_ERROR, "NDIlib_recv_create2 failed.\n");
        return AVERROR(EIO);
    }

    /* Set tally */
<<<<<<< HEAD
    p_NDILib->recv_set_tally(ctx->recv, &tally_state);
=======
    NDIlib_recv_set_tally(ctx->recv, &tally_state);
>>>>>>> f98229d13e11418c1ad1f9836ffe2788f6b1b863

    avctx->ctx_flags |= AVFMTCTX_NOHEADER;

    return 0;
}

static int ndi_create_video_stream(AVFormatContext *avctx, NDIlib_video_frame_t *v)
{
    AVStream *st;
    AVRational tmp;
    struct NDIContext *ctx = avctx->priv_data;

    st = avformat_new_stream(avctx, NULL);
<<<<<<< HEAD
    if (!st)
    {
=======
    if (!st) {
>>>>>>> f98229d13e11418c1ad1f9836ffe2788f6b1b863
        av_log(avctx, AV_LOG_ERROR, "Cannot add video stream\n");
        return AVERROR(ENOMEM);
    }

<<<<<<< HEAD
    st->time_base = NDI_TIME_BASE_Q;
    st->r_frame_rate = av_make_q(v->frame_rate_N, v->frame_rate_D);
=======
    st->time_base                   = NDI_TIME_BASE_Q;
    st->r_frame_rate                = av_make_q(v->frame_rate_N, v->frame_rate_D);
>>>>>>> f98229d13e11418c1ad1f9836ffe2788f6b1b863

    tmp = av_mul_q(av_d2q(v->picture_aspect_ratio, INT_MAX), (AVRational){v->yres, v->xres});
    av_reduce(&st->sample_aspect_ratio.num, &st->sample_aspect_ratio.den, tmp.num, tmp.den, 1000);
    st->codecpar->sample_aspect_ratio = st->sample_aspect_ratio;

<<<<<<< HEAD
    st->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    st->codecpar->width = v->xres;
    st->codecpar->height = v->yres;
    st->codecpar->codec_id = AV_CODEC_ID_RAWVIDEO;
    st->codecpar->bit_rate = av_rescale(v->xres * v->yres * 16, v->frame_rate_N, v->frame_rate_D);
    st->codecpar->field_order = v->frame_format_type == NDIlib_frame_format_type_progressive
                                    ? AV_FIELD_PROGRESSIVE
                                    : AV_FIELD_TT;

    if (NDIlib_FourCC_type_UYVY == v->FourCC || NDIlib_FourCC_type_UYVA == v->FourCC)
    {
        st->codecpar->format = AV_PIX_FMT_UYVY422;
        st->codecpar->codec_tag = MKTAG('U', 'Y', 'V', 'Y');
        if (NDIlib_FourCC_type_UYVA == v->FourCC)
            av_log(avctx, AV_LOG_WARNING, "Alpha channel ignored\n");
    }
    else if (NDIlib_FourCC_type_BGRA == v->FourCC)
    {
        st->codecpar->format = AV_PIX_FMT_BGRA;
        st->codecpar->codec_tag = MKTAG('B', 'G', 'R', 'A');
    }
    else if (NDIlib_FourCC_type_BGRX == v->FourCC)
    {
        st->codecpar->format = AV_PIX_FMT_BGR0;
        st->codecpar->codec_tag = MKTAG('B', 'G', 'R', '0');
    }
    else if (NDIlib_FourCC_type_RGBA == v->FourCC)
    {
        st->codecpar->format = AV_PIX_FMT_RGBA;
        st->codecpar->codec_tag = MKTAG('R', 'G', 'B', 'A');
    }
    else if (NDIlib_FourCC_type_RGBX == v->FourCC)
    {
        st->codecpar->format = AV_PIX_FMT_RGB0;
        st->codecpar->codec_tag = MKTAG('R', 'G', 'B', '0');
    }
    else
    {
=======
    st->codecpar->codec_type        = AVMEDIA_TYPE_VIDEO;
    st->codecpar->width             = v->xres;
    st->codecpar->height            = v->yres;
    st->codecpar->codec_id          = AV_CODEC_ID_RAWVIDEO;
    st->codecpar->bit_rate          = av_rescale(v->xres * v->yres * 16, v->frame_rate_N, v->frame_rate_D);
    st->codecpar->field_order       = v->frame_format_type == NDIlib_frame_format_type_progressive
        ? AV_FIELD_PROGRESSIVE : AV_FIELD_TT;

    if (NDIlib_FourCC_type_UYVY == v->FourCC || NDIlib_FourCC_type_UYVA == v->FourCC) {
        st->codecpar->format        = AV_PIX_FMT_UYVY422;
        st->codecpar->codec_tag     = MKTAG('U', 'Y', 'V', 'Y');
        if (NDIlib_FourCC_type_UYVA == v->FourCC)
            av_log(avctx, AV_LOG_WARNING, "Alpha channel ignored\n");
    } else if (NDIlib_FourCC_type_BGRA == v->FourCC) {
        st->codecpar->format        = AV_PIX_FMT_BGRA;
        st->codecpar->codec_tag     = MKTAG('B', 'G', 'R', 'A');
    } else if (NDIlib_FourCC_type_BGRX == v->FourCC) {
        st->codecpar->format        = AV_PIX_FMT_BGR0;
        st->codecpar->codec_tag     = MKTAG('B', 'G', 'R', '0');
    } else if (NDIlib_FourCC_type_RGBA == v->FourCC) {
        st->codecpar->format        = AV_PIX_FMT_RGBA;
        st->codecpar->codec_tag     = MKTAG('R', 'G', 'B', 'A');
    } else if (NDIlib_FourCC_type_RGBX == v->FourCC) {
        st->codecpar->format        = AV_PIX_FMT_RGB0;
        st->codecpar->codec_tag     = MKTAG('R', 'G', 'B', '0');
    } else {
>>>>>>> f98229d13e11418c1ad1f9836ffe2788f6b1b863
        av_log(avctx, AV_LOG_ERROR, "Unsupported video stream format, v->FourCC=%d\n", v->FourCC);
        return AVERROR(EINVAL);
    }

    avpriv_set_pts_info(st, 64, 1, NDI_TIME_BASE);

    ctx->video_st = st;

    return 0;
}

static int ndi_create_audio_stream(AVFormatContext *avctx, NDIlib_audio_frame_t *a)
{
    AVStream *st;
    struct NDIContext *ctx = avctx->priv_data;

    st = avformat_new_stream(avctx, NULL);
<<<<<<< HEAD
    if (!st)
    {
=======
    if (!st) {
>>>>>>> f98229d13e11418c1ad1f9836ffe2788f6b1b863
        av_log(avctx, AV_LOG_ERROR, "Cannot add audio stream\n");
        return AVERROR(ENOMEM);
    }

<<<<<<< HEAD
    st->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
    st->codecpar->codec_id = AV_CODEC_ID_PCM_S16LE;
    st->codecpar->sample_rate = a->sample_rate;
    st->codecpar->channels = a->no_channels;
=======
    st->codecpar->codec_type        = AVMEDIA_TYPE_AUDIO;
    st->codecpar->codec_id          = AV_CODEC_ID_PCM_S16LE;
    st->codecpar->sample_rate       = a->sample_rate;
    st->codecpar->channels          = a->no_channels;
>>>>>>> f98229d13e11418c1ad1f9836ffe2788f6b1b863

    avpriv_set_pts_info(st, 64, 1, NDI_TIME_BASE);

    ctx->audio_st = st;

    return 0;
}

static int ndi_read_packet(AVFormatContext *avctx, AVPacket *pkt)
{
    int ret = 0;
    struct NDIContext *ctx = avctx->priv_data;

<<<<<<< HEAD
    while (!ret)
    {
=======
    while (!ret) {
>>>>>>> f98229d13e11418c1ad1f9836ffe2788f6b1b863
        NDIlib_video_frame_t v;
        NDIlib_audio_frame_t a;
        NDIlib_metadata_frame_t m;
        NDIlib_frame_type_e t;

        av_log(avctx, AV_LOG_DEBUG, "NDIlib_recv_capture...\n");
<<<<<<< HEAD
        t = p_NDILib->recv_capture(ctx->recv, &v, &a, &m, 40);
        av_log(avctx, AV_LOG_DEBUG, "NDIlib_recv_capture=%d\n", t);

        if (t == NDIlib_frame_type_video)
        {
=======
        t = NDIlib_recv_capture(ctx->recv, &v, &a, &m, 40);
        av_log(avctx, AV_LOG_DEBUG, "NDIlib_recv_capture=%d\n", t);

        if (t == NDIlib_frame_type_video) {
>>>>>>> f98229d13e11418c1ad1f9836ffe2788f6b1b863
            if (!ctx->video_st)
                ret = ndi_create_video_stream(avctx, &v);
            if (!ret)
                ret = ndi_set_video_packet(avctx, &v, pkt);
<<<<<<< HEAD
            p_NDILib->recv_free_video_v2(ctx->recv, &v);
            break;
        }
        else if (t == NDIlib_frame_type_audio)
        {
=======
            NDIlib_recv_free_video(ctx->recv, &v);
            break;
        }
        else if (t == NDIlib_frame_type_audio) {
>>>>>>> f98229d13e11418c1ad1f9836ffe2788f6b1b863
            if (!ctx->audio_st)
                ret = ndi_create_audio_stream(avctx, &a);
            if (!ret)
                ret = ndi_set_audio_packet(avctx, &a, pkt);
<<<<<<< HEAD
            p_NDILib->recv_free_audio_v2(ctx->recv, &a);
            break;
        }
        else if (t == NDIlib_frame_type_metadata)
            p_NDILib->recv_free_metadata(ctx->recv, &m);
        else if (t == NDIlib_frame_type_error)
        {
=======
            NDIlib_recv_free_audio(ctx->recv, &a);
            break;
        }
        else if (t == NDIlib_frame_type_metadata)
            NDIlib_recv_free_metadata(ctx->recv, &m);
        else if (t == NDIlib_frame_type_error){
>>>>>>> f98229d13e11418c1ad1f9836ffe2788f6b1b863
            av_log(avctx, AV_LOG_ERROR, "NDIlib_recv_capture failed with error\n");
            ret = AVERROR(EIO);
        }
    };

    return ret;
}

static int ndi_read_close(AVFormatContext *avctx)
{
    struct NDIContext *ctx = (struct NDIContext *)avctx->priv_data;

    if (ctx->recv)
<<<<<<< HEAD
        p_NDILib->recv_destroy(ctx->recv);

    if (ctx->ndi_find)
        p_NDILib->find_destroy(ctx->ndi_find);
=======
        NDIlib_recv_destroy(ctx->recv);

    if (ctx->ndi_find)
        NDIlib_find_destroy(ctx->ndi_find);
>>>>>>> f98229d13e11418c1ad1f9836ffe2788f6b1b863

    return 0;
}

#define OFFSET(x) offsetof(struct NDIContext, x)
#define DEC AV_OPT_FLAG_DECODING_PARAM

static const AVOption options[] = {
<<<<<<< HEAD
    {"find_sources", "Find available sources", OFFSET(find_sources), AV_OPT_TYPE_BOOL, {.i64 = 0}, 0, 1, DEC},
    {"wait_sources", "Time to wait until the number of online sources have changed", OFFSET(wait_sources), AV_OPT_TYPE_DURATION, {.i64 = 1000000}, 100000, 20000000, DEC},
    {"allow_video_fields", "When this flag is FALSE, all video that you receive will be progressive", OFFSET(allow_video_fields), AV_OPT_TYPE_BOOL, {.i64 = 1}, 0, 1, DEC},
    {"extra_ips", "List of comma separated ip addresses to scan for remote sources", OFFSET(extra_ips), AV_OPT_TYPE_STRING, {.str = NULL}, 0, 0, DEC},
    {NULL},
=======
    { "find_sources", "Find available sources"  , OFFSET(find_sources), AV_OPT_TYPE_BOOL, { .i64 = 0 }, 0, 1, DEC },
    { "wait_sources", "Time to wait until the number of online sources have changed"  , OFFSET(wait_sources), AV_OPT_TYPE_DURATION, { .i64 = 1000000 }, 100000, 20000000, DEC },
    { "allow_video_fields", "When this flag is FALSE, all video that you receive will be progressive"  , OFFSET(allow_video_fields), AV_OPT_TYPE_BOOL, { .i64 = 1 }, 0, 1, DEC },
    { "extra_ips", "List of comma separated ip addresses to scan for remote sources",       OFFSET(extra_ips), AV_OPT_TYPE_STRING, {.str = NULL }, 0, 0, DEC },
    { NULL },
>>>>>>> f98229d13e11418c1ad1f9836ffe2788f6b1b863
};

static const AVClass libndi_newtek_demuxer_class = {
    .class_name = "NDI demuxer",
<<<<<<< HEAD
    .item_name = av_default_item_name,
    .option = options,
    .version = LIBAVUTIL_VERSION_INT,
    .category = AV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT,
};

AVInputFormat ff_libndi_newtek_demuxer = {
    .name = "libndi_newtek",
    .long_name = NULL_IF_CONFIG_SMALL("Network Device Interface (NDI) input using NewTek library"),
    .flags = AVFMT_NOFILE,
    .priv_class = &libndi_newtek_demuxer_class,
    .priv_data_size = sizeof(struct NDIContext),
    .read_header = ndi_read_header,
    .read_packet = ndi_read_packet,
    .read_close = ndi_read_close,
=======
    .item_name  = av_default_item_name,
    .option     = options,
    .version    = LIBAVUTIL_VERSION_INT,
    .category   = AV_CLASS_CATEGORY_DEVICE_VIDEO_INPUT,
};

AVInputFormat ff_libndi_newtek_demuxer = {
    .name           = "libndi_newtek",
    .long_name      = NULL_IF_CONFIG_SMALL("Network Device Interface (NDI) input using NewTek library"),
    .flags          = AVFMT_NOFILE,
    .priv_class     = &libndi_newtek_demuxer_class,
    .priv_data_size = sizeof(struct NDIContext),
    .read_header   = ndi_read_header,
    .read_packet   = ndi_read_packet,
    .read_close    = ndi_read_close,
>>>>>>> f98229d13e11418c1ad1f9836ffe2788f6b1b863
};