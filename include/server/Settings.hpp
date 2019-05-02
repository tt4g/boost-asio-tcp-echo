#ifndef BOOST_ASIO_TCP_ECHO_SERVER_SETTINGS_HPP_
#define BOOST_ASIO_TCP_ECHO_SERVER_SETTINGS_HPP_

#include <chrono>

namespace boost_asio_tcp_echo
{
namespace server
{

class Settings
{
public:

    explicit Settings(
            int port,
            std::chrono::milliseconds requestReadTimeout,
            std::chrono::milliseconds responseWriteTimeout);

    int getPort() const;

    const std::chrono::milliseconds& getRequestReadTimeout() const;

    const std::chrono::milliseconds& getResponseWriteTimeout() const;

private:

    int port_;

    std::chrono::milliseconds requestReadTimeout_;

    std::chrono::milliseconds responseWriteTimeout_;

};

} // namespace server
} // namespace boost_asio_tcp_echo

#endif // BOOST_ASIO_TCP_ECHO_SERVER_SETTINGS_HPP_
