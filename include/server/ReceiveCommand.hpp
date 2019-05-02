#ifndef BOOST_ASIO_TCP_ECHO_SERVER_RECEIVECOMMAND_HPP_
#define BOOST_ASIO_TCP_ECHO_SERVER_RECEIVECOMMAND_HPP_

#include "server/Settings.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/system/error_code.hpp>

#include <functional>
#include <memory>
#include <string>

namespace boost_asio_tcp_echo
{
namespace server
{

class ReceiveCommand : public std::enable_shared_from_this<ReceiveCommand>
{
public:

    explicit ReceiveCommand(
            boost::asio::io_context &ioContext,
            std::shared_ptr<Settings> settings,
            std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    ~ReceiveCommand();

    ReceiveCommand(const ReceiveCommand&) = delete;

    ReceiveCommand(ReceiveCommand&&) = delete;

    void run(
            std::function<void(std::string, std::shared_ptr<boost::asio::ip::tcp::socket>)> callback);

    ReceiveCommand& operator=(const ReceiveCommand&) = delete;

    ReceiveCommand& operator=(ReceiveCommand&&) = delete;

private:

    void handleTimeout(
            const std::shared_ptr<boost::asio::ip::tcp::socket> socket,
            const std::shared_ptr<boost::asio::steady_timer> deadLineTimer,
            const boost::system::error_code &ec);

    void handleRead(
            std::function<void(std::string, std::shared_ptr<boost::asio::ip::tcp::socket>)> callback,
            std::shared_ptr<boost::asio::ip::tcp::socket> socket,
            const std::shared_ptr<boost::asio::steady_timer> deadLineTimer,
            std::shared_ptr<boost::asio::streambuf> receiveBuffer,
            const boost::system::error_code &ec,
            std::size_t bytesReceived);

    boost::asio::io_context &ioContext_;

    std::shared_ptr<Settings> settings_;

    std::shared_ptr<boost::asio::ip::tcp::socket> socket_;

};

} // namespace server
} // namespace boost_asio_tcp_echo

#endif // BOOST_ASIO_TCP_ECHO_SERVER_RECEIVECOMMAND_HPP_
