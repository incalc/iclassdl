#ifndef ICLASSDL_SESSION_H
#define ICLASSDL_SESSION_H

#include <exception>
#include <string>
#include <string_view>

namespace iclassdl::session {
    class LoginFailException : public std::exception {
        using std::exception::exception;
    };

    std::string login(std::string_view sid, std::string_view password);
} // namespace iclassdl::session

#endif // ICLASSDL_SESSION_H
