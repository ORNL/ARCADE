#include <iostream>
#include <sstream>
#include <string>
#include <csignal>
#include <atomic>
#include <zmq.hpp>
#include <random>
#include <chrono>
#include <thread>



std::atomic<bool> run(true);

void signal_handler(int signal_num) {
    if (signal_num == SIGINT){
        run = false;
    }
}

int main(int argc, char** argv){

    std::signal(SIGINT, signal_handler);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> distrib(0.0, 5.0);

    std::vector<std::string> args(argv + 1, argv + argc);
    for (const auto& arg: args){
        std::cout << "arg: " << arg << "\n"; 
    }

    try {
        zmq::context_t ctx;
        zmq::socket_t socket(ctx, zmq::socket_type::req);        
        socket.connect("tcp://0.0.0.0:5556");
        std::cout << "Connecting to ZMQ test server running on tcp://0.0.0.0:5556..." << std::endl;

        while(run){
            // 1. Send data
            std::ostringstream rsp;
            rsp << "{\"Output_Value_1\":" << distrib(gen) << ",\"Output_Value_2\":" << distrib(gen) << "}";
            std::cout << rsp.str() << std::endl;
            
            zmq::message_t reply{rsp.str().data(), rsp.str().size()};
            socket.send(reply, zmq::send_flags::none);

            // 2. Receive data from DataBroker
            zmq::message_t msg;
            auto recv_result = socket.recv(msg, zmq::recv_flags::none);
            if (recv_result) {
                std::string_view msg_str{static_cast<const char*>(msg.data()), msg.size()};
                std::cout << "Received: " << msg_str << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }        

        
    } catch (const zmq::error_t& e) {
        std::cout << e.what() << std::endl;
        return 1;

    }
        

    return 0;


}