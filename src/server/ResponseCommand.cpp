#include "server/ResponseCommand.hpp"

#include <boost/asio/write.hpp>

#include <iostream>
#include <utility>

namespace boost_asio_tcp_echo
{
namespace server
{

ResponseCommand::ResponseCommand(
        boost::asio::io_context &ioContext,
        std::shared_ptr<Settings> settings,
        std::shared_ptr<boost::asio::ip::tcp::socket> socket,
        std::string responseBody)
        : ioContext_(ioContext),
          settings_(std::move(settings)),
          socket_(std::move(socket)),
          responseBody_(std::move(responseBody))
{

}

ResponseCommand::~ResponseCommand()
{

}

void ResponseCommand::run(
        std::function<void(std::shared_ptr<boost::asio::ip::tcp::socket>)> callback)
{
    auto self = this->shared_from_this();
    auto deadLineTimer = std::make_shared<boost::asio::steady_timer>(this->ioContext_);

    deadLineTimer->expires_after(this->settings_->getResponseWriteTimeout());
    deadLineTimer->async_wait(
            std::bind(&ResponseCommand::handleTimeout, self, this->socket_, deadLineTimer,
                      std::placeholders::_1));

    boost::asio::async_write(
            *(this->socket_), boost::asio::buffer(this->responseBody_),
            std::bind(&ResponseCommand::handleWrite, self, std::move(callback),
                      this->socket_, deadLineTimer, std::placeholders::_1,
                      std::placeholders::_2));
}

void ResponseCommand::handleTimeout(
        const std::shared_ptr<boost::asio::ip::tcp::socket> socket,
        const std::shared_ptr<boost::asio::steady_timer> deadLineTimer,
        const boost::system::error_code &ec)
{
    if (ec == boost::asio::error::operation_aborted) {
        // Timer canceled
        return;
    }

    if (ec || boost::asio::steady_timer::clock_type::now() < deadLineTimer->expiry()) {
        boost::system::error_code cancelEc;
        socket->cancel(cancelEc);
    }

    if (ec) {
        std::cout << "ResponseCommand::handleTimeout: " << ec.message() << std::endl;
    }
}

void ResponseCommand::handleWrite(
        std::function<void(std::shared_ptr<boost::asio::ip::tcp::socket>)> callback,
        std::shared_ptr<boost::asio::ip::tcp::socket> socket,
        const std::shared_ptr<boost::asio::steady_timer> deadLineTimer,
        const boost::system::error_code &ec,
        std::size_t /* bytesTransferred */)
{
    if (ec) {
        std::cout << "ResponseCommand::handleWrite: " << ec.message() << std::endl;

        return;
    }

    deadLineTimer->expires_at(
            boost::asio::steady_timer::time_point::max());
    deadLineTimer->cancel();

    callback(std::move(socket));
}

} // namespace server
} // namespace boost_asio_tcp_echo
