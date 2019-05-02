#ifndef BOOST_ASIO_TCP_ECHO_SERVER_SERVERLAYER_HPP_
#define BOOST_ASIO_TCP_ECHO_SERVER_SERVERLAYER_HPP_

#include "server/Settings.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/system/error_code.hpp>

#include <functional>
#include <memory>

namespace boost_asio_tcp_echo
{
namespace server
{

class ServerLayer : public std::enable_shared_from_this<ServerLayer>
{
public:

    explicit ServerLayer(
            boost::asio::io_context &ioContext,
            std::shared_ptr<Settings> settings);

    ~ServerLayer();

    ServerLayer(const ServerLayer&) = delete;

    ServerLayer(ServerLayer&&) = delete;

    bool start();

    void stop();

    ServerLayer& operator=(const ServerLayer&) = delete;

    ServerLayer& operator=(ServerLayer&&) = delete;

private:

    void accept(
            std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor);

    void onAccept(
            std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor,
            std::shared_ptr<boost::asio::ip::tcp::socket> socket,
            const boost::system::error_code &ec);

    void onReceive(
            std::string requestBody,
            std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    void onWrite(std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    boost::asio::io_context &ioContext_;

    std::shared_ptr<Settings> settings_;

    std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_;

};

} // namespace server
} // namespace boost_asio_tcp_echo

#endif // BOOST_ASIO_TCP_ECHO_SERVER_SERVERLAYER_HPP_
