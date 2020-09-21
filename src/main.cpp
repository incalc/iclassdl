#include <iostream>

#include <iclassdl/session.h>

extern "C" {
#include <iclassdl/media.h>
}

int main(int argc, char **argv) {
    if (argc < 5) {
        printf("usage: %s id password input output\n"
               "API example program to remux a media file with libavformat and libavcodec.\n"
               "The output format is guessed according to the file extension.\n"
               "\n",
               argv[0]);
        return 1;
    }

    char const *sid = argv[1];
    char const *password = argv[2];
    char const *infile = argv[3];
    char const *outfile = argv[4];

    try {
        auto session_id = iclassdl::session::login(sid, password);
        std::cout << "session_id=" << session_id << '\n';
    } catch (std::exception const &e) {
        std::cerr << e.what() << std::endl;
    }

    if (remux(infile, outfile) != MEDIA_REMUX_SUCCESS) {
        std::cerr << "Fail to download and remux media\n";
    }
    return 0;
}
