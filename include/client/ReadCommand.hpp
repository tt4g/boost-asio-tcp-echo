#ifndef BOOST_ASIO_TCP_ECHO_CLIENT_READCOMMAND_HPP_
#define BOOST_ASIO_TCP_ECHO_CLIENT_READCOMMAND_HPP_

#include "client/Settings.hpp"

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
namespace client
{

class ReadCommand : public std::enable_shared_from_this<ReadCommand>
{
public:

    explicit ReadCommand(
            boost::asio::io_context &ioContext,
            std::shared_ptr<Settings> settings,
            std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    ~ReadCommand();

    ReadCommand(const ReadCommand&) = delete;

    ReadCommand(ReadCommand&&) = delete;

    void run(
            std::function<void(std::string, std::shared_ptr<boost::asio::ip::tcp::socket>)> callback);

    ReadCommand& operator=(const ReadCommand&) = delete;

    ReadCommand& operator=(ReadCommand&&) = delete;

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

} // namespace boost_asio_tcp_echo
} // namespace client

#endif // BOOST_ASIO_TCP_ECHO_CLIENT_READCOMMAND_HPP_
