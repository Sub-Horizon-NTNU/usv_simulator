
#include <asio.hpp>
#include <iostream>
#include <Data.hpp>
#include <thread>
#include <chrono>

class UDPClient{
public:

    UDPClient(std::string ip_address, unsigned short listen_port);

    ~UDPClient();

    bool try_receive_sitl(sitl_data &sitl_data);
    
    void send_string(std::string &msg);

    private:        
        asio::io_context io_context_;
        asio::ip::udp::socket socket_;

        asio::ip::udp::endpoint remote_endpoint_;

        std::atomic<bool> running_;
        std::atomic<bool> new_data_;
        sitl_data recv_data_;
        sitl_data sitl_data_;
        std::array<char,100> recv_buffer_;

        static constexpr uint16_t magic = 18458;

};





