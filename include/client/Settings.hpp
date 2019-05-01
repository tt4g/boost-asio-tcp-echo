#ifndef BOOST_ASIO_TCP_ECHO_CLIENT_SETTINGS_HPP_
#define BOOST_ASIO_TCP_ECHO_CLIENT_SETTINGS_HPP_

#include <chrono>
#include <string>

namespace boost_asio_tcp_echo
{
namespace client
{

class Settings
{
public:

    explicit Settings(
            std::string host,
            int port,
            std::chrono::milliseconds resolveTimeout,
            std::chrono::milliseconds connectTimeout,
            std::chrono::milliseconds writeTimeout,
            std::chrono::milliseconds readTimeout);

    const std::string& getHost() const;

    int getPort() const;

    const std::chrono::milliseconds& getResolveTimeout() const;

    const std::chrono::milliseconds& getConnectTimeout() const;

    const std::chrono::milliseconds& getWriteTimeout() const;

    const std::chrono::milliseconds& getReadTimeout() const;

private:

    std::string host_;

    int port_;

    std::chrono::milliseconds resolveTimeout_;

    std::chrono::milliseconds connectTimeout_;

    std::chrono::milliseconds writeTimeout_;

    std::chrono::milliseconds readTimeout_;

};

} // namespace boost_asio_tcp_echo
} // namespace client

#endif // BOOST_ASIO_TCP_ECHO_CLIENT_SETTINGS_HPP_
