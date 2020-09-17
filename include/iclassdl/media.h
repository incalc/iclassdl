#ifndef ICLASSDL_MEDIA_H
#define ICLASSDL_MEDIA_H

#include <libavformat/avformat.h>

#define MEDIA_REMUX_SUCCESS 0
#define MEDIA_REMUX_FAILURE 1

int open_media_input(AVFormatContext **fmt_ctx, char const *filename);
int open_media_output(AVFormatContext **fmt_ctx, char const *filename);
int prepare_copy(AVStream **out_stream, AVFormatContext *fmt_ctx, AVCodecParameters const *codecpar);
int remux(char const *infile, char const *outfile);

#endif // ICLASSDL_MEDIA_H
