#include "client/ClientLayer.hpp"
#include "client/Settings.hpp"
#include "server/ServerLayer.hpp"
#include "server/Settings.hpp"

#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>

#include <chrono>
#include <future>
#include <iostream>
#include <memory>

int main(int /* argc */, char ** /* *argv[] */)
{
    using namespace boost_asio_tcp_echo;

    boost::asio::io_context serverIoContext;
    auto serverWorkGuard = boost::asio::make_work_guard(serverIoContext);
    boost::asio::io_context clientIoContext;
    auto clientWorkGuard = boost::asio::make_work_guard(serverIoContext);

    auto serverSettings =
            std::make_shared<server::Settings>(
                    8080,
                    std::chrono::milliseconds(2'000),
                    std::chrono::milliseconds(2'000));
    auto serverLayer =
            std::make_shared<server::ServerLayer>(
                    serverIoContext, serverSettings);

    if (!serverLayer->start()) {
        std::cout << "failed to Server start" << std::endl;

        return 1;
    }

    auto clientSettings =
            std::make_shared<client::Settings>(
                    "localhost", 8080,
                    std::chrono::milliseconds(500),
                    std::chrono::milliseconds(500),
                    std::chrono::milliseconds(2'000),
                    std::chrono::milliseconds(2'000));
    auto clientLayer = std::make_shared<client::ClientLayer>(clientIoContext, clientSettings);
    std::future<std::string> responseFuture =
            clientLayer->request(R"EOS({"client": {"message": "Hello, Client!"}})EOS");


    auto serverFuture = std::async(std::launch::async, [&serverIoContext]() {
        try {
            serverIoContext.run();
        } catch (const std::exception &ex) {
            std::cout << "server error: " << ex.what() << std::endl;
        }
    });
    clientIoContext.run_for(std::chrono::seconds(10));

    switch (responseFuture.wait_for(std::chrono::milliseconds(5'000))) {
        case std::future_status::ready:
            try {
                std::cout << "responseFuture: " << responseFuture.get() << std::endl;
            } catch (const std::exception &ex) {
                // broken promise if error occurred/cancel from Boost.Asio operations.
                std::cout << "ex.what(): " << ex.what() << std::endl;
            }
            break;

        case std::future_status::timeout:
            std::cout << "responseFuture timeout!" << std::endl;
            break;

        case std::future_status::deferred:
            std::cout << "responseFuture deferred" << std::endl;
            break;
    }

    serverLayer->stop();

    serverWorkGuard.reset();
    clientWorkGuard.reset();

    return 0;
}
