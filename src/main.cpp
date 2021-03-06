#include <iostream>

#include <iclassdl/media.h>
#include <iclassdl/session.h>

int main(int argc, char **argv) {
    if (argc < 5) {
        printf("usage: %s id password url filename\n"
               "API example program to remux a media file with libavformat and libavcodec.\n"
               "The output format is guessed according to the file extension.\n"
               "\n",
               argv[0]);
        return 1;
    }

    char const *sid = argv[1];
    char const *password = argv[2];
    char const *viewer_url = argv[3];
    char const *filename = argv[4];

    try {
        auto session = iclassdl::Session(sid, password);
        std::cout << "session_id=" << session.get_session_id() << '\n';

        auto resource_url = session.parse_video_viewer(viewer_url);
        std::cout << "resource_url=" << resource_url << '\n';

        iclassdl::media::download(resource_url, filename);
        std::cout << "Download finished." << std::endl;
    } catch (std::exception const &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
