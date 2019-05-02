#ifndef BOOST_ASIO_TCP_ECHO_SERVER_RESPONSECOMMAND_HPP_
#define BOOST_ASIO_TCP_ECHO_SERVER_RESPONSECOMMAND_HPP_

#include "server/Settings.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/steady_timer.hpp>

#include <functional>
#include <memory>
#include <string>

namespace boost_asio_tcp_echo
{
namespace server
{

class ResponseCommand : public std::enable_shared_from_this<ResponseCommand>
{
public:

    explicit ResponseCommand(
            boost::asio::io_context &ioContext,
            std::shared_ptr<Settings> settings,
            std::shared_ptr<boost::asio::ip::tcp::socket> socket,
            std::string responseBody);

    ~ResponseCommand();

    ResponseCommand(const ResponseCommand&) = delete;

    ResponseCommand(ResponseCommand&&) = delete;

    void run(
            std::function<void(std::shared_ptr<boost::asio::ip::tcp::socket>)> callback);

    ResponseCommand& operator=(const ResponseCommand&) = delete;

    ResponseCommand& operator=(ResponseCommand&&) = delete;

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

    std::string responseBody_;

};

} // namespace server
} // namespace boost_asio_tcp_echo

#endif // BOOST_ASIO_TCP_ECHO_SERVER_RESPONSECOMMAND_HPP_
