#pragma once
#ifndef SIMPLE_SERIAL_H
#define SIMPLE_SERIAL_H

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
 
#include <string>
#include <vector>
#include <time.h>

typedef boost::shared_ptr<boost::asio::serial_port> serial_port_ptr;
 
#define SERIAL_PORT_READ_BUF_SIZE 256
 
class SerialPort
{
protected:
	boost::asio::io_service io_service_;
	
	serial_port_ptr port_;
	boost::mutex mutex_;
	
	uint8_t read_buf_bytes_raw_[SERIAL_PORT_READ_BUF_SIZE];
	std::vector<uint8_t> read_buf_bytes_;

	//char read_buf_raw_[SERIAL_PORT_READ_BUF_SIZE];
	//std::string read_buf_str_;
	unsigned char read_buf_raw_[SERIAL_PORT_READ_BUF_SIZE];
	std::vector<unsigned char> read_buf_str_;
 
 

private:
	SerialPort(const SerialPort &p);
	SerialPort &operator=(const SerialPort &p); 
 
	clock_t tSend;
	clock_t tRecieve;

	boost::thread t;

public:
	enum Baud {
		BR_9600 = 9600,
		BR_19200 = 19200,
		BR_115200 = 115200
	};
		
	SerialPort(void);
	virtual ~SerialPort(void);
 
	char end_of_line_char() const;
	void end_of_line_char(const char &c);
 
	virtual bool start(const char *com_port_name, Baud baud_rate=BR_9600);//int baud_rate=9600);
	virtual void stop();
 
	int write_some(const std::string &buf);
	int write_some(const char *buf, const int &size);
	int write_some_bytes(const std::vector<uint8_t> &buf, const int &size);
 
	bool get_port_name(const unsigned int &idx, std::string& port_name);
	std::vector<std::string> get_port_names();

	bool isOpen();
 
	//std::string flushData();
	std::vector<unsigned char> flushData();
	std::vector<uint8_t> flushData_bytes();

	// why are these here?
protected:
	virtual void async_read_some_();
	virtual void on_receive_(const boost::system::error_code& ec, size_t bytes_transferred);
	//virtual void on_receive_(const std::string &data);
	virtual void async_read_some_bytes_();
	virtual void on_receive_bytes_(const boost::system::error_code& ec, size_t bytes_transferred);
 
};
#endif