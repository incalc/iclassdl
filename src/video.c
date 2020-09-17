#include <iclassdl/video.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>

int open_file(AVFormatContext **fmt_ctx, char const *filename) {
    assert(filename != NULL);

    if ((*fmt_ctx = avformat_alloc_context()) == NULL) {
        fprintf(stderr, "Fail to allocate memory for context\n");
        return -1;
    }

    if (avformat_open_input(fmt_ctx, filename, NULL, NULL) < 0) {
        fprintf(stderr, "Fail to open input file '%s'", filename);
        return -1;
    }

    if (avformat_find_stream_info(*fmt_ctx, NULL) < 0) {
        fprintf(stderr, "Fail to find input stream information");
        return -1;
    }

    assert(*fmt_ctx != NULL);
    return 0;
}

int prepare_copy(AVStream **out_stream, AVFormatContext *fmt_ctx, AVCodecParameters const *codecpar) {
    assert(fmt_ctx != NULL);
    assert(codecpar != NULL);

    if ((*out_stream = avformat_new_stream(fmt_ctx, NULL)) == NULL) {
        fprintf(stderr, "Fail to allocate output stream\n");
        return -1;
    }

    if (avcodec_parameters_copy((*out_stream)->codecpar, codecpar) < 0) {
        fprintf(stderr, "Fail to copy codec parameters\n");
        return -1;
    }

    assert(*out_stream != NULL);
    return 0;
}

int remux(char const *infile, char const *outfile) {
    assert(infile != NULL);
    assert(outfile != NULL);

    int ret;

    AVFormatContext *in_fmt_ctx = NULL;
    if ((ret = open_file(&in_fmt_ctx, infile)) < 0) {
        goto end;
    }

    AVFormatContext *out_fmt_ctx = NULL;
    ret = avformat_alloc_output_context2(&out_fmt_ctx, NULL, NULL, outfile);
    if (out_fmt_ctx == NULL) {
        fprintf(stderr, "Fail to allocate memory for output context\n");
        fprintf(stderr, "Error occurred: %s\n", av_err2str(ret));
        ret = AVERROR_UNKNOWN;
        goto end;
    }

    unsigned int const stream_count = in_fmt_ctx->nb_streams;
    int *streams = av_mallocz_array(stream_count, sizeof(*streams));
    if (!streams) {
        ret = AVERROR(ENOMEM);
        goto end;
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
            goto end;
        }

        out_stream->codecpar->codec_tag = 0;
    }

    if (!(out_fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        if ((ret = avio_open(&out_fmt_ctx->pb, outfile, AVIO_FLAG_WRITE)) < 0) {
            fprintf(stderr, "Fail to open output file '%s'", outfile);
            goto end;
        }
    }

    if ((ret = avformat_write_header(out_fmt_ctx, NULL)) < 0) {
        fprintf(stderr, "Error occurred when opening output file\n");
        goto end;
    }

    AVPacket packet;
    while (true) {
        AVStream *in_stream, *out_stream;

        ret = av_read_frame(in_fmt_ctx, &packet);
        if (ret < 0)
            break;

        in_stream = in_fmt_ctx->streams[packet.stream_index];
        if (packet.stream_index >= stream_count || streams[packet.stream_index] < 0) {
            av_packet_unref(&packet);
            continue;
        }

        packet.stream_index = streams[packet.stream_index];
        out_stream = out_fmt_ctx->streams[packet.stream_index];

        /* copy packet */
        packet.pts = av_rescale_q_rnd(packet.pts, in_stream->time_base, out_stream->time_base, AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
        packet.dts = av_rescale_q_rnd(packet.dts, in_stream->time_base, out_stream->time_base, AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
        packet.duration = av_rescale_q(packet.duration, in_stream->time_base, out_stream->time_base);

        packet.pos = -1;

        ret = av_interleaved_write_frame(out_fmt_ctx, &packet);
        if (ret < 0) {
            fprintf(stderr, "Error muxing packet\n");
            break;
        }
        av_packet_unref(&packet);
    }

    av_write_trailer(out_fmt_ctx);

end:

    avformat_close_input(&in_fmt_ctx);

    /* close output */
    if (out_fmt_ctx && !(out_fmt_ctx->oformat->flags & AVFMT_NOFILE))
        avio_closep(&out_fmt_ctx->pb);

    avformat_free_context(out_fmt_ctx);

    av_freep(&streams);

    if (ret < 0 && ret != AVERROR_EOF) {
        fprintf(stderr, "Error occurred: %s\n", av_err2str(ret));
        return 1;
    }

    return 0;
}
