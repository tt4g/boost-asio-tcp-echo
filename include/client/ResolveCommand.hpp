#ifndef BOOST_ASIO_TCP_ECHO_CLIENT_BINDCOMMAND_HPP_
#define BOOST_ASIO_TCP_ECHO_CLIENT_BINDCOMMAND_HPP_

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

class ResolveCommand : public std::enable_shared_from_this<ResolveCommand>
{
public:

    explicit ResolveCommand(
            boost::asio::io_context &ioContext,
            std::shared_ptr<Settings> settings);

    ~ResolveCommand();

    ResolveCommand(const ResolveCommand&) = delete;

    ResolveCommand(ResolveCommand&&) = delete;

    void run(
            std::function<void(boost::asio::ip::tcp::resolver::results_type)> callback);

    ResolveCommand& operator=(const ResolveCommand&) = delete;

    ResolveCommand& operator=(ResolveCommand&&) = delete;

private:

    void handleTimeout(
            const std::shared_ptr<boost::asio::ip::tcp::resolver> resolver,
            const std::shared_ptr<boost::asio::steady_timer> deadLineTimer,
            const boost::system::error_code &ec);

    void handleResolve(
            std::function<void(boost::asio::ip::tcp::resolver::results_type)> callback,
            const std::shared_ptr<boost::asio::steady_timer> deadLineTimer,
            const boost::system::error_code &ec,
            boost::asio::ip::tcp::resolver::results_type endpoints);

    boost::asio::io_context &ioContext_;

    std::shared_ptr<Settings> settings_;

};

} // namespace boost_asio_tcp_echo
} // namespace client

#endif // BOOST_ASIO_TCP_ECHO_CLIENT_BINDCOMMAND_HPP_
