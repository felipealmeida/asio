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

struct homa_recvmsg_args {
  uint64_t id;
  uint64_t completion_cookie;
  int flags;
  sockaddr_in6 peer_addr;
  uint32_t num_bpages;
  uint32_t _pad[1];
  uint32_t bpage_offsets[homa_max_bpages];
};
static_assert(sizeof(struct homa_recvmsg_args) >= 120,
		"homa_recvmsg_args shrunk");
static_assert(sizeof(struct homa_recvmsg_args) <= 120,
		"homa_recvmsg_args grew");

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

signed_size_type release_pages(socket_type s, homa_pages const& pages,
                               int flags, int homa_flags, boost::system::error_code& ec,
                               const void* addr, int addrlen)
{
  homa_recvmsg_args args;
  args.num_bpages = pages.count();

  if (args.num_bpages) {
    std::memcpy(args.bpage_offsets, pages.offsets(), pages.count()*sizeof(args.bpage_offsets[0]));
    args.flags = homa_flags;

    //fprintf(stderr, "%d %d\n", (int)args.num_bpages, (int)args.bpage_offsets[0]);
    
    msghdr msg = msghdr();
    socket_ops::init_msghdr_msg_name(msg.msg_name, addr);
    msg.msg_namelen = static_cast<int>(addrlen);
    msg.msg_control = &args;
    msg.msg_controllen = sizeof(args);
    signed_size_type result = ::recvmsg(s, &msg, flags);
    socket_ops::get_last_error(ec, result < 0);
    return result;
  }
  else
  {
    return 0;
  }
}

signed_size_type recvfrom(socket_type s, homa_pages& pages,
                          int flags, void* addr, std::size_t* addrlen,
                          uint64_t& id, std::uint64_t& completion_cookie,
                          int homa_flags, boost::system::error_code& ec)
{
  homa_recvmsg_args args;
  std::memset(&args, 0, sizeof(args));
  args.id = id;
  args.flags = homa_flags;

  msghdr msg = msghdr();
  socket_ops::init_msghdr_msg_name(msg.msg_name, addr);
  msg.msg_namelen = static_cast<int>(*addrlen);
  msg.msg_control = &args;
  msg.msg_controllen = sizeof(args);
  fprintf(stderr, "waiting to read %d %d\n", (int)args.num_bpages, args.bpage_offsets[0]);
  signed_size_type result = ::recvmsg(s, &msg, flags);
  fprintf(stderr, "result of recvmsg %d\n", (int)result);
  fprintf(stderr, "finished reading first offset at %d\n", (int)args.bpage_offsets[0]);
  pages.copy_from(args.bpage_offsets, args.num_bpages);
  id = args.id;
  completion_cookie = args.completion_cookie;
  socket_ops::get_last_error(ec, result < 0);
  *addrlen = msg.msg_namelen;
  return result;
}

signed_size_type recv(socket_type s, homa_pages& pages,
                      int flags,
                      uint64_t& id, std::uint64_t& completion_cookie,
                      int homa_flags, boost::system::error_code& ec)
{
  homa_recvmsg_args args;
  std::memset(&args, 0, sizeof(args));
  args.id = id;
  args.flags = homa_flags;

  msghdr msg = msghdr();
  msg.msg_control = &args;
  msg.msg_controllen = sizeof(args);
  fprintf(stderr, "waiting to read %d %d\n", (int)args.num_bpages, args.bpage_offsets[0]);
  signed_size_type result = ::recvmsg(s, &msg, flags);
  fprintf(stderr, "result of recvmsg %d\n", (int)result);
  fprintf(stderr, "finished reading first offset at %d\n", (int)args.bpage_offsets[0]);
  pages.copy_from(args.bpage_offsets, args.num_bpages);
  id = args.id;
  completion_cookie = args.completion_cookie;
  socket_ops::get_last_error(ec, result < 0);
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
  fprintf(stderr, "sendto %d %d\n", (int)id, (int)completion_cookie);
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
  fprintf(stderr, "sendmsg return %d\n", (int)result);
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
            && ec != boost::asio::error::try_again)) {
      fprintf(stderr, "would block or try again\n");
      return 0;
    }

    // Wait for socket to become ready.
    if (socket_ops::poll_write(s, 0, -1, ec) < 0)
      return 0;
  }
}

bool non_blocking_send_request_to(socket_type s,
                                  const socket_ops::buf* bufs, size_t count, int flags,
                                  const void* addr, std::size_t addrlen,
                                  std::uint64_t& id, std::uint64_t completion_cookie,
    boost::system::error_code& ec, size_t& bytes_transferred)
{
  for (;;)
  {
    // Write some data.
    signed_size_type bytes = homa_ops::sendto(
                                              s, bufs, count, flags, addr, addrlen, id, completion_cookie, ec);

    // Check if operation succeeded.
    if (bytes >= 0)
    {
      bytes_transferred = bytes;
      return true;
    }

    // Retry operation if interrupted by signal.
    if (ec == boost::asio::error::interrupted)
      continue;

    // Check if we need to run the operation again.
    if (ec == boost::asio::error::would_block
        || ec == boost::asio::error::try_again)
      return false;

    // Operation failed.
    bytes_transferred = 0;
    return true;
  }
}

bool non_blocking_recvfrom(socket_type s, homa_pages& pages,
                           int flags, void* addr, std::size_t* addrlen,
                           boost::system::error_code& ec, size_t& bytes_transferred, std::uint64_t& id,
                           int homa_flags)
{
  for (;;)
  {
    // Read some data.
    std::uint64_t completion_cookie = 0;
    signed_size_type bytes = homa_ops::recvfrom(s, pages, flags, addr, addrlen, id, completion_cookie, homa_flags, ec);

    // Check if operation succeeded.
    if (bytes >= 0)
    {
      bytes_transferred = bytes;
      return true;
    }

    // Retry operation if interrupted by signal.
    if (ec == boost::asio::error::interrupted)
      continue;

    // Check if we need to run the operation again.
    if (ec == boost::asio::error::would_block
        || ec == boost::asio::error::try_again)
      return false;

    // Operation failed.
    bytes_transferred = 0;
    return true;
  }
}

bool non_blocking_recv(socket_type s, homa_pages& pages,
                       int flags,
                       boost::system::error_code& ec, size_t& bytes_transferred, std::uint64_t& id,
                       int homa_flags)
{
  for (;;)
  {
    // Read some data.
    std::uint64_t completion_cookie = 0;
    signed_size_type bytes = homa_ops::recv(s, pages, flags, id, completion_cookie, homa_flags, ec);

    // Check if operation succeeded.
    if (bytes >= 0)
    {
      bytes_transferred = bytes;
      return true;
    }

    // Retry operation if interrupted by signal.
    if (ec == boost::asio::error::interrupted)
      continue;

    // Check if we need to run the operation again.
    if (ec == boost::asio::error::would_block
        || ec == boost::asio::error::try_again)
      return false;

    // Operation failed.
    bytes_transferred = 0;
    return true;
  }
}

} // namespace homa_ops
} // namespace detail
} // namespace asio
} // namespace boost

#include <boost/asio/detail/pop_options.hpp>

#endif // BOOST_ASIO_DETAIL_SOCKET_OPS_IPP
