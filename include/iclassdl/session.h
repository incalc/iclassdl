#ifndef ICLASSDL_SESSION_H
#define ICLASSDL_SESSION_H

#include <exception>
#include <string>
#include <string_view>

#include <boost/beast/http.hpp>
#include <iclassdl/export.h>

namespace beast = boost::beast;

namespace iclassdl {
    class ICLASSDL_EXPORT SessionException : public std::exception {
    public:
        explicit SessionException(char const *message) noexcept;
        [[nodiscard]] char const *what() const noexcept override;

    private:
        char const *message;
    };

    class ICLASSDL_EXPORT Session {
    public:
        Session(std::string_view sid, std::string_view password);
        [[nodiscard]] std::string const &get_sid() const noexcept;
        [[nodiscard]] std::string const &get_password() const noexcept;
        [[nodiscard]] std::string const &get_session_id() const noexcept;
        [[nodiscard]] std::string parse_video_viewer(std::string_view url) const;

    protected:
        beast::http::response<beast::http::dynamic_body> do_request(beast::http::request<beast::http::string_body> &req) const;

    private:
        std::string sid;
        std::string password;
        std::string session_id;
    };
} // namespace iclassdl

#endif // ICLASSDL_SESSION_H
