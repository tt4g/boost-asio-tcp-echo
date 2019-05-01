#ifndef BOOST_ASIO_TCP_ECHO_CLIENT_WRITECOMMAND_HPP_
#define BOOST_ASIO_TCP_ECHO_CLIENT_WRITECOMMAND_HPP_

#include "client/Settings.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/system/error_code.hpp>

#include <functional>
#include <memory>
#include <string>

namespace boost_asio_tcp_echo
{
namespace client
{

class WriteCommand : public std::enable_shared_from_this<WriteCommand>
{
public:

    explicit WriteCommand(
            boost::asio::io_context &ioContext,
            std::shared_ptr<Settings> settings,
            std::shared_ptr<boost::asio::ip::tcp::socket> socket,
            std::string body);

    ~WriteCommand();

    WriteCommand(const WriteCommand&) = delete;

    WriteCommand(WriteCommand&&) = delete;

    void run(
            std::function<void(std::shared_ptr<boost::asio::ip::tcp::socket>)> callback);

    WriteCommand& operator=(const WriteCommand&) = delete;

    WriteCommand& operator=(WriteCommand&&) = delete;

private:

    void handleTimeout(
            const std::shared_ptr<boost::asio::ip::tcp::socket> socket,
            const std::shared_ptr<boost::asio::steady_timer> deadLineTimer,
            const boost::system::error_code &ec);

    void handleWrite(
            std::function<void(std::shared_ptr<boost::asio::ip::tcp::socket>)> callback,
            std::shared_ptr<boost::asio::ip::tcp::socket> socket,
            const std::shared_ptr<boost::asio::steady_timer> deadLineTimer,
            const boost::system::error_code &ec,
            std::size_t bytesTransferred);

    boost::asio::io_context &ioContext_;

    std::shared_ptr<Settings> settings_;

    std::shared_ptr<boost::asio::ip::tcp::socket> socket_;

    std::string body_;

};

} // namespace boost_asio_tcp_echo
} // namespace client

#endif // BOOST_ASIO_TCP_ECHO_CLIENT_WRITECOMMAND_HPP_
