//
// detail/impl/homa_ops.ipp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2023 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_DETAIL_HOMA_OPS_IPP
#define BOOST_ASIO_DETAIL_HOMA_OPS_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <sys/mman.h>

#include <boost/asio/detail/config.hpp>

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <new>
#include <boost/asio/detail/assert.hpp>
#include <boost/asio/detail/socket_ops.hpp>
#include <boost/asio/detail/homa_ops.hpp>
#include <boost/asio/error.hpp>

#if defined(BOOST_ASIO_WINDOWS_RUNTIME)
# include <codecvt>
# include <locale>
# include <string>
#endif // defined(BOOST_ASIO_WINDOWS_RUNTIME)

#if defined(BOOST_ASIO_WINDOWS) || defined(__CYGWIN__) \
  || defined(__MACH__) && defined(__APPLE__)
# if defined(BOOST_ASIO_HAS_PTHREADS)
#  include <pthread.h>
# endif // defined(BOOST_ASIO_HAS_PTHREADS)
#endif // defined(BOOST_ASIO_WINDOWS) || defined(__CYGWIN__)
       // || defined(__MACH__) && defined(__APPLE__)

#include <boost/asio/detail/push_options.hpp>

namespace boost {
namespace asio {
namespace detail {
namespace homa_ops {

void set_buffer(socket_type s, void* data, size_t length, boost::system::error_code& ec) {
    struct homa_set_buf_args {
      void *start;
      size_t length;
    }  args;
    args.start = data;
    args.length = length;
    const unsigned so_homa_set_buf = 10;
    auto status = ::setsockopt(s, 0xFD, so_homa_set_buf, &args,
                               sizeof(args));
    socket_ops::get_last_error(ec, status < 0);
}

signed_size_type recvfrom(socket_type s, homa_pages& pages,
                               int flags, void* addr, std::size_t* addrlen,
                               uint64_t& id, std::uint64_t& completion_cookie,
                               int homa_flags, boost::system::error_code& ec)
{
struct homa_recvmsg_args {

	/**
	 * @id: (in/out) Initially specifies the id of the desired RPC, or 0
	 * if any RPC is OK; returns the actual id received.
	 */
	uint64_t id;

	/**
	 * @completion_cookie: (out) If the incoming message is a response,
	 * this will return the completion cookie specified when the
	 * request was sent. For requests this will always be zero.
	 */
	uint64_t completion_cookie;

	/**
	 * @flags: (in) OR-ed combination of bits that control the operation.
	 * See below for values.
	 */
	int flags;

	/**
	 * @error_addr: the address of the peer is stored here when available.
	 * This field is different from the msg_name field in struct msghdr
	 * in that the msg_name field isn't set after errors. This field will
	 * always be set when peer information is available, which includes
	 * some error cases.
	 */
	sockaddr_in6 peer_addr;

	/**
	 * @num_bpages: (in/out) Number of valid entries in @bpage_offsets.
	 * Passes in bpages from previous messages that can now be
	 * recycled; returns bpages from the new message.
	 */
	uint32_t num_bpages;

	uint32_t _pad[1];

	/**
	 * @bpage_offsets: (in/out) Each entry is an offset into the buffer
	 * region for the socket pool. When returned from recvmsg, the
	 * offsets indicate where fragments of the new message are stored. All
	 * entries but the last refer to full buffer pages (HOMA_BPAGE_SIZE bytes)
	 * and are bpage-aligned. The last entry may refer to a bpage fragment and
	 * is not necessarily aligned. The application now owns these bpages and
	 * must eventually return them to Homa, using bpage_offsets in a future
	 * recvmsg invocation.
	 */
#define HOMA_MAX_MESSAGE_LENGTH 1000000
#define HOMA_BPAGE_SHIFT 16
#define HOMA_BPAGE_SIZE (1 << HOMA_BPAGE_SHIFT)
#define HOMA_MAX_BPAGES ((HOMA_MAX_MESSAGE_LENGTH + HOMA_BPAGE_SIZE - 1) \
		>> HOMA_BPAGE_SHIFT)
  uint32_t bpage_offsets[HOMA_MAX_BPAGES];
} args;
static_assert(sizeof(struct homa_recvmsg_args) >= 120,
		"homa_recvmsg_args shrunk");
static_assert(sizeof(struct homa_recvmsg_args) <= 120,
		"homa_recvmsg_args grew");
 std::memset(&args, 0, sizeof(args));
 args.id = id;
 args.flags = homa_flags;
 args.completion_cookie = 100;

