//
// homa.cpp
// ~~~~~~~
//
// Copyright (c) 2003-2023 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Copyright (c) 2023      Felipe Magno de Almeida (felipe@expertise.dev)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// Disable autolinking for unit tests.
#if !defined(BOOST_ALL_NO_LIB)
#define BOOST_ALL_NO_LIB 1
#endif // !defined(BOOST_ALL_NO_LIB)

// Test that header file is self-contained.
#include <boost/asio/ip/homa.hpp>

#include <cstring>
#include <functional>
#include <iterator>
#include <boost/asio/io_context.hpp>
#include "../unit_test.hpp"
#include "../archetypes/async_result.hpp"
#include "../archetypes/gettable_socket_option.hpp"
#include "../archetypes/io_control_command.hpp"
#include "../archetypes/settable_socket_option.hpp"

//------------------------------------------------------------------------------

// ip_homa_socket_compile test
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
// The following test checks that all public member functions on the class
// ip::homa::socket compile and link correctly. Runtime failures are ignored.

namespace ip_homa_socket_compile {

struct connect_handler
{
  connect_handler() {}
  void operator()(const boost::system::error_code&) {}
  connect_handler(connect_handler&&) {}
private:
  connect_handler(const connect_handler&);
};

struct wait_handler
{
  wait_handler() {}
  void operator()(const boost::system::error_code&) {}
  wait_handler(wait_handler&&) {}
private:
  wait_handler(const wait_handler&);
};

struct send_handler
{
  send_handler() {}
  void operator()(const boost::system::error_code&, std::size_t) {}
  send_handler(send_handler&&) {}
private:
  send_handler(const send_handler&);
};

struct receive_handler
{
  receive_handler() {}
  void operator()(const boost::system::error_code&, std::size_t) {}
  receive_handler(receive_handler&&) {}
private:
  receive_handler(const receive_handler&);
};

void test()
{
  using namespace boost::asio;
  namespace ip = boost::asio::ip;

  try
  {
    io_context ioc;
    const io_context::executor_type ioc_ex = ioc.get_executor();
    char mutable_char_buffer[128] = "";
    const char const_char_buffer[128] = "";
    socket_base::message_flags in_flags = 0;
    archetypes::settable_socket_option<void> settable_socket_option1;
    archetypes::settable_socket_option<int> settable_socket_option2;
    archetypes::settable_socket_option<double> settable_socket_option3;
    archetypes::gettable_socket_option<void> gettable_socket_option1;
    archetypes::gettable_socket_option<int> gettable_socket_option2;
    archetypes::gettable_socket_option<double> gettable_socket_option3;
    archetypes::io_control_command io_control_command;
    archetypes::immediate_handler immediate;
    archetypes::lazy_handler lazy;
    boost::system::error_code ec;
    const unsigned ipproto_homa = 0xFD;

    // basic_datagram_socket constructors.

    ip::homa::socket socket1(ioc);
    ip::homa::socket socket2(ioc, ip::homa::v4());
    ip::homa::socket socket3(ioc, ip::homa::v6());
    ip::homa::socket socket4(ioc, ip::homa::endpoint(ip::homa::v4(), 0));
    ip::homa::socket socket5(ioc, ip::homa::endpoint(ip::homa::v6(), 0));
#if !defined(BOOST_ASIO_WINDOWS_RUNTIME)
    ip::homa::socket::native_handle_type native_socket1
      = ::socket(AF_INET, SOCK_DGRAM, ipproto_homa);
    ip::homa::socket socket6(ioc, ip::homa::v4(), native_socket1);
#endif // !defined(BOOST_ASIO_WINDOWS_RUNTIME)

    ip::homa::socket socket7(ioc_ex);
    ip::homa::socket socket8(ioc_ex, ip::homa::v4());
    ip::homa::socket socket9(ioc_ex, ip::homa::v6());
    ip::homa::socket socket10(ioc_ex, ip::homa::endpoint(ip::homa::v4(), 0));
    ip::homa::socket socket11(ioc_ex, ip::homa::endpoint(ip::homa::v6(), 0));
#if !defined(BOOST_ASIO_WINDOWS_RUNTIME)
    ip::homa::socket::native_handle_type native_socket2
      = ::socket(AF_INET, SOCK_DGRAM, ipproto_homa);
    ip::homa::socket socket12(ioc_ex, ip::homa::v4(), native_socket2);
#endif // !defined(BOOST_ASIO_WINDOWS_RUNTIME)

    ip::homa::socket socket13(std::move(socket6));

    // basic_datagram_socket operators.

    socket1 = ip::homa::socket(ioc);
    socket1 = std::move(socket2);

    // basic_io_object functions.

    ip::homa::socket::executor_type ex = socket1.get_executor();
    (void)ex;

    // basic_socket functions.

    ip::homa::socket::lowest_layer_type& lowest_layer = socket1.lowest_layer();
    (void)lowest_layer;

    const ip::homa::socket& socket14 = socket1;
    const ip::homa::socket::lowest_layer_type& lowest_layer2
      = socket14.lowest_layer();
    (void)lowest_layer2;

    socket1.open(ip::homa::v4());
    socket1.open(ip::homa::v6());
    socket1.open(ip::homa::v4(), ec);
    socket1.open(ip::homa::v6(), ec);

#if !defined(BOOST_ASIO_WINDOWS_RUNTIME)
    ip::homa::socket::native_handle_type native_socket3
      = ::socket(AF_INET, SOCK_DGRAM, ipproto_homa);
    socket1.assign(ip::homa::v4(), native_socket3);
    ip::homa::socket::native_handle_type native_socket4
      = ::socket(AF_INET, SOCK_DGRAM, ipproto_homa);
    socket1.assign(ip::homa::v4(), native_socket4, ec);
#endif // !defined(BOOST_ASIO_WINDOWS_RUNTIME)

    bool is_open = socket1.is_open();
    (void)is_open;

    socket1.close();
    socket1.close(ec);

    socket1.release();
    socket1.release(ec);

    ip::homa::socket::native_handle_type native_socket5
      = socket1.native_handle();
    (void)native_socket5;

    socket1.cancel();
    socket1.cancel(ec);

    bool at_mark1 = socket1.at_mark();
    (void)at_mark1;
    bool at_mark2 = socket1.at_mark(ec);
    (void)at_mark2;

    std::size_t available1 = socket1.available();
    (void)available1;
    std::size_t available2 = socket1.available(ec);
    (void)available2;

    socket1.bind(ip::homa::endpoint(ip::homa::v4(), 0));
    socket1.bind(ip::homa::endpoint(ip::homa::v6(), 0));
    socket1.bind(ip::homa::endpoint(ip::homa::v4(), 0), ec);
    socket1.bind(ip::homa::endpoint(ip::homa::v6(), 0), ec);

    socket1.connect(ip::homa::endpoint(ip::homa::v4(), 0));
    socket1.connect(ip::homa::endpoint(ip::homa::v6(), 0));
    socket1.connect(ip::homa::endpoint(ip::homa::v4(), 0), ec);
    socket1.connect(ip::homa::endpoint(ip::homa::v6(), 0), ec);

    socket1.async_connect(ip::homa::endpoint(ip::homa::v4(), 0),
        connect_handler());
    socket1.async_connect(ip::homa::endpoint(ip::homa::v6(), 0),
        connect_handler());
    socket1.async_connect(ip::homa::endpoint(ip::homa::v4(), 0), immediate);
    socket1.async_connect(ip::homa::endpoint(ip::homa::v6(), 0), immediate);
    int i1 = socket1.async_connect(ip::homa::endpoint(ip::homa::v4(), 0), lazy);
    (void)i1;
    int i2 = socket1.async_connect(ip::homa::endpoint(ip::homa::v6(), 0), lazy);
    (void)i2;

    socket1.set_option(settable_socket_option1);
    socket1.set_option(settable_socket_option1, ec);
    socket1.set_option(settable_socket_option2);
    socket1.set_option(settable_socket_option2, ec);
    socket1.set_option(settable_socket_option3);
    socket1.set_option(settable_socket_option3, ec);

    socket1.get_option(gettable_socket_option1);
    socket1.get_option(gettable_socket_option1, ec);
    socket1.get_option(gettable_socket_option2);
    socket1.get_option(gettable_socket_option2, ec);
    socket1.get_option(gettable_socket_option3);
    socket1.get_option(gettable_socket_option3, ec);

    socket1.io_control(io_control_command);
    socket1.io_control(io_control_command, ec);

    bool non_blocking1 = socket1.non_blocking();
    (void)non_blocking1;
    socket1.non_blocking(true);
    socket1.non_blocking(false, ec);

    bool non_blocking2 = socket1.native_non_blocking();
    (void)non_blocking2;
    socket1.native_non_blocking(true);
    socket1.native_non_blocking(false, ec);

    ip::homa::endpoint endpoint1 = socket1.local_endpoint();
    (void)endpoint1;
    ip::homa::endpoint endpoint2 = socket1.local_endpoint(ec);
    (void)endpoint2;

    ip::homa::endpoint endpoint3 = socket1.remote_endpoint();
    (void)endpoint3;
    ip::homa::endpoint endpoint4 = socket1.remote_endpoint(ec);
    (void)endpoint4;

    socket1.shutdown(socket_base::shutdown_both);
    socket1.shutdown(socket_base::shutdown_both, ec);

    socket1.wait(socket_base::wait_read);
    socket1.wait(socket_base::wait_write, ec);

    socket1.async_wait(socket_base::wait_read, wait_handler());
    socket1.async_wait(socket_base::wait_read, immediate);
    int i3 = socket1.async_wait(socket_base::wait_write, lazy);
    (void)i3;

    // basic_datagram_socket functions.

    // socket1.send(buffer(mutable_char_buffer));
    // socket1.send(buffer(const_char_buffer));
    // socket1.send(null_buffers());
    // socket1.send(buffer(mutable_char_buffer), in_flags);
    // socket1.send(buffer(const_char_buffer), in_flags);
    // socket1.send(null_buffers(), in_flags);
    // socket1.send(buffer(mutable_char_buffer), in_flags, ec);
    // socket1.send(buffer(const_char_buffer), in_flags, ec);
    // socket1.send(null_buffers(), in_flags, ec);

    // socket1.async_send(buffer(mutable_char_buffer), send_handler());
    // socket1.async_send(buffer(const_char_buffer), send_handler());
    // socket1.async_send(null_buffers(), send_handler());
    // socket1.async_send(buffer(mutable_char_buffer), in_flags, send_handler());
    // socket1.async_send(buffer(const_char_buffer), in_flags, send_handler());
    // socket1.async_send(null_buffers(), in_flags, send_handler());
    // socket1.async_send(buffer(mutable_char_buffer), immediate);
    // socket1.async_send(buffer(const_char_buffer), immediate);
    // socket1.async_send(null_buffers(), immediate);
    // socket1.async_send(buffer(mutable_char_buffer), in_flags, immediate);
    // socket1.async_send(buffer(const_char_buffer), in_flags, immediate);
    // socket1.async_send(null_buffers(), in_flags, immediate);
    // int i4 = socket1.async_send(buffer(mutable_char_buffer), lazy);
    // (void)i4;
    // int i5 = socket1.async_send(buffer(const_char_buffer), lazy);
    // (void)i5;
    // int i6 = socket1.async_send(null_buffers(), lazy);
    // (void)i6;
    // int i7 = socket1.async_send(buffer(mutable_char_buffer), in_flags, lazy);
    // (void)i7;
    // int i8 = socket1.async_send(buffer(const_char_buffer), in_flags, lazy);
    // (void)i8;
    // int i9 = socket1.async_send(null_buffers(), in_flags, lazy);
    // (void)i9;

    // socket1.send_request_to(buffer(mutable_char_buffer),
    //                 ip::homa::endpoint(ip::homa::v4(), 0), 0, 0);
    // socket1.send_to(buffer(mutable_char_buffer),
    //     ip::homa::endpoint(ip::homa::v6(), 0), 0, 0);
    // socket1.send_to(buffer(const_char_buffer),
    //     ip::homa::endpoint(ip::homa::v4(), 0), 0, 0);
    // socket1.send_to(buffer(const_char_buffer),
    //     ip::homa::endpoint(ip::homa::v6(), 0), 0, 0);
    // socket1.send_to(null_buffers(),
    //     ip::homa::endpoint(ip::homa::v4(), 0), 0, 0);
    // socket1.send_to(null_buffers(),
    //     ip::homa::endpoint(ip::homa::v6(), 0), 0, 0);
    // socket1.send_to(buffer(mutable_char_buffer),
    //     ip::homa::endpoint(ip::homa::v4(), 0), in_flags, 0, 0);
    // socket1.send_to(buffer(mutable_char_buffer),
    //     ip::homa::endpoint(ip::homa::v6(), 0), in_flags, 0, 0);
    // socket1.send_to(buffer(const_char_buffer),
    //     ip::homa::endpoint(ip::homa::v4(), 0), in_flags, 0, 0);
    // socket1.send_to(buffer(const_char_buffer),
    //     ip::homa::endpoint(ip::homa::v6(), 0), in_flags, 0, 0);
    // socket1.send_to(null_buffers(),
    //     ip::homa::endpoint(ip::homa::v4(), 0), in_flags, 0, 0);
    // socket1.send_to(null_buffers(),
    //     ip::homa::endpoint(ip::homa::v6(), 0), in_flags, 0, 0);
    // socket1.send_to(buffer(mutable_char_buffer),
    //     ip::homa::endpoint(ip::homa::v4(), 0), in_flags, 0, 0, ec);
    // socket1.send_to(buffer(mutable_char_buffer),
    //     ip::homa::endpoint(ip::homa::v6(), 0), in_flags, 0, 0, ec);
    // socket1.send_to(buffer(const_char_buffer),
    //     ip::homa::endpoint(ip::homa::v4(), 0), in_flags, 0, 0, ec);
    // socket1.send_to(buffer(const_char_buffer),
    //     ip::homa::endpoint(ip::homa::v6(), 0), in_flags, 0, 0, ec);
    // socket1.send_to(null_buffers(),
    //     ip::homa::endpoint(ip::homa::v4(), 0), in_flags, 0, 0, ec);
    // socket1.send_to(null_buffers(),
    //     ip::homa::endpoint(ip::homa::v6(), 0), in_flags, 0, 0, ec);

    // socket1.async_send_to(buffer(mutable_char_buffer),
    //     ip::homa::endpoint(ip::homa::v4(), 0), send_handler());
    // socket1.async_send_to(buffer(mutable_char_buffer),
    //     ip::homa::endpoint(ip::homa::v6(), 0), send_handler());
    // socket1.async_send_to(buffer(const_char_buffer),
    //     ip::homa::endpoint(ip::homa::v4(), 0), send_handler());
    // socket1.async_send_to(buffer(const_char_buffer),
    //     ip::homa::endpoint(ip::homa::v6(), 0), send_handler());
    // socket1.async_send_to(null_buffers(),
    //     ip::homa::endpoint(ip::homa::v4(), 0), send_handler());
    // socket1.async_send_to(null_buffers(),
    //     ip::homa::endpoint(ip::homa::v6(), 0), send_handler());
    // socket1.async_send_to(buffer(mutable_char_buffer),
    //     ip::homa::endpoint(ip::homa::v4(), 0), in_flags, send_handler());
    // socket1.async_send_to(buffer(mutable_char_buffer),
    //     ip::homa::endpoint(ip::homa::v6(), 0), in_flags, send_handler());
    // socket1.async_send_to(buffer(const_char_buffer),
    //     ip::homa::endpoint(ip::homa::v4(), 0), in_flags, send_handler());
    // socket1.async_send_to(buffer(const_char_buffer),
    //     ip::homa::endpoint(ip::homa::v6(), 0), in_flags, send_handler());
    // socket1.async_send_to(null_buffers(),
    //     ip::homa::endpoint(ip::homa::v4(), 0), in_flags, send_handler());
    // socket1.async_send_to(null_buffers(),
    //     ip::homa::endpoint(ip::homa::v6(), 0), in_flags, send_handler());
    // socket1.async_send_to(buffer(mutable_char_buffer),
    //     ip::homa::endpoint(ip::homa::v4(), 0), immediate);
    // socket1.async_send_to(buffer(mutable_char_buffer),
    //     ip::homa::endpoint(ip::homa::v6(), 0), immediate);
    // socket1.async_send_to(buffer(const_char_buffer),
    //     ip::homa::endpoint(ip::homa::v4(), 0), immediate);
    // socket1.async_send_to(buffer(const_char_buffer),
    //     ip::homa::endpoint(ip::homa::v6(), 0), immediate);
    // socket1.async_send_to(null_buffers(),
    //     ip::homa::endpoint(ip::homa::v4(), 0), immediate);
    // socket1.async_send_to(null_buffers(),
    //     ip::homa::endpoint(ip::homa::v6(), 0), immediate);
    // socket1.async_send_to(buffer(mutable_char_buffer),
    //     ip::homa::endpoint(ip::homa::v4(), 0), in_flags, immediate);
    // socket1.async_send_to(buffer(mutable_char_buffer),
    //     ip::homa::endpoint(ip::homa::v6(), 0), in_flags, immediate);
    // socket1.async_send_to(buffer(const_char_buffer),
    //     ip::homa::endpoint(ip::homa::v4(), 0), in_flags, immediate);
    // socket1.async_send_to(buffer(const_char_buffer),
    //     ip::homa::endpoint(ip::homa::v6(), 0), in_flags, immediate);
    // socket1.async_send_to(null_buffers(),
    //     ip::homa::endpoint(ip::homa::v4(), 0), in_flags, immediate);
    // socket1.async_send_to(null_buffers(),
    //     ip::homa::endpoint(ip::homa::v6(), 0), in_flags, immediate);
    // int i10 = socket1.async_send_to(buffer(mutable_char_buffer),
    //     ip::homa::endpoint(ip::homa::v4(), 0), lazy);
    // (void)i10;
    // int i11 = socket1.async_send_to(buffer(mutable_char_buffer),
    //     ip::homa::endpoint(ip::homa::v6(), 0), lazy);
    // (void)i11;
    // int i12 = socket1.async_send_to(buffer(const_char_buffer),
    //     ip::homa::endpoint(ip::homa::v4(), 0), lazy);
    // (void)i12;
    // int i13 = socket1.async_send_to(buffer(const_char_buffer),
    //     ip::homa::endpoint(ip::homa::v6(), 0), lazy);
    // (void)i13;
    // int i14 = socket1.async_send_to(null_buffers(),
    //     ip::homa::endpoint(ip::homa::v4(), 0), lazy);
    // (void)i14;
    // int i15 = socket1.async_send_to(null_buffers(),
    //     ip::homa::endpoint(ip::homa::v6(), 0), lazy);
    // (void)i15;
    // int i16 = socket1.async_send_to(buffer(mutable_char_buffer),
    //     ip::homa::endpoint(ip::homa::v4(), 0), in_flags, lazy);
    // (void)i16;
    // int i17 = socket1.async_send_to(buffer(mutable_char_buffer),
    //     ip::homa::endpoint(ip::homa::v6(), 0), in_flags, lazy);
    // (void)i17;
    // int i18 = socket1.async_send_to(buffer(const_char_buffer),
    //     ip::homa::endpoint(ip::homa::v4(), 0), in_flags, lazy);
    // (void)i18;
    // int i19 = socket1.async_send_to(buffer(const_char_buffer),
    //     ip::homa::endpoint(ip::homa::v6(), 0), in_flags, lazy);
    // (void)i19;
    // int i20 = socket1.async_send_to(null_buffers(),
    //     ip::homa::endpoint(ip::homa::v4(), 0), in_flags, lazy);
    // (void)i20;
    // int i21 = socket1.async_send_to(null_buffers(),
    //     ip::homa::endpoint(ip::homa::v6(), 0), in_flags, lazy);
    // (void)i21;

    // socket1.receive(buffer(mutable_char_buffer));
    // socket1.receive(null_buffers());
    // socket1.receive(buffer(mutable_char_buffer), in_flags);
    // socket1.receive(null_buffers(), in_flags);
    // socket1.receive(buffer(mutable_char_buffer), in_flags, ec);
    // socket1.receive(null_buffers(), in_flags, ec);

    // socket1.async_receive(buffer(mutable_char_buffer), receive_handler());
    // socket1.async_receive(null_buffers(), receive_handler());
    // socket1.async_receive(buffer(mutable_char_buffer), in_flags,
    //     receive_handler());
    // socket1.async_receive(null_buffers(), in_flags, receive_handler());
    // socket1.async_receive(buffer(mutable_char_buffer), immediate);
    // socket1.async_receive(null_buffers(), immediate);
    // socket1.async_receive(buffer(mutable_char_buffer), in_flags, immediate);
    // socket1.async_receive(null_buffers(), in_flags, immediate);
    // int i22 = socket1.async_receive(buffer(mutable_char_buffer), lazy);
    // (void)i22;
    // int i23 = socket1.async_receive(null_buffers(), lazy);
    // (void)i23;
    // int i24 = socket1.async_receive(buffer(mutable_char_buffer),
    //     in_flags, lazy);
    // (void)i24;
    // int i25 = socket1.async_receive(null_buffers(), in_flags, lazy);
    // (void)i25;

    ip::homa::endpoint endpoint;
    // socket1.receive_from(buffer(mutable_char_buffer), endpoint, 0, 0);
    // socket1.receive_from(null_buffers(), endpoint, 0, 0);
    //socket1.receive_from(buffer(mutable_char_buffer), endpoint, in_flags, 0, 0);
    //socket1.receive_from(null_buffers(), endpoint, in_flags, 0, 0);
    //socket1.receive_from(buffer(mutable_char_buffer), endpoint, in_flags, ec, 0, 0);
    //socket1.receive_from(null_buffers(), endpoint, in_flags, ec, 0, 0);

    // socket1.async_receive_from(buffer(mutable_char_buffer),
    //     endpoint, receive_handler());
    // socket1.async_receive_from(null_buffers(),
    //     endpoint, receive_handler());
    // socket1.async_receive_from(buffer(mutable_char_buffer),
    //     endpoint, in_flags, receive_handler());
    // socket1.async_receive_from(null_buffers(),
    //     endpoint, in_flags, receive_handler());
    // socket1.async_receive_from(buffer(mutable_char_buffer),
    //     endpoint, immediate);
    // socket1.async_receive_from(null_buffers(),
    //     endpoint, immediate);
    // socket1.async_receive_from(buffer(mutable_char_buffer),
    //     endpoint, in_flags, immediate);
    // socket1.async_receive_from(null_buffers(),
    //     endpoint, in_flags, immediate);
    // int i26 = socket1.async_receive_from(buffer(mutable_char_buffer),
    //     endpoint, lazy);
    // (void)i26;
    // int i27 = socket1.async_receive_from(null_buffers(),
    //     endpoint, lazy);
    // (void)i27;
    // int i28 = socket1.async_receive_from(buffer(mutable_char_buffer),
    //     endpoint, in_flags, lazy);
    // (void)i28;
    // int i29 = socket1.async_receive_from(null_buffers(),
    //     endpoint, in_flags, lazy);
    // (void)i29;
  }
  catch (std::exception&)
  {
  }
}

} // namespace ip_homa_socket_compile

