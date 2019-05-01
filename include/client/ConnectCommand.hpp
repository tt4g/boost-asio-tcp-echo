#ifndef BOOST_ASIO_TCP_ECHO_CLIENT_CONNECTCOMMAND_HPP_
#define BOOST_ASIO_TCP_ECHO_CLIENT_CONNECTCOMMAND_HPP_

#include "client/Settings.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/system/error_code.hpp>

#include <functional>
#include <memory>

namespace boost_asio_tcp_echo
{
namespace client
{

class ConnectCommand : public std::enable_shared_from_this<ConnectCommand>
{
public:

    explicit ConnectCommand(
            boost::asio::io_context &ioContext,
            std::shared_ptr<Settings> settings,
            boost::asio::ip::tcp::resolver::results_type endpoints);

    ~ConnectCommand();

    ConnectCommand(const ConnectCommand&) = delete;

    ConnectCommand(ConnectCommand&&) = delete;

    ConnectCommand& operator=(const ConnectCommand&) = delete;

    ConnectCommand& operator=(ConnectCommand&&) = delete;

    void run(
            std::function<void(std::shared_ptr<boost::asio::ip::tcp::socket>)> callback);

private:

    void handleTimeout(
            const std::shared_ptr<boost::asio::ip::tcp::socket> socket,
            const std::shared_ptr<boost::asio::steady_timer> deadLineTimer,
            const boost::system::error_code &ec);

    void handleConnect(
            std::function<void(std::shared_ptr<boost::asio::ip::tcp::socket>)> callback,
            std::shared_ptr<boost::asio::ip::tcp::socket> socket,
            const std::shared_ptr<boost::asio::steady_timer> deadLineTimer,
            const boost::system::error_code &ec,
            const boost::asio::ip::tcp::endpoint &endpoint);

    boost::asio::io_context &ioContext_;

    std::shared_ptr<Settings> settings_;

    boost::asio::ip::tcp::resolver::results_type endpoints_;

};

} // namespace boost_asio_tcp_echo
} // namespace client

#endif // BOOST_ASIO_TCP_ECHO_CLIENT_CONNECTCOMMAND_HPP_