  msghdr msg = msghdr();
  socket_ops::init_msghdr_msg_name(msg.msg_name, addr);
  msg.msg_namelen = static_cast<int>(*addrlen);
  msg.msg_iov = /*bufs*/ nullptr;
  msg.msg_iovlen = /*static_cast<int>(count)*/ 0;
  msg.msg_control = &args;
  msg.msg_controllen = sizeof(args);
  signed_size_type result = ::recvmsg(s, &msg, flags);
  pages.copy_from(args.bpage_offsets, args.num_bpages);
  id = args.id;
  completion_cookie = args.completion_cookie;
  socket_ops::get_last_error(ec, result < 0);
  *addrlen = msg.msg_namelen;
  return result;
}

size_t sync_recvfrom(socket_type s, socket_ops::state_type state, homa_pages& pages,
                          int flags, void* addr, std::size_t* addrlen, uint64_t& id,
                          uint64_t& completion_cookie, int homa_flags,
                          boost::system::error_code& ec)
{
  if (s == invalid_socket)
  {
    ec = boost::asio::error::bad_descriptor;
    return 0;
  }

  // Read some data.
  for (;;)
  {
    // Try to complete the operation without blocking.
    signed_size_type bytes = homa_ops::recvfrom
      (
       s, pages, flags, addr, addrlen, id, completion_cookie, homa_flags, ec);

    // Check if operation succeeded.
    if (bytes >= 0)
      return bytes;

    // Operation failed.
    if ((state & socket_ops::user_set_non_blocking)
        || (ec != boost::asio::error::would_block
          && ec != boost::asio::error::try_again))
      return 0;

    // Wait for socket to become ready.
    if (socket_ops::poll_read(s, 0, -1, ec) < 0)
      return 0;
  }
}

signed_size_type sendto(socket_type s, const socket_ops::buf* bufs,
                             size_t count, int flags, const void* addr,
                             std::size_t addrlen
                           , uint64_t& id, uint64_t completion_cookie
                           , boost::system::error_code& ec)
{
  struct homa_sendmsg_args {
    uint64_t id;
    uint64_t completion_cookie;
  } args {id, completion_cookie};
  msghdr msg = msghdr();
  socket_ops::init_msghdr_msg_name(msg.msg_name, addr);
  msg.msg_namelen = static_cast<int>(addrlen);
  msg.msg_iov = const_cast<socket_ops::buf*>(bufs);
  msg.msg_iovlen = static_cast<int>(count);
  msg.msg_control = &args;
  msg.msg_controllen = 0;
#if defined(BOOST_ASIO_HAS_MSG_NOSIGNAL)
  flags |= MSG_NOSIGNAL;
#endif // defined(BOOST_ASIO_HAS_MSG_NOSIGNAL)
  signed_size_type result = ::sendmsg(s, &msg, flags);
  id = args.id;
  socket_ops::get_last_error(ec, result < 0);
  return result;
}

size_t sync_sendto(socket_type s, socket_ops::state_type state,
                        const socket_ops::buf* bufs, size_t count, int flags, const void* addr,
                        std::size_t addrlen, uint64_t& id, uint64_t completion_cookie, boost::system::error_code& ec)
{
  if (s == invalid_socket)
  {
    ec = boost::asio::error::bad_descriptor;
    return 0;
  }

  // Write some data.
  for (;;)
  {
    // Try to complete the operation without blocking.
    signed_size_type bytes = homa_ops::sendto(
                                                s, bufs, count, flags, addr, addrlen, id, completion_cookie, ec);

    // Check if operation succeeded.
    if (bytes >= 0)
      return bytes;

    // Operation failed.
    if ((state & socket_ops::user_set_non_blocking)
        || (ec != boost::asio::error::would_block
          && ec != boost::asio::error::try_again))
      return 0;

    // Wait for socket to become ready.
    if (socket_ops::poll_write(s, 0, -1, ec) < 0)
      return 0;
  }
}

} // namespace homa_ops
} // namespace detail
} // namespace asio
} // namespace boost

#include <boost/asio/detail/pop_options.hpp>

#endif // BOOST_ASIO_DETAIL_SOCKET_OPS_IPP
