#ifndef BOOST_ASIO_TCP_ECHO_CLIENT_CLIENTLAYER_HPP_
#define BOOST_ASIO_TCP_ECHO_CLIENT_CLIENTLAYER_HPP_

#include "client/ConnectCommand.hpp"
#include "client/ResolveCommand.hpp"
#include "client/Settings.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <future>
#include <memory>
#include <string>

namespace boost_asio_tcp_echo
{
namespace client
{

class ClientLayer : public std::enable_shared_from_this<ClientLayer>
{
public:

    explicit ClientLayer(
            boost::asio::io_context &ioContext,
            std::shared_ptr<Settings> settings);

    std::future<std::string> request(std::string body);

private:

    void onResolve(
            std::shared_ptr<std::promise<std::string>> promise,
            std::string body,
            boost::asio::ip::tcp::resolver::results_type endpoints);

    void onConnect(
            std::shared_ptr<std::promise<std::string>> promise,
            std::string body,
            std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    void onWrite(
            std::shared_ptr<std::promise<std::string>> promise,
            std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    void onRead(
            std::shared_ptr<std::promise<std::string>> promise,
            std::string response,
            std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    boost::asio::io_context &ioContext_;

    std::shared_ptr<Settings> settings_;

};

} // namespace boost_asio_tcp_echo
} // namespace client

#endif // BOOST_ASIO_TCP_ECHO_CLIENT_CONNECTCOMMAND_HPP_
