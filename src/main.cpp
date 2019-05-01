#include "client/ClientLayer.hpp"
#include "client/Settings.hpp"

#include <boost/asio/io_context.hpp>

#include <chrono>
#include <future>
#include <iostream>
#include <memory>

int main(int /* argc */, char ** /* *argv[] */)
{
    using namespace boost_asio_tcp_echo;

    boost::asio::io_context ioContext;

    auto clientSettings =
            std::make_shared<client::Settings>(
                    "localhost", 8080,
                    std::chrono::milliseconds(500),
                    std::chrono::milliseconds(500),
                    std::chrono::milliseconds(2000),
                    std::chrono::milliseconds(2000)
            );
    auto clientLayer = std::make_shared<client::ClientLayer>(ioContext, clientSettings);
    std::future<std::string> responseFuture = clientLayer->request("Hello! World");

    ioContext.run();

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
    }

    return 0;
}
