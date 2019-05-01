#include "client/Settings.hpp"

#include <utility>

namespace boost_asio_tcp_echo
{
namespace client
{

Settings::Settings(
        std::string host,
        int port,
        std::chrono::milliseconds resolveTimeout,
        std::chrono::milliseconds connectTimeout,
        std::chrono::milliseconds writeTimeout,
        std::chrono::milliseconds readTimeout)
        : host_(std::move(host)),
          port_(port),
          resolveTimeout_(std::move(resolveTimeout)),
          connectTimeout_(std::move(connectTimeout)),
          writeTimeout_(std::move(writeTimeout)),
          readTimeout_(std::move(readTimeout))
{

}

const std::string& Settings::getHost() const
{
    return this->host_;
}

int Settings::getPort() const
{
    return this->port_;
}

const std::chrono::milliseconds& Settings::getResolveTimeout() const
{
    return this->resolveTimeout_;
}

const std::chrono::milliseconds& Settings::getConnectTimeout() const
{
    return this->connectTimeout_;
}

const std::chrono::milliseconds& Settings::getWriteTimeout() const
{
    return this->writeTimeout_;
}

const std::chrono::milliseconds& Settings::getReadTimeout() const
{
    return this->readTimeout_;
}

} // namespace boost_asio_tcp_echo
} // namespace client
