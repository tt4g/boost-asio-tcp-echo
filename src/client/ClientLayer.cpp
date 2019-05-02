#include "client/ClientLayer.hpp"

#include "client/ConnectCommand.hpp"
#include "client/ReadCommand.hpp"
#include "client/ResolveCommand.hpp"
#include "client/WriteCommand.hpp"

#include <functional>
#include <iostream>

namespace boost_asio_tcp_echo
{
namespace client
{

ClientLayer::ClientLayer(
        boost::asio::io_context &ioContext,
        std::shared_ptr<Settings> settings)
        : std::enable_shared_from_this<ClientLayer>(),
          ioContext_(ioContext),
          settings_(std::move(settings))
{

}

std::future<std::string> ClientLayer::request(std::string body)
{
    auto self = this->shared_from_this();

    auto promise = std::make_shared<std::promise<std::string>>();
    std::future<std::string> future = promise->get_future();

    // NOTE: If onResolve modifier public, promise into stack.
//    std::promise<std::string> promise;
//    resolveCommand->run([promise = std::move(promise), self](boost::asio::ip::tcp::resolver::results_type endpoints)
//    {
//        self->onResolve(std::move(promise), std::move(endpoints));
//    });

    auto resolveCommand =
            std::make_shared<ResolveCommand>(this->ioContext_, this->settings_);
    resolveCommand->run(
        std::bind(&ClientLayer::onResolve, self, std::move(promise),
                  std::move(body), std::placeholders::_1));

    return future;
}

void ClientLayer::onResolve(
        std::shared_ptr<std::promise<std::string>> promise,
        std::string body,
        boost::asio::ip::tcp::resolver::results_type endpoints)
{
    std::cout << "ClientLayer resolved!" << std::endl;

    for (auto endpoint_ : endpoints) {
        std::cout << endpoint_.endpoint() << std::endl;
    }

    auto self = this->shared_from_this();

    auto connectCommand =
            std::make_shared<ConnectCommand>(
                        this->ioContext_, this->settings_, std::move(endpoints));

    connectCommand->run(
            std::bind(&ClientLayer::onConnect, self,
                      std::move(promise), std::move(body),
                      std::placeholders::_1));
}

void ClientLayer::onConnect(
        std::shared_ptr<std::promise<std::string>> promise,
        std::string body,
        std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    std::cout << "ClientLayer connected!" << std::endl;

    auto self = this->shared_from_this();

    auto writeCommand =
            std::make_shared<WriteCommand>(
                    this->ioContext_, this->settings_, std::move(socket),
                    std::move(body));
    writeCommand->run(
            std::bind(&ClientLayer::onWrite, self, std::move(promise),
                      std::placeholders::_1));
}

void ClientLayer::onWrite(
        std::shared_ptr<std::promise<std::string>> promise,
        std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    auto self = this->shared_from_this();

    auto readCommand =
            std::make_shared<ReadCommand>(
                    this->ioContext_, this->settings_, std::move(socket));

    readCommand->run(
            std::bind(&ClientLayer::onRead, self, std::move(promise),
            std::placeholders::_1, std::placeholders::_2));
}

void ClientLayer::onRead(
        std::shared_ptr<std::promise<std::string>> promise,
        std::string response,
        std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    promise->set_value(std::move(response));

    boost::system::error_code ec;
    socket->close(ec);
    if (ec)
    {
        std::cout << "ClientLayer::onRead: " << ec.message() << std::endl;
    }
}

} // namespace boost_asio_tcp_echo
} // namespace client
