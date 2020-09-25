#ifndef ICLASSDL_MEDIA_H
#define ICLASSDL_MEDIA_H

#include <exception>
#include <string_view>

#include <iclassdl/export.h>

namespace iclassdl::media {
    class ICLASSDL_EXPORT FFmpegException final : public std::exception {
    public:
        explicit FFmpegException(int errnum) noexcept;
        ~FFmpegException() final;
        [[nodiscard]] char const *what() const noexcept override;

    private:
        char *message{};
    };

    void ICLASSDL_EXPORT download(std::string_view url, std::string_view filename);
} // namespace iclassdl::media

#endif // ICLASSDL_MEDIA_H
