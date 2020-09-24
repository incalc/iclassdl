#ifndef ICLASSDL_SESSION_H
#define ICLASSDL_SESSION_H

#include <exception>
#include <string>
#include <string_view>

#include <iclassdl/export.h>

namespace iclassdl::session {
    class ICLASSDL_EXPORT LoginFailException : public std::exception {
    public:
        explicit LoginFailException(char const *message);
        [[nodiscard]] char const *what() const noexcept override;

    private:
        char const *message;
    };

    std::string ICLASSDL_EXPORT login(std::string_view sid, std::string_view password) noexcept(false);
} // namespace iclassdl::session

#endif // ICLASSDL_SESSION_H
