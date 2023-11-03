//
// detail/homa_ops.hpp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2023 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_DETAIL_HOMA_OPS_HPP
#define BOOST_ASIO_DETAIL_HOMA_OPS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/asio/detail/config.hpp>

#include <boost/system/error_code.hpp>
#include <boost/asio/detail/memory.hpp>
#include <boost/asio/detail/socket_types.hpp>

#include <boost/asio/detail/push_options.hpp>

namespace boost {
namespace asio {
namespace detail {
namespace homa_ops {

#ifndef HOMA_MAX_MESSAGE_LENGTH
static const std::size_t homa_max_message_length = 1000000;
#else
static const std::size_t homa_max_message_length = HOMA_MAX_MESSAGE_LENGTH;
#endif
#ifndef HOMA_BPAGE_SIZE
#  ifndef HOMA_BPAGE_SHIFT  
static const std::size_t homa_bpage_size = (1 << 16);
#  else
static const std::size_t homa_bpage_size = (1 << HOMA_BPAGE_SHIFT);
#endif
#else
static const std::size_t homa_bpage_size = HOMA_BPAGE_SIZE;
#endif
static const std::size_t homa_max_bpages = (homa_max_message_length + homa_bpage_size)/homa_bpage_size;
static const int homa_recvmsg_request = 0x01;
static const int homa_recvmsg_response = 0x02;
static const int homa_recvmsg_nonblocking = 0x04;

}
}

/** @defgroup buffer_offset boost::asio::buffer_offset
 *
 */
class homa_pages
{
public:
  homa_pages() noexcept
    : count_(0)
  {
    std::memset(bpage_offsets_, 0, sizeof(bpage_offsets_));
  }
  
  homa_pages(uint32_t count, const std::uint32_t* bpage_offsets) noexcept
    : count_(count)
  {
    std::memcpy(bpage_offsets_, bpage_offsets, count*sizeof(bpage_offsets_[0]));
  }

  std::uint32_t* offsets() noexcept { return bpage_offsets_; }
  const std::uint32_t* offsets() const noexcept { return bpage_offsets_; }

  std::uint32_t count() const noexcept { return count_; }

  void copy_from(std::uint32_t offsets[], std::uint32_t count) {
    count_ = count;
    std::memcpy(bpage_offsets_, offsets, count*sizeof(bpage_offsets_[0]));
  }
private:
  std::uint32_t count_;
  std::uint32_t bpage_offsets_[asio::detail::homa_ops::homa_max_bpages];
};

} // namespace asio
} // namespace boost

#include <boost/asio/detail/pop_options.hpp>

#if defined(BOOST_ASIO_HEADER_ONLY)
# include <boost/asio/detail/impl/homa_ops.ipp>
#endif // defined(BOOST_ASIO_HEADER_ONLY)

#endif // BOOST_ASIO_DETAIL_HOMA_OPS_HPP
