#include <iclassdl/media.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>

int open_media_input(AVFormatContext **fmt_ctx, char const *filename) {
    assert(filename != NULL);

    int errnum;

    if ((*fmt_ctx = avformat_alloc_context()) == NULL) {
        return AVERROR(ENOMEM);
    }

    if ((errnum = avformat_open_input(fmt_ctx, filename, NULL, NULL)) < 0) {
        return errnum;
    }

    if ((errnum = avformat_find_stream_info(*fmt_ctx, NULL)) < 0) {
        return errnum;
    }

    assert(*fmt_ctx != NULL);
    return 0;
}

int open_media_output(AVFormatContext **fmt_ctx, char const *filename) {
    assert(filename != NULL);

    int errnum;

    if ((errnum = avformat_alloc_output_context2(fmt_ctx, NULL, NULL, filename)) < 0) {
        return errnum;
    }

    assert(*fmt_ctx != NULL);
    return 0;
}

int prepare_copy(AVStream **out_stream, AVFormatContext *fmt_ctx, AVCodecParameters const *codecpar) {
    assert(fmt_ctx != NULL);
    assert(codecpar != NULL);

    int errnum;

    if ((*out_stream = avformat_new_stream(fmt_ctx, NULL)) == NULL) {
        return AVERROR(ENOMEM);
    }

    if ((errnum = avcodec_parameters_copy((*out_stream)->codecpar, codecpar)) < 0) {
        return errnum;
    }

    (*out_stream)->codecpar->codec_tag = 0;

    assert(*out_stream != NULL);
    assert(fmt_ctx != NULL);
    return 0;
}

int remux(char const *infile, char const *outfile) {
    assert(infile != NULL);
    assert(outfile != NULL);

    int errnum;

    AVFormatContext *in_fmt_ctx = NULL;
    if ((errnum = open_media_input(&in_fmt_ctx, infile)) < 0) {
        goto finally;
    }

    AVFormatContext *out_fmt_ctx = NULL;
    if ((errnum = open_media_output(&out_fmt_ctx, outfile)) < 0) {
        goto finally;
    }

    unsigned int const stream_count = in_fmt_ctx->nb_streams;

    int *streams = av_mallocz_array(stream_count, sizeof(*streams));
    if (streams == NULL) {
        errnum = AVERROR(ENOMEM);
        goto finally;
    }

    int stream_index = 0;
    for (unsigned int i = 0; i < stream_count; i += 1) {
        AVStream *out_stream;
        AVStream *in_stream = in_fmt_ctx->streams[i];
        AVCodecParameters *in_codecpar = in_stream->codecpar;

        if (in_codecpar->codec_type != AVMEDIA_TYPE_AUDIO
            && in_codecpar->codec_type != AVMEDIA_TYPE_VIDEO
            && in_codecpar->codec_type != AVMEDIA_TYPE_SUBTITLE) {
            streams[i] = -1;
            continue;
        }
        streams[i] = stream_index++;

        if (prepare_copy(&out_stream, out_fmt_ctx, in_codecpar) < 0) {
            goto finally;
        }
    }

    if (!(out_fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        if ((errnum = avio_open(&out_fmt_ctx->pb, outfile, AVIO_FLAG_WRITE)) < 0) {
            goto finally;
        }
    }

    if ((errnum = avformat_write_header(out_fmt_ctx, NULL)) < 0) {
        goto finally;
    }

    AVPacket packet;
    while (true) {
        AVStream *in_stream, *out_stream;

        if ((errnum = av_read_frame(in_fmt_ctx, &packet)) < 0) {
            break;
        }

        in_stream = in_fmt_ctx->streams[packet.stream_index];
        if ((unsigned int) packet.stream_index >= stream_count || streams[packet.stream_index] < 0) {
            av_packet_unref(&packet);
            continue;
        }

        packet.stream_index = streams[packet.stream_index];
        out_stream = out_fmt_ctx->streams[packet.stream_index];

        packet.pts = av_rescale_q_rnd(packet.pts, in_stream->time_base, out_stream->time_base, AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
        packet.dts = av_rescale_q_rnd(packet.dts, in_stream->time_base, out_stream->time_base, AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
        packet.duration = av_rescale_q(packet.duration, in_stream->time_base, out_stream->time_base);

        packet.pos = -1;

        if ((errnum = av_interleaved_write_frame(out_fmt_ctx, &packet)) < 0) {
            break;
        }
        av_packet_unref(&packet);
    }

    av_write_trailer(out_fmt_ctx);

finally:
    avformat_close_input(&in_fmt_ctx);
    avformat_free_context(in_fmt_ctx);

    if (out_fmt_ctx && !(out_fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        avio_closep(&out_fmt_ctx->pb);
    }

    avformat_free_context(out_fmt_ctx);

    av_freep(&streams);

    if (errnum < 0 && errnum != AVERROR_EOF) {
        fprintf(stderr, "Error: %s\n", av_err2str(errnum));
        return MEDIA_REMUX_FAILURE;
    }

    return MEDIA_REMUX_SUCCESS;
}
