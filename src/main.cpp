#include <iostream>

extern "C" {
#include <iclassdl/video.h>
}

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("usage: %s input output\n"
               "API example program to remux a media file with libavformat and libavcodec.\n"
               "The output format is guessed according to the file extension.\n"
               "\n",
               argv[0]);
        return 1;
    }

    char const *infile = argv[1];
    char const *outfile = argv[2];

    remux(infile, outfile);
    return 0;
}
