#ifndef ICLASSDL_MEDIA_H
#define ICLASSDL_MEDIA_H

#include <exception>
#include <string_view>

namespace iclassdl::media {
    class FFmpegException final : public std::exception {
    public:
        explicit FFmpegException(int errnum);
        ~FFmpegException() final;
        [[nodiscard]] char const *what() const noexcept override;

    private:
        char *message{};
    };

    void download(std::string_view url, std::string_view filename) noexcept(false);
} // namespace iclassdl::media

#endif // ICLASSDL_MEDIA_H
