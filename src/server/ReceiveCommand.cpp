#include "server/ReceiveCommand.hpp"

#include <boost/asio/completion_condition.hpp>
#include <boost/asio/read.hpp>

#include <iostream>
#include <iterator>
#include <limits>
#include <utility>

namespace boost_asio_tcp_echo
{
namespace server
{

ReceiveCommand::ReceiveCommand(
        boost::asio::io_context &ioContext,
        std::shared_ptr<Settings> settings,
        std::shared_ptr<boost::asio::ip::tcp::socket> socket)
        : ioContext_(ioContext),
          settings_(std::move(settings)),
          socket_(std::move(socket))
{

}

ReceiveCommand::~ReceiveCommand()
{

}

void ReceiveCommand::run(
        std::function<void(std::string, std::shared_ptr<boost::asio::ip::tcp::socket>)> callback)
{
    auto self = this->shared_from_this();
    auto deadLineTimer = std::make_shared<boost::asio::steady_timer>(this->ioContext_);
    auto receiveBuffer =
            std::make_shared<boost::asio::streambuf>(std::numeric_limits<std::size_t>::max());

    deadLineTimer->expires_after(this->settings_->getRequestReadTimeout());
    deadLineTimer->async_wait(
            std::bind(&ReceiveCommand::handleTimeout, self, this->socket_, deadLineTimer,
                      std::placeholders::_1));

    boost::asio::async_read(*(this->socket_), receiveBuffer->prepare(2 * 1'024 * 1'024),
            boost::asio::transfer_at_least(1),
            std::bind(&ReceiveCommand::handleRead, self, std::move(callback),
                     this->socket_, deadLineTimer, receiveBuffer,
                     std::placeholders::_1, std::placeholders::_2));
}

void ReceiveCommand::handleTimeout(
        const std::shared_ptr<boost::asio::ip::tcp::socket> socket,
        const std::shared_ptr<boost::asio::steady_timer> deadLineTimer,
        const boost::system::error_code &ec)
{
    if (ec == boost::asio::error::operation_aborted) {
        // Timer canceled
        return;
    }

    if (ec || boost::asio::steady_timer::clock_type::now() < deadLineTimer->expiry()) {
        boost::system::error_code cancelEc;
        socket->cancel(cancelEc);
    }

    if (ec) {
        std::cout << "ReceiveCommand::handleTimeout: " << ec.message() << std::endl;
    }
}

void ReceiveCommand::handleRead(
        std::function<void(std::string, std::shared_ptr<boost::asio::ip::tcp::socket>)> callback,
        std::shared_ptr<boost::asio::ip::tcp::socket> socket,
        const std::shared_ptr<boost::asio::steady_timer> deadLineTimer,
        std::shared_ptr<boost::asio::streambuf> receiveBuffer,
        const boost::system::error_code &ec,
        std::size_t bytesReceived)
{
    if (ec) {
        // ec is boost::asio::error::eof if socked is closed before reading the
        // expected amount of data.
        if (ec != boost::asio::error::eof) {
            // pass here: boost::asio::error::operation_aborted
            std::cout << "ReceiveCommand::handleRead: " << ec.message() << std::endl;

            return;
        }
    }

    // If read unknown size,
    // call `boost::asio::async_read<AsyncReadStream, MutableBufferSequence, ReadHandler>` repeatedly.
    // but will not call `ReadHandler` when use `boost::asio::transfer_at_least(1)`
    // and no more data exists.
    // So, if there is no data in the cancellation process by `boost::asio::steady_time`,
    // it is necessary to be ready to restore the process.
    // By the way, if use `boost::asio::transfer_at_least(0)`,
    // `ReadHandler` will be called immediately without reading the data.
//     if (bytesReceived > 0) {
//         receiveBuffer->commit(bytesReceived);
//         // consume and store receive data.
//         std::string requestBody(
//                 (std::istreambuf_iterator<char>(receiveBuffer.get())),
//                 std::istreambuf_iterator<char>());
//
//         boost::asio::async_read(*(this->socket_), receiveBuffer->prepare(2 * 1'024 * 1'024),
//                 boost::asio::transfer_at_least(1),
//                 std::bind(&ReceiveCommand::handleRead, this->shared_from_this(), std::move(callback),
//                         this->socket_, deadLineTimer, receiveBuffer,
//                         std::placeholders::_1, std::placeholders::_2));
//         return;
//     }

    deadLineTimer->expires_at(
            boost::asio::steady_timer::time_point::max());
    deadLineTimer->cancel();

    receiveBuffer->commit(bytesReceived);
    std::string requestBody(
            (std::istreambuf_iterator<char>(receiveBuffer.get())),
            std::istreambuf_iterator<char>());
    callback(std::move(requestBody), std::move(socket));
}

} // namespace server
} // namespace boost_asio_tcp_echo
