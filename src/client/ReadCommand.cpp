#include "client/ReadCommand.hpp"

#include <boost/asio/completion_condition.hpp>
#include <boost/asio/read.hpp>

#include <iostream>
#include <iterator>
#include <limits>
#include <utility>

namespace boost_asio_tcp_echo
{
namespace client
{

ReadCommand::ReadCommand(
        boost::asio::io_context &ioContext,
        std::shared_ptr<Settings> settings,
        std::shared_ptr<boost::asio::ip::tcp::socket> socket)
        : ioContext_(ioContext),
          settings_(std::move(settings)),
          socket_(std::move(socket))
{

}

ReadCommand::~ReadCommand()
{

}

void ReadCommand::run(
        std::function<void(std::string, std::shared_ptr<boost::asio::ip::tcp::socket>)> callback)
{
    auto self = this->shared_from_this();
    auto deadLineTimer = std::make_shared<boost::asio::steady_timer>(this->ioContext_);
    auto receiveBuffer =
            std::make_shared<boost::asio::streambuf>(std::numeric_limits<std::size_t>::max());

    deadLineTimer->expires_after(this->settings_->getReadTimeout());
    deadLineTimer->async_wait(
            std::bind(&ReadCommand::handleTimeout, self, this->socket_, deadLineTimer,
                      std::placeholders::_1));

    boost::asio::async_read(*(this->socket_), *receiveBuffer, boost::asio::transfer_at_least(1),
            std::bind(&ReadCommand::handleRead, self, std::move(callback),
                     this->socket_, deadLineTimer, receiveBuffer,
                     std::placeholders::_1, std::placeholders::_2));
}

void ReadCommand::handleTimeout(
        const std::shared_ptr<boost::asio::ip::tcp::socket> socket,
        const std::shared_ptr<boost::asio::steady_timer> deadLineTimer,
        const boost::system::error_code &ec)

{
    if (ec || boost::asio::steady_timer::clock_type::now() < deadLineTimer->expiry()) {
        boost::system::error_code cancelEc;
        socket->cancel(cancelEc);
    }

    if (ec) {
        std::cout << "ReadCommand::handleTimeout: " << ec.message() << std::endl;
    }
}

void ReadCommand::handleRead(
        std::function<void(std::string, std::shared_ptr<boost::asio::ip::tcp::socket>)> callback,
        std::shared_ptr<boost::asio::ip::tcp::socket> socket,
        const std::shared_ptr<boost::asio::steady_timer> deadLineTimer,
        std::shared_ptr<boost::asio::streambuf> receiveBuffer,
        const boost::system::error_code &ec,
        std::size_t /* bytesReceived */)
{
    if (ec) {
        std::cout << "ReadCommand::handleRead: " << ec.message() << std::endl;

        return;
    }

    deadLineTimer->expires_at(
            boost::asio::steady_timer::time_point::max());
    deadLineTimer->cancel();

    std::string response(
            (std::istreambuf_iterator<char>(receiveBuffer.get())),
            std::istreambuf_iterator<char>());
    callback(std::move(response), std::move(socket));
}

} // namespace boost_asio_tcp_echo
} // namespace client
