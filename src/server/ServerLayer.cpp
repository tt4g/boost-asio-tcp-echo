#include "server/ServerLayer.hpp"

#include "server/ReceiveCommand.hpp"
#include "server/ResponseCommand.hpp"

#include <iostream>
#include <utility>

namespace boost_asio_tcp_echo
{
namespace server
{

ServerLayer::ServerLayer(
        boost::asio::io_context &ioContext,
        std::shared_ptr<Settings> settings)
        : ioContext_(ioContext),
          settings_(settings),
          acceptor_(
                std::make_shared<boost::asio::ip::tcp::acceptor>(ioContext))
{

}

ServerLayer::~ServerLayer()
{

}

bool ServerLayer::start()
{
    if (this->acceptor_->is_open()) {
        return false;
    }

    // bind ANY address (interface).
    boost::asio::ip::tcp::endpoint endpoint(
                boost::asio::ip::tcp::v4(), this->settings_->getPort());
    boost::system::error_code ec;

    this->acceptor_->open(endpoint.protocol(), ec);
    if (ec) {
        std::cout << "ServerLayer::start: " << ec.message() << std::endl;
        return false;
    }

    this->acceptor_->set_option(
            boost::asio::ip::tcp::acceptor::reuse_address(true), ec);
    if (ec) {
        std::cout << "ServerLayer::start: " << ec.message() << std::endl;
        return false;
    }

    this->acceptor_->bind(endpoint, ec);
    if (ec) {
        std::cout << "ServerLayer::start: " << ec.message() << std::endl;
        return false;
    }

    this->acceptor_->listen(boost::asio::socket_base::max_listen_connections, ec);
    if (ec) {
        std::cout << "ServerLayer::start: " << ec.message() << std::endl;
        return false;
    }


    this->accept(this->acceptor_);

    return true;
}

void ServerLayer::stop()
{
    if (!this->acceptor_->is_open()) {
        return;
    }

    boost::system::error_code ec;
    this->acceptor_->close(ec);
    if (ec)
    {
        std::cout << "ServerLayer::stop: " << ec.message() << std::endl;
    }
}

void ServerLayer::accept(
        std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor)
{
    auto self = this->shared_from_this();
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(this->ioContext_);

    acceptor->async_accept(
            *socket, std::bind(&ServerLayer::onAccept, self, acceptor, socket,
            std::placeholders::_1));
}

void ServerLayer::onAccept(
        std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor,
        std::shared_ptr<boost::asio::ip::tcp::socket> socket,
        const boost::system::error_code &ec)
{
    if (ec == boost::asio::error::operation_aborted) {
        // If call `stop()` or `boost::asio::io_context::stop()` ...
        return;
    }

    if (ec) {
        std::cout << "ServerLayer::onAccept: " << ec.message() << std::endl;
    } else {

        auto self = this->shared_from_this();

        auto receiveCommand =
                std::make_shared<server::ReceiveCommand>(
                        this->ioContext_, this->settings_, std::move(socket));
        receiveCommand->run(
                std::bind(&ServerLayer::onReceive, self, std::placeholders::_1,
                std::placeholders::_2));
    }

    if (acceptor->is_open()) {
        // call accept() to accept next connection.
        accept(acceptor);
    }
}

void ServerLayer::onReceive(
        std::string requestBody,
        std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    std::cout << "server received!: " << requestBody << std::endl;

    auto self = this->shared_from_this();

    auto responseCommand =
            std::make_shared<ResponseCommand>(
                    this->ioContext_, this->settings_, std::move(socket),
                    R"EOS({"respose": {"message": "Hello, Server!"}})EOS");
    responseCommand->run(
            std::bind(&ServerLayer::onWrite, self, std::placeholders::_1));
}

void ServerLayer::onWrite(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    boost::system::error_code ec;
    socket->close(ec);
    if (ec)
    {
        std::cout << "ServerLayer::onWrite: " << ec.message() << std::endl;
    }
}

} // namespace server
} // namespace boost_asio_tcp_echo

