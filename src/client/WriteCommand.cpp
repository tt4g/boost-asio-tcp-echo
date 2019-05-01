#include "client/WriteCommand.hpp"

#include <boost/asio/write.hpp>

#include <iostream>
#include <utility>

namespace boost_asio_tcp_echo
{
namespace client
{

WriteCommand::WriteCommand(
        boost::asio::io_context &ioContext,
        std::shared_ptr<Settings> settings,
        std::shared_ptr<boost::asio::ip::tcp::socket> socket,
        std::string body)
        : ioContext_(ioContext),
          settings_(std::move(settings)),
          socket_(std::move(socket)),
          body_(std::move(body))
{

}

WriteCommand::~WriteCommand()
{

}

void WriteCommand::run(
        std::function<void(std::shared_ptr<boost::asio::ip::tcp::socket>)> callback)
{
    auto self = this->shared_from_this();
    auto deadLineTimer = std::make_shared<boost::asio::steady_timer>(this->ioContext_);

    deadLineTimer->expires_after(this->settings_->getWriteTimeout());
    deadLineTimer->async_wait(
            std::bind(&WriteCommand::handleTimeout, self, this->socket_, deadLineTimer,
                      std::placeholders::_1));

    boost::asio::async_write(
            *(this->socket_), boost::asio::buffer(this->body_),
            std::bind(&WriteCommand::handleWrite, self, std::move(callback),
                      this->socket_, deadLineTimer, std::placeholders::_1,
                      std::placeholders::_2));
}

void WriteCommand::handleTimeout(
        const std::shared_ptr<boost::asio::ip::tcp::socket> socket,
        const std::shared_ptr<boost::asio::steady_timer> deadLineTimer,
        const boost::system::error_code &ec)
{
    if (ec || boost::asio::steady_timer::clock_type::now() < deadLineTimer->expiry()) {
        socket->cancel();
    }

    if (ec) {
        std::cout << "WriteCommand::handleTimeout: " << ec.message() << std::endl;
    }
}

void WriteCommand::handleWrite(
        std::function<void(std::shared_ptr<boost::asio::ip::tcp::socket>)> callback,
        std::shared_ptr<boost::asio::ip::tcp::socket> socket,
        const std::shared_ptr<boost::asio::steady_timer> deadLineTimer,
        const boost::system::error_code &ec,
        std::size_t /* bytesTransferred */)
{
    if (ec) {
        std::cout << "WriteCommand::handleResolve: " << ec.message() << std::endl;

        return;
    }

    deadLineTimer->expires_at(
            boost::asio::steady_timer::time_point::max());
    deadLineTimer->cancel();

    callback(std::move(socket));
}

} // namespace boost_asio_tcp_echo
} // namespace client