//------------------------------------------------------------------------------

// ip_homa_socket_runtime test
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
// The following test checks the runtime operation of the ip::homa::socket class.

namespace ip_homa_socket_runtime {

void handle_send_request(size_t expected_bytes_sent,
                         const boost::system::error_code& err, size_t bytes_sent,
                         std::uint64_t id)
{
  std::cout << "err " << err << " " << err.message() << std::endl;
  std::cout << "id of request " << id << std::endl;
  std::cout << "bytes sent " << bytes_sent << " expected " << expected_bytes_sent << std::endl;
  BOOST_ASIO_CHECK(!err);
  // BOOST_ASIO_CHECK(expected_bytes_sent == bytes_sent);
}

void handle_recv_request(char* send_msg, std::size_t send_msg_size, uint8_t* buffer,
                         size_t expected_bytes_recvd,
                         const boost::system::error_code& err, size_t bytes_recvd,
                         const boost::asio::homa_pages& pages, std::uint64_t id)
{
  std::cout << "handle_recv_request" << std::endl;
  BOOST_ASIO_CHECK(!err);
  BOOST_ASIO_CHECK(expected_bytes_recvd == bytes_recvd);
  BOOST_ASIO_CHECK(memcmp(send_msg, (buffer + pages.offsets()[0]), send_msg_size) == 0);
}

void test()
{
  using namespace std; // For memcmp and memset.
  using namespace boost::asio;
  namespace ip = boost::asio::ip;

  namespace bindns = std;
  using bindns::placeholders::_1;
  using bindns::placeholders::_2;
  using bindns::placeholders::_3;
  using bindns::placeholders::_4;

  io_context ioc;

  ip::homa::socket s1(ioc, ip::homa::endpoint(ip::homa::v4(), 0));

  auto set_buffer = [] (auto& s) -> auto {
    // Set up the buffer region.
#define HOMA_BPAGE_SHIFT 16
#define HOMA_BPAGE_SIZE (1 << HOMA_BPAGE_SHIFT)

    const int buf_size = 1000*HOMA_BPAGE_SIZE;
    std::cout << "page size " << HOMA_BPAGE_SIZE << " buf_size " << buf_size
              << " pages " << buf_size/HOMA_BPAGE_SIZE << std::endl;
    std::unique_ptr<uint8_t, void(*)(uint8_t*)> buffer_region
      (static_cast<uint8_t*>
       ( ::mmap(NULL, buf_size, PROT_READ|PROT_WRITE,
                MAP_PRIVATE|MAP_ANONYMOUS, 0, 0))
       
       , [] (uint8_t* p)
       {
         if (p != MAP_FAILED)
           munmap(p, buf_size);
       });

    if (buffer_region.get() == MAP_FAILED) {
      abort();
    }
    auto buffer = boost::asio::buffer(buffer_region.get(), buf_size);
    s.set_buffers(buffer);
    std::memset(buffer_region.get(), '0', buf_size);
    return std::make_pair(buffer, std::move(buffer_region));
  };
  auto buffer1 = set_buffer(s1);

  ip::homa::endpoint target_endpoint = s1.local_endpoint();
  target_endpoint.address(ip::address_v4::loopback());

  ip::homa::socket s2(ioc);
  s2.open(ip::homa::v4());
  auto buffer2 = set_buffer(s2);
  s2.bind(ip::homa::endpoint(ip::homa::v4(), 0));
  std::cout << "sender socket bound to " << s2.local_endpoint() << std::endl;;
  char send_msg[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  char response_msg[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  request_id send_request_id;
  uint64_t send_completion_cookie = 111;
  s2.send_request_to(buffer(send_msg, (sizeof(send_msg)-1)), target_endpoint, send_request_id, send_completion_cookie);

  ip::homa::endpoint sender_endpoint;
  homa_pages pages;
  uint64_t completion_cookie = 112;
  request_id received_request_id;
  size_t bytes_recvd = s1.receive_request_from(pages, sender_endpoint, received_request_id, completion_cookie);
  BOOST_ASIO_CHECK(completion_cookie == 0);
  
  BOOST_ASIO_CHECK(completion_cookie == 0);
  BOOST_ASIO_CHECK(bytes_recvd == (sizeof(send_msg)-1));
  BOOST_ASIO_CHECK(pages.count() == 1);
  BOOST_ASIO_CHECK(memcmp(send_msg, (buffer1.first + pages.offsets()[0]).data(), (sizeof(send_msg)-1)) == 0);

  /// release pages
  //s1.release_pages(pages, sender_endpoint);

  s1.send_reply_to(buffer(response_msg, (sizeof(response_msg)-1)),
                   sender_endpoint, received_request_id, completion_cookie);
  BOOST_ASIO_CHECK(completion_cookie == 0);

  bytes_recvd = s2.receive_reply_from(pages, target_endpoint,
                                      send_request_id, completion_cookie);

  BOOST_ASIO_CHECK(completion_cookie == send_completion_cookie);
  BOOST_ASIO_CHECK(bytes_recvd == (sizeof(send_msg)-1));
  BOOST_ASIO_CHECK(pages.count() == 1);
  BOOST_ASIO_CHECK(memcmp(response_msg, (buffer2.first + pages.offsets()[0]).data(), (sizeof(response_msg)-2)) == 0);
  //s2.release_pages(pages, sender_endpoint);
  
  // memset(recv_msg, 0, sizeof(recv_msg));

  fprintf(stderr, "async send\n");
  // target_endpoint = sender_endpoint;
  s1.async_send_request_to(buffer(send_msg, (sizeof(send_msg)-1)), sender_endpoint, 0,
                           bindns::bind(handle_send_request, (sizeof(send_msg)-1), _1, _2, _3));
  // fprintf(stderr, "async receive\n");
  // s2.async_receive_request_from
  //   (target_endpoint,
  //    bindns::bind(handle_recv_request, send_msg, sizeof(send_msg)-1,
  //                 (std::uint8_t*)buffer2.first.data(),
  //                 sizeof(send_msg)-1, _1, _2, _3, _4));
  fprintf(stderr, "async receive request\n");
  s2.async_receive_request
    (
     bindns::bind(handle_recv_request, send_msg, sizeof(send_msg)-1,
                  (std::uint8_t*)buffer2.first.data(),
                  sizeof(send_msg)-1, _1, _2, _3, _4));
  
  ioc.run();
}

} // namespace ip_homa_socket_runtime

//------------------------------------------------------------------------------

// ip_homa_resolver_compile test
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// The following test checks that all public member functions on the class
// ip::homa::resolver compile and link correctly. Runtime failures are ignored.

namespace ip_homa_resolver_compile {

struct resolve_handler
{
  resolve_handler() {}
  void operator()(const boost::system::error_code&,
      boost::asio::ip::homa::resolver::results_type) {}
  resolve_handler(resolve_handler&&) {}
private:
  resolve_handler(const resolve_handler&);
};

void test()
{
  using namespace boost::asio;
  namespace ip = boost::asio::ip;

  try
  {
    io_context ioc;
    const io_context::executor_type ioc_ex = ioc.get_executor();
    archetypes::lazy_handler lazy;
    boost::system::error_code ec;
#if !defined(BOOST_ASIO_NO_DEPRECATED)
    ip::homa::resolver::query q(ip::homa::v4(), "localhost", "0");
#endif // !defined(BOOST_ASIO_NO_DEPRECATED)
    ip::homa::endpoint e(ip::address_v4::loopback(), 0);

    // basic_resolver constructors.

    ip::homa::resolver resolver(ioc);
    ip::homa::resolver resolver2(ioc_ex);

    ip::homa::resolver resolver3(std::move(resolver));

    // basic_resolver operators.

    resolver = ip::homa::resolver(ioc);
    resolver = std::move(resolver3);

    // basic_io_object functions.

    ip::homa::resolver::executor_type ex = resolver.get_executor();
    (void)ex;

    // basic_resolver functions.

    resolver.cancel();

#if !defined(BOOST_ASIO_NO_DEPRECATED)
    ip::homa::resolver::results_type results1 = resolver.resolve(q);
    (void)results1;

    ip::homa::resolver::results_type results2 = resolver.resolve(q, ec);
    (void)results2;
#endif // !defined(BOOST_ASIO_NO_DEPRECATED)

    ip::homa::resolver::results_type results3 = resolver.resolve("", "");
    (void)results3;

    ip::homa::resolver::results_type results4 = resolver.resolve("", "", ec);
    (void)results4;

    ip::homa::resolver::results_type results5 =
      resolver.resolve("", "", ip::homa::resolver::flags());
    (void)results5;

    ip::homa::resolver::results_type results6 =
      resolver.resolve("", "", ip::homa::resolver::flags(), ec);
    (void)results6;

    ip::homa::resolver::results_type results7 =
      resolver.resolve(ip::homa::v4(), "", "");
    (void)results7;

    ip::homa::resolver::results_type results8 =
      resolver.resolve(ip::homa::v4(), "", "", ec);
    (void)results8;

    ip::homa::resolver::results_type results9 =
      resolver.resolve(ip::homa::v4(), "", "", ip::homa::resolver::flags());
    (void)results9;

    ip::homa::resolver::results_type results10 =
      resolver.resolve(ip::homa::v4(), "", "", ip::homa::resolver::flags(), ec);
    (void)results10;

    ip::homa::resolver::results_type results11 = resolver.resolve(e);
    (void)results11;

    ip::homa::resolver::results_type results12 = resolver.resolve(e, ec);
    (void)results12;

#if !defined(BOOST_ASIO_NO_DEPRECATED)
    resolver.async_resolve(q, resolve_handler());
    int i1 = resolver.async_resolve(q, lazy);
    (void)i1;
#endif // !defined(BOOST_ASIO_NO_DEPRECATED)

    resolver.async_resolve("", "", resolve_handler());
    int i2 = resolver.async_resolve("", "", lazy);
    (void)i2;

    resolver.async_resolve("", "",
        ip::homa::resolver::flags(), resolve_handler());
    int i3 = resolver.async_resolve("", "",
        ip::homa::resolver::flags(), lazy);
    (void)i3;

    resolver.async_resolve(ip::homa::v4(), "", "", resolve_handler());
    int i4 = resolver.async_resolve(ip::homa::v4(), "", "", lazy);
    (void)i4;

    resolver.async_resolve(ip::homa::v4(),
        "", "", ip::homa::resolver::flags(), resolve_handler());
    int i5 = resolver.async_resolve(ip::homa::v4(),
        "", "", ip::homa::resolver::flags(), lazy);
    (void)i5;

    resolver.async_resolve(e, resolve_handler());
    int i6 = resolver.async_resolve(e, lazy);
    (void)i6;
  }
  catch (std::exception&)
  {
  }
}

} // namespace ip_homa_resolver_compile

//------------------------------------------------------------------------------

BOOST_ASIO_TEST_SUITE
(
  "ip/homa",
  //BOOST_ASIO_COMPILE_TEST_CASE(ip_homa_socket_compile::test)
  BOOST_ASIO_TEST_CASE(ip_homa_socket_runtime::test)
  // BOOST_ASIO_COMPILE_TEST_CASE(ip_homa_resolver_compile::test)
)
