#include "client/ResolveCommand.hpp"

#include <iostream>
#include <utility>

namespace boost_asio_tcp_echo
{
namespace client
{

ResolveCommand::ResolveCommand(
        boost::asio::io_context &ioContext,
        std::shared_ptr<Settings> settings)
        : std::enable_shared_from_this<ResolveCommand>(),
          ioContext_(ioContext),
          settings_(std::move(settings))
{

}

ResolveCommand::~ResolveCommand()
{

}

void ResolveCommand::run(
        std::function<void(boost::asio::ip::tcp::resolver::results_type)> callback)
{
    auto self = this->shared_from_this();
    auto resolver = std::make_shared<boost::asio::ip::tcp::resolver>(this->ioContext_);
    auto deadLineTimer = std::make_shared<boost::asio::steady_timer>(this->ioContext_);

    deadLineTimer->expires_after(this->settings_->getResolveTimeout());
    deadLineTimer->async_wait(
            std::bind(&ResolveCommand::handleTimeout, self, resolver, deadLineTimer,
                      std::placeholders::_1));

    resolver->async_resolve(this->settings_->getHost(), std::to_string(this->settings_->getPort()),
            std::bind(&ResolveCommand::handleResolve, self, std::move(callback),
                      deadLineTimer, std::placeholders::_1, std::placeholders::_2));
}

void ResolveCommand::handleTimeout(
        const std::shared_ptr<boost::asio::ip::tcp::resolver> resolver,
        const std::shared_ptr<boost::asio::steady_timer> deadLineTimer,
        const boost::system::error_code &ec)
{
    // There are times when timer cancellation does not make sense.
    // That's when function call task is already queued.
    // Workaround: double check "why call this function reason" ?
    // 1. Check error_code
    //  boost::system::error_code is boost::asio::error::operation_aborted If timer cancellation.
    //
    // 2. expire timer.
    //  If it is canceled after being added to the task queue, `expiry` is a future time.
    if (ec == boost::asio::error::operation_aborted
            || boost::asio::steady_timer::clock_type::now() < deadLineTimer->expiry()) {

        // Timer canceled
        return;
    }

    if (ec) {
        // another error.
        std::cout << "ResolveCommand::handleTimeout: " << ec.message() << std::endl;

        return;
    }

    resolver->cancel();
}

void ResolveCommand::handleResolve(
        std::function<void(boost::asio::ip::tcp::resolver::results_type)> callback,
        const std::shared_ptr<boost::asio::steady_timer> deadLineTimer,
        const boost::system::error_code& ec,
        boost::asio::ip::tcp::resolver::results_type endpoints)
{
    if (ec) {
        std::cout << "ResolveCommand::handleResolve: " << ec.message() << std::endl;

        return;
    }

    deadLineTimer->expires_at(
            boost::asio::steady_timer::time_point::max());
    deadLineTimer->cancel();

    callback(std::move(endpoints));
}

} // namespace boost_asio_tcp_echo
} // namespace client
