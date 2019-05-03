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
            clientLayer->request("{"
                    R"EOS("client": {"message": "Hello, Client!"})EOS"
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
