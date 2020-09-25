#ifndef ICLASSDL_SESSION_H
#define ICLASSDL_SESSION_H

#include <exception>
#include <string>
#include <string_view>

#include <iclassdl/export.h>

namespace iclassdl {
    class ICLASSDL_EXPORT LoginException : public std::exception {
    public:
        explicit LoginException(char const *message) noexcept;
        [[nodiscard]] char const *what() const noexcept override;

    private:
        char const *message{};
    };

    class ICLASSDL_EXPORT Session {
    public:
        Session(std::string_view sid, std::string_view password);
        [[nodiscard]] std::string const &get_sid() const noexcept;
        [[nodiscard]] std::string const &get_password() const noexcept;
        [[nodiscard]] std::string const &get_session_id() const noexcept;

    private:
        std::string sid;
        std::string password;
        std::string session_id;
    };
} // namespace iclassdl

#endif // ICLASSDL_SESSION_H
