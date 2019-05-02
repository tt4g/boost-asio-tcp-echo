#include "client/ConnectCommand.hpp"

#include <boost/asio/connect.hpp>

#include <iostream>
#include <utility>

namespace boost_asio_tcp_echo
{
namespace client
{

ConnectCommand::ConnectCommand(
        boost::asio::io_context &ioContext,
        std::shared_ptr<Settings> settings,
        boost::asio::ip::tcp::resolver::results_type endpoints)
        : std::enable_shared_from_this<ConnectCommand>(),
          ioContext_(ioContext),
          settings_(std::move(settings)),
          endpoints_(std::move(endpoints))
{

}

ConnectCommand::~ConnectCommand()
{

}

void ConnectCommand::run(
        std::function<void(std::shared_ptr<boost::asio::ip::tcp::socket>)> callback)
{
    auto self = this->shared_from_this();
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(this->ioContext_);
    auto deadLineTimer = std::make_shared<boost::asio::steady_timer>(this->ioContext_);

    deadLineTimer->expires_after(this->settings_->getConnectTimeout());
    deadLineTimer->async_wait(
            std::bind(&ConnectCommand::handleTimeout, self, socket, deadLineTimer,
                      std::placeholders::_1));

    boost::asio::async_connect(
            *socket,
            this->endpoints_,
            std::bind(&ConnectCommand::handleConnect, self, std::move(callback),
                      socket, deadLineTimer, std::placeholders::_1,
                      std::placeholders::_2));
}

void ConnectCommand::handleTimeout(
        const std::shared_ptr<boost::asio::ip::tcp::socket> socket,
        const std::shared_ptr<boost::asio::steady_timer> deadLineTimer,
        const boost::system::error_code &ec)
{
    if (ec || boost::asio::steady_timer::clock_type::now() < deadLineTimer->expiry()) {
        socket->cancel();
    }

    if (ec) {
        std::cout << "ConnectCommand::handleTimeout: " << ec.message() << std::endl;
    }
}

void ConnectCommand::handleConnect(
        std::function<void(std::shared_ptr<boost::asio::ip::tcp::socket>)> callback,
        std::shared_ptr<boost::asio::ip::tcp::socket> socket,
        const std::shared_ptr<boost::asio::steady_timer> deadLineTimer,
        const boost::system::error_code &ec,
        const boost::asio::ip::tcp::endpoint &/* endpoint */)
{
    if (ec) {
        std::cout << "ConnectCommand::handleConnect: " << ec.message() << std::endl;

        return;
    }

    deadLineTimer->expires_at(
            boost::asio::steady_timer::time_point::max());
    deadLineTimer->cancel();

    callback(std::move(socket));
}

} // namespace boost_asio_tcp_echo
} // namespace client
