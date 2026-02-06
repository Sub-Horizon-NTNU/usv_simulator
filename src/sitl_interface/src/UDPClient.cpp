
#include "UDPClient.hpp"


    UDPClient::UDPClient(std::string ip_address, unsigned short listen_port = 9002)
    :   io_context_(), 
        socket_(io_context_,
        asio::ip::udp::endpoint(asio::ip::udp::v4(),listen_port)),
        remote_endpoint_(asio::ip::address::from_string(ip_address),listen_port)
    {
        
    }
    UDPClient::~UDPClient()
    {
        socket_.cancel();   
    }

    bool UDPClient::try_receive_sitl(sitl_data &sitl_data)
    {             
        std::error_code error;
        asio::ip::udp::endpoint  sender_endpoint;
        size_t len = socket_.receive_from(asio::buffer(recv_buffer_,40),  sender_endpoint, 0, error);
        if(!error && len== sizeof(sitl_data))
        {
            try
            {  
                // Copy 40 bytes received from the buffer into received data
                memcpy(&recv_data_, &recv_buffer_, sizeof(recv_data_));
                if(recv_data_.magic == magic) 
                {
                    //std::cout << std::to_string(recv_data_.magic)<< std::endl;;
                    sitl_data = std::move(recv_data_);
                    remote_endpoint_ = sender_endpoint;
                    return true;
                }
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << "\n";
                return false;
            }
        }
        return false;
    }   
    
    void UDPClient::send_string(std::string &msg)
    {
        socket_.send_to(asio::buffer(msg), remote_endpoint_);
    }






