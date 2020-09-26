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
    constexpr auto iclass_host = "learn.inha.ac.kr";
    constexpr auto iclass_port = "443";
    constexpr auto iclass_login_url = "/login/index.php";
    constexpr auto iclass_viewer_url = "/mod/vod/viewer.php";

    SessionException::SessionException(char const *message) noexcept
    : message(message) {}

    char const *SessionException::what() const noexcept {
        return message;
    }

    Session::Session(std::string_view sid, std::string_view password)
    : sid(sid), password(password) {
        std::stringstream body;
        body << "username=" << sid << "&password=" << password << "&rememberusername=off";

        beast::http::request<beast::http::string_body> req{ beast::http::verb::post, iclass_login_url, 11 };
        req.set(beast::http::field::content_type, "application/x-www-form-urlencoded");
        req.body() = body.str();
        req.prepare_payload();

        auto const res = do_request(req);

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
            throw SessionException("login: ID or password is incorrect");
        }
    }

    beast::http::response<beast::http::dynamic_body> Session::do_request(beast::http::request<beast::http::string_body> &req) const {
        req.set(beast::http::field::host, iclass_host);
        req.set(beast::http::field::user_agent, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/85.0.4183.102 Safari/537.36");

        asio::io_context ioc;
        asio::ip::tcp::resolver resolver(ioc);

        asio::ssl::context ctx(asio::ssl::context::tlsv12_client);
        ctx.set_verify_mode(asio::ssl::verify_none);

        beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);

        // Set SNI Hostname (many hosts need this to handshake successfully)
        if (!SSL_set_tlsext_host_name(stream.native_handle(), iclass_host)) {
            beast::error_code ec{ static_cast<int>(::ERR_get_error()), asio::error::get_ssl_category() };
            throw beast::system_error{ ec };
        }

        beast::get_lowest_layer(stream).connect(resolver.resolve(iclass_host, iclass_port));
        stream.handshake(asio::ssl::stream_base::client);

        beast::error_code ec;

        beast::http::write(stream, req, ec);

        beast::flat_buffer buffer;
        beast::http::response_parser<beast::http::dynamic_body> parser;
        beast::http::read(stream, buffer, parser, ec);

        stream.shutdown(ec);

        if (ec && ec != asio::error::eof) {
            throw beast::system_error{ ec };
        }
        return parser.release();
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

    std::string Session::parse_video_viewer(std::string_view url) const {
        std::regex const id_re("id=(\\d+)");
        std::match_results<std::string_view::const_iterator> id_match;
        if (!std::regex_search(url.begin(), url.end(), id_match, id_re)) {
            throw SessionException("parse: Invalid video view url");
        }

        std::stringstream target;
        target << iclass_viewer_url << "?id=" << id_match[1].str();

        std::stringstream cookie;
        cookie << "MoodleSession=" << session_id;

        beast::http::request<beast::http::string_body> req{ beast::http::verb::get, target.str(), 11 };
        req.set(beast::http::field::cookie, cookie.str());

        auto const res = do_request(req);
        auto const body = boost::beast::buffers_to_string(res.body().data());

        std::regex resource_re(R"(<source src=\"(.+\.m3u8)\")");
        std::smatch resource_match;
        if (!std::regex_search(body, resource_match, resource_re)) {
            throw SessionException("parse: Can not find resource url");
        }
        return resource_match[1].str();
    }
} // namespace iclassdl
