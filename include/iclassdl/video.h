#ifndef ICLASSDL_VIDEO_H
#define ICLASSDL_VIDEO_H

#include <libavformat/avformat.h>

int open_file(AVFormatContext **fmt_ctx, char const *filename);
int prepare_copy(AVStream **out_stream, AVFormatContext *fmt_ctx, AVCodecParameters const *codecpar);
int remux(char const *infile, char const *outfile);

#endif // ICLASSDL_VIDEO_H
