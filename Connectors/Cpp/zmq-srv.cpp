#include <iostream>
#include <sstream>
#include <string>
#include <csignal>
#include <atomic>
#include <zmq.hpp>
#include <random>



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
        zmq::socket_t socket(ctx, zmq::socket_type::rep);        
        socket.bind("tcp://0.0.0.0:5556");
        std::cout << "ZMQ test server is running on tcp://0.0.0.0:5556..." << std::endl;

        while(run){
            // 1. Receive data from DataBroker
            zmq::message_t msg;
            auto recv_result = socket.recv(msg, zmq::recv_flags::none);
            if (recv_result) {
                std::string_view msg_str{static_cast<const char*>(msg.data()), msg.size()};
                std::cout << "Received: " << msg_str << std::endl;
            }

            // 2. Respond with new data
            std::ostringstream rsp;
            rsp << "{\"Input_Value_1\":" << distrib(gen) << ",\"Input_Value_2\":" << distrib(gen) << "}";
            std::cout << rsp.str() << std::endl;
            
            zmq::message_t reply{rsp.str().data(), rsp.str().size()};
            socket.send(reply, zmq::send_flags::none);
        }        

        
    } catch (const zmq::error_t& e) {
        std::cout << e.what() << std::endl;
        return 1;

    }
        

    return 0;


}