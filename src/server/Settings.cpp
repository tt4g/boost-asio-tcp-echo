#include "server/Settings.hpp"

#include <utility>

namespace boost_asio_tcp_echo
{
namespace server
{

Settings::Settings(
        int port,
        std::chrono::milliseconds requestReadTimeout,
        std::chrono::milliseconds responseWriteTimeout)
        : port_(port),
          requestReadTimeout_(std::move(requestReadTimeout)),
          responseWriteTimeout_(std::move(responseWriteTimeout))
{

}

int Settings::getPort() const
{
    return this->port_;
}

const std::chrono::milliseconds& Settings::getRequestReadTimeout() const
{
    return this->requestReadTimeout_;
}

const std::chrono::milliseconds& Settings::getResponseWriteTimeout() const
{
    return this->responseWriteTimeout_;
}

} // namespace server
} // namespace boost_asio_tcp_echo
