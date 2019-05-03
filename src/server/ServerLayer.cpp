#include "server/ServerLayer.hpp"

#include "server/ReceiveCommand.hpp"
#include "server/ResponseCommand.hpp"

#include <iostream>
#include <utility>

namespace boost_asio_tcp_echo
{
namespace server
{

ServerLayer::ServerLayer(
        boost::asio::io_context &ioContext,
        std::shared_ptr<Settings> settings)
        : ioContext_(ioContext),
          settings_(settings),
          acceptor_(
                std::make_shared<boost::asio::ip::tcp::acceptor>(ioContext))
{

}

ServerLayer::~ServerLayer()
{

}

bool ServerLayer::start()
{
    if (this->acceptor_->is_open()) {
        return false;
    }

    // bind ANY address (interface).
    boost::asio::ip::tcp::endpoint endpoint(
                boost::asio::ip::tcp::v4(), this->settings_->getPort());
    boost::system::error_code ec;

    this->acceptor_->open(endpoint.protocol(), ec);
    if (ec) {
        std::cout << "ServerLayer::start: " << ec.message() << std::endl;
        return false;
    }

    this->acceptor_->set_option(
            boost::asio::ip::tcp::acceptor::reuse_address(true), ec);
    if (ec) {
        std::cout << "ServerLayer::start: " << ec.message() << std::endl;
        return false;
    }

    this->acceptor_->bind(endpoint, ec);
    if (ec) {
        std::cout << "ServerLayer::start: " << ec.message() << std::endl;
        return false;
    }

    this->acceptor_->listen(boost::asio::socket_base::max_listen_connections, ec);
    if (ec) {
        std::cout << "ServerLayer::start: " << ec.message() << std::endl;
        return false;
    }


    this->accept(this->acceptor_);

    return true;
}

void ServerLayer::stop()
{
    if (!this->acceptor_->is_open()) {
        return;
    }

    boost::system::error_code ec;
    this->acceptor_->close(ec);
    if (ec)
    {
        std::cout << "ServerLayer::stop: " << ec.message() << std::endl;
    }
}

void ServerLayer::accept(
        std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor)
{
    auto self = this->shared_from_this();
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(this->ioContext_);

    acceptor->async_accept(
            *socket, std::bind(&ServerLayer::onAccept, self, acceptor, socket,
            std::placeholders::_1));
}

void ServerLayer::onAccept(
        std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor,
        std::shared_ptr<boost::asio::ip::tcp::socket> socket,
        const boost::system::error_code &ec)
{
    if (ec == boost::asio::error::operation_aborted) {
        // If call `stop()` or `boost::asio::io_context::stop()` ...
        return;
    }

    if (ec) {
        std::cout << "ServerLayer::onAccept: " << ec.message() << std::endl;
    } else {

        auto self = this->shared_from_this();

        auto receiveCommand =
                std::make_shared<server::ReceiveCommand>(
                        this->ioContext_, this->settings_, std::move(socket));
        receiveCommand->run(
                std::bind(&ServerLayer::onReceive, self, std::placeholders::_1,
                std::placeholders::_2));
    }

    if (acceptor->is_open()) {
        // call accept() to accept next connection.
        accept(acceptor);
    }
}

void ServerLayer::onReceive(
        std::string requestBody,
        std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    std::cout << "server received!: " << requestBody << std::endl;

    auto self = this->shared_from_this();

    auto responseCommand =
            std::make_shared<ResponseCommand>(
                    this->ioContext_, this->settings_, std::move(socket),
                    "{"
                    R"EOS("respose": {"message": "Hello, Server!"})EOS"
                    R"EOS(, "1": "1", "2": "2", "3": "3", "4": "4" )EOS"
                    R"EOS(, "5": "5", "6": "6", "7": "7", "8": "8" )EOS"
                    R"EOS(, "9": "9", "10": "10", "11": "11", "12": "12" )EOS"
                    R"EOS(, "13": "13", "14": "14", "15": "15", "16": "16" )EOS"
                    R"EOS(, "17": "17", "18": "18", "19": "19", "20": "20" )EOS"
                    R"EOS(, "21": "21", "22": "22", "23": "23", "24": "24" )EOS"
                    R"EOS(, "25": "25", "26": "26", "27": "27", "28": "28" )EOS"
                    R"EOS(, "29": "29", "30": "30", "31": "31", "32": "32" )EOS"
                    R"EOS(, "33": "33", "34": "34", "35": "35", "36": "36" )EOS"
                    R"EOS(, "37": "37", "38": "38", "39": "39", "40": "40" )EOS"
                    R"EOS(, "41": "41", "42": "42", "43": "43", "44": "44" )EOS"
                    R"EOS(, "45": "45", "46": "46", "47": "47", "48": "48" )EOS"
                    R"EOS(, "49": "49", "50": "50", "51": "51", "52": "52" )EOS"
                    R"EOS(, "53": "53", "54": "54", "55": "55", "56": "56" )EOS"
                    R"EOS(, "57": "57", "58": "58", "59": "59", "60": "60" )EOS"
                    R"EOS(, "60": "60", "61": "61", "62": "62", "63": "63" )EOS"
                    R"EOS(, "64": "64", "65": "65", "66": "66", "67": "67" )EOS"
                    R"EOS(, "68": "68", "69": "69", "70": "70", "71": "71" )EOS"
                    R"EOS(, "72": "72", "73": "73", "74": "74", "75": "75" )EOS"
                    R"EOS(, "76": "76", "77": "77", "78": "78", "79": "79" )EOS"
                    R"EOS(, "80": "80", "81": "81", "82": "82", "83": "83" )EOS"
                    R"EOS(, "84": "84", "85": "85", "86": "86", "87": "87" )EOS"
                    R"EOS(, "88": "88", "89": "89", "90": "90", "91": "91" )EOS"
                    R"EOS(, "92": "92", "93": "93", "94": "94", "95": "95" )EOS"
                    R"EOS(, "96": "96", "97": "97", "98": "98", "99": "99" )EOS"
                    "}");
    responseCommand->run(
            std::bind(&ServerLayer::onWrite, self, std::placeholders::_1));
}

void ServerLayer::onWrite(std::shared_ptr<boost::asio::ip::tcp::socket> socket)
{
    boost::system::error_code ec;
    socket->close(ec);
    if (ec)
    {
        std::cout << "ServerLayer::onWrite: " << ec.message() << std::endl;
    }
}

} // namespace server
} // namespace boost_asio_tcp_echo

