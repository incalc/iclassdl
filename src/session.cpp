#include <iclassdl/session.h>

#include <regex>
#include <sstream>
#include <string>
#include <string_view>

#include <boost/asio/connect.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>

namespace asio = boost::asio;
namespace beast = boost::beast;

namespace iclassdl {
    LoginException::LoginException(char const *message) noexcept
    : message(message) {}

    char const *LoginException::what() const noexcept {
        return message;
    }

    Session::Session(std::string_view sid, std::string_view password)
    : sid(sid), password(password) {
        try {
            constexpr auto host = "learn.inha.ac.kr";

            std::stringstream body;
            body << "username=" << sid << "&password=" << password << "&rememberusername=off";

            asio::io_context ioc;
            asio::ip::tcp::resolver resolver(ioc);

            asio::ssl::context ctx(asio::ssl::context::tlsv12_client);
            ctx.set_verify_mode(asio::ssl::verify_none);

            beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);

            // Set SNI Hostname (many hosts need this to handshake successfully)
            if (!SSL_set_tlsext_host_name(stream.native_handle(), host)) {
                beast::error_code ec{ static_cast<int>(::ERR_get_error()), asio::error::get_ssl_category() };
                throw beast::system_error{ ec };
            }

            beast::get_lowest_layer(stream).connect(resolver.resolve(host, "443"));
            stream.handshake(asio::ssl::stream_base::client);

            beast::http::request<beast::http::string_body> req{ beast::http::verb::post, "/login/index.php", 11 };
            req.set(beast::http::field::host, host);
            req.set(beast::http::field::user_agent, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/85.0.4183.102 Safari/537.36");
            req.set(beast::http::field::content_type, "application/x-www-form-urlencoded");
            req.body() = body.str();
            req.prepare_payload();
            beast::http::write(stream, req);

            beast::flat_buffer buffer;
            beast::http::response<beast::http::dynamic_body> res;
            beast::http::read(stream, buffer, res);

            int cookie_header_count = 0;
            std::string last_cookie_header;
            for (auto const &header : res.base()) {
                auto const &header_value = header.value();
                if (header_value.starts_with("MoodleSession=")) {
                    cookie_header_count += 1;
                    last_cookie_header = header_value.to_string();
                }
            }
            std::smatch match;
            std::regex const cookie_re("MoodleSession=(\\w+)");
            std::regex_search(last_cookie_header, match, cookie_re);

            if (cookie_header_count == 3 && match.size() == 2) {
                session_id = match[1];
            } else {
                throw LoginException("login: ID or password is incorrect");
            }

            beast::error_code ec;
            stream.shutdown(ec);

            if (ec == asio::error::eof) {
                ec = {};
            }

            if (ec) {
                throw beast::system_error{ ec };
            }
        } catch (std::exception const &e) {
            throw LoginException(e.what());
        }
    }

    std::string const &Session::get_sid() const noexcept {
        return sid;
    }

    std::string const &Session::get_password() const noexcept {
        return password;
    }

    std::string const &Session::get_session_id() const noexcept {
        return session_id;
    }
} // namespace iclassdl
