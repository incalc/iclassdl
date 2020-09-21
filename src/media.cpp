#include <iclassdl/media.h>

#include <string>
#include <string_view>

extern "C" {
#include <libavutil/error.h>
#include <iclassdl/media_impl.h>
}

namespace iclassdl::media {
    FFmpegException::FFmpegException(int errnum) {
        message = new char[64];
        av_strerror(errnum, message, 64);
    }

    FFmpegException::~FFmpegException() {
        delete[] message;
    }

    char const *FFmpegException::what() const noexcept {
        return message;
    }

    void download(std::string_view url, std::string_view filename) noexcept(false) {
        std::string const infile = std::string(url);
        std::string const outfile = std::string(filename);

        int errnum;
        if ((errnum = remux(infile.c_str(), outfile.c_str())) < 0) {
            throw FFmpegException(errnum);
        }
    }
} // namespace iclassdl::media
