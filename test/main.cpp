// Created by Mitia Tristan on 20.12.24.

#include "include/handler/request_handler.hpp"
#include "include/request/requests.hpp"

#include <thread>
#include <iostream>

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {

    mt::network::AsyncRequestHandler asyncHandler;
    asyncHandler.run();
    mt::network::Url url{"https://sampletestfile.com/wp-content/uploads/2023/07/1.5-MB-PDF.pdf"};
    // mt::network::Url url{"https://bing.com"};
    url.resolve();
    if (const auto error = url.error(); error) {
        std::cout << error.message() << std::endl;
        return 1;
    }
    bool waiting{true};
    for (int8_t requestNumber = 0; requestNumber < 15; ++requestNumber) {
        auto request = std::make_shared< mt::network::GetRequest >(url);
        const std::string file_name{"result" + std::to_string(requestNumber) + ".pdf"};
        request->outputToFile(file_name);
        request->signupForFinished([/*&waiting,*/ request, requestNumber]() -> void {
            auto response = std::get< std::shared_ptr< mt::network::HttpResponse > >(request->response());

            std::cout << "Request number " << std::to_string(requestNumber) << " finished with status " << mt::network::http::getHttpStatusDetails(response->status()) << std::endl;
            // waiting = false;
        });
        request->signupForFailed([/*&waiting,*/ request, requestNumber]() -> void {
            std::cout << "Request number " << std::to_string(requestNumber) << " finished with error " << request->error().message() << std::endl;
            // waiting = false;
        });
        asyncHandler.addRequest(std::move(request));
    }
    std::this_thread::sleep_for(std::chrono::seconds{5});
    auto request = std::make_shared< mt::network::GetRequest >(url);
    const std::string file_name{"result" + std::to_string(10) + ".pdf"};
    request->outputToFile(file_name);
    request->signupForFinished([/*&waiting,*/ request]() -> void {
        auto response = std::get< std::shared_ptr< mt::network::HttpResponse > >(request->response());

        std::cout << "Request number " << std::to_string(10) << " finished with status " << mt::network::http::getHttpStatusDetails(response->status()) << std::endl;
        // waiting = false;
    });
    request->signupForFailed([/*&waiting,*/ request]() -> void {
        std::cout << "Request number " << std::to_string(10) << " finished with error " << request->error().message() << std::endl;
        // waiting = false;
    });
    asyncHandler.addRequest(std::move(request));
    // mt::network::handle_request(request, true, false);
    while (waiting) {
        std::cout << "Waiting for request to finish" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds{1});
    }
    asyncHandler.stop();
    return 0;
}
