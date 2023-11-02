//
// ip/homa.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2023 Christopher M. Kohlhoff (chris at kohlhoff dot com)
// Copyright (c) 2023      Felipe Magno de Almeida (felipe@expertise.dev)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_IP_HOMA_HPP
#define BOOST_ASIO_IP_HOMA_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/asio/detail/config.hpp>
#include <boost/asio/basic_homa_socket.hpp>
#include <boost/asio/detail/socket_types.hpp>
#include <boost/asio/ip/basic_endpoint.hpp>
#include <boost/asio/ip/basic_resolver.hpp>
#include <boost/asio/ip/basic_resolver_iterator.hpp>
#include <boost/asio/ip/basic_resolver_query.hpp>

#include <boost/asio/detail/push_options.hpp>

namespace boost {
namespace asio {
namespace ip {

/// Encapsulates the flags needed for Homa.
/**
 * The boost::asio::ip::homa class contains flags necessary for Homa sockets.
 *
 * @par Thread Safety
 * @e Distinct @e objects: Safe.@n
 * @e Shared @e objects: Safe.
 *
 * @par Concepts:
 * Protocol, InternetProtocol.
 */
class homa
{
public:
  /// The type of a Homa endpoint.
  typedef basic_endpoint<homa> endpoint;

  /// Construct to represent the IPv4 Homa protocol.
  static homa v4() noexcept
  {
    return homa(BOOST_ASIO_OS_DEF(AF_INET));
  }

  /// Construct to represent the IPv6 Homa protocol.
  static homa v6() noexcept
  {
    return homa(BOOST_ASIO_OS_DEF(AF_INET6));
  }

  /// Obtain an identifier for the type of the protocol.
  int type() const noexcept
  {
    return BOOST_ASIO_OS_DEF(SOCK_DGRAM);
  }

  /// Obtain an identifier for the protocol.
  int protocol() const noexcept
  {
    const unsigned ipproto_homa = 0xFD;
    return ipproto_homa;
  }

  /// Obtain an identifier for the protocol family.
  int family() const noexcept
  {
    return family_;
  }

  /// The Homa socket type.
  typedef basic_homa_socket<homa> socket;

  /// The Homa resolver type.
  typedef basic_resolver<homa> resolver;

  /// Compare two protocols for equality.
  friend bool operator==(const homa& p1, const homa& p2)
  {
    return p1.family_ == p2.family_;
  }

  /// Compare two protocols for inequality.
  friend bool operator!=(const homa& p1, const homa& p2)
  {
    return p1.family_ != p2.family_;
  }

private:
  // Construct with a specific family.
  explicit homa(int protocol_family) noexcept
    : family_(protocol_family)
  {
  }

  int family_;
};

} // namespace ip
} // namespace asio
} // namespace boost

#include <boost/asio/detail/pop_options.hpp>

#endif // BOOST_ASIO_IP_HOMA_HPP
