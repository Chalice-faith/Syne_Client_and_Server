
#include <iostream>
#include<boost/asio.hpp>
#include<memory>
#include <set>
using boost::asio::ip::tcp;
using namespace std;
const int max_length = 1024;
typedef std::shared_ptr<tcp::socket> socket_ptr;
std::set < std::shared_ptr<std::thread> >thread_set;

void session(socket_ptr sock) {
	try
	{
		for (;;) {
			char data[max_length];
			memset(data, 0, max_length);
			boost::system::error_code error;
			//size_t length= boost::asio::read(sock, boost::asio::buffer(data,max_length), error);
			size_t length = sock->read_some(boost::asio::buffer(data, max_length), error);
			if (error == boost::asio::error::eof) {
				std::cout << "Close the connection by peer " << std::endl;
				break;
			}
			else if (error) {
				throw boost::system::system_error(error);
			}

			cout<<"receive from"<<sock->remote_endpoint().address().to_string()<<endl;
			cout << "receive message is" << data << endl;
			//回传给对方
			boost::asio::write(*sock, boost::asio::buffer(data, length));
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception in thread:" << e.what() << '\n'<<std::endl;
	}
}
void server(boost::asio::io_context& io_contetxt,unsigned short port) {
	tcp::acceptor acceptor(io_contetxt, tcp::endpoint(tcp::v4(), port));
	for (;;) {
		socket_ptr socket(new tcp::socket(io_contetxt));
		acceptor.accept(*socket);
		auto thread = std::make_shared<std::thread>(session,socket);
		thread_set.insert(thread);
	}
}
int main()
{
    try {
		boost::asio::io_context ioc;
		server(ioc, 10086);
		for (auto& t : thread_set) {
			 t->join();
		}
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
