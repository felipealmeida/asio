//
// detail/reactive_socket_send_request_to_op.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2023 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_DETAIL_REACTIVE_SOCKET_SEND_REQUEST_TO_OP_HPP
#define BOOST_ASIO_DETAIL_REACTIVE_SOCKET_SEND_REQUEST_TO_OP_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/asio/detail/config.hpp>
#include <boost/asio/detail/bind_handler.hpp>
#include <boost/asio/detail/buffer_sequence_adapter.hpp>
#include <boost/asio/detail/fenced_block.hpp>
#include <boost/asio/detail/handler_alloc_helpers.hpp>
#include <boost/asio/detail/handler_work.hpp>
#include <boost/asio/detail/memory.hpp>
#include <boost/asio/detail/reactor_op.hpp>
#include <boost/asio/detail/socket_ops.hpp>
#include <boost/asio/detail/impl/homa_ops.ipp>

#include <boost/asio/detail/push_options.hpp>

namespace boost {
namespace asio {
namespace detail {

template <typename ConstBufferSequence, typename Endpoint>
class reactive_socket_send_request_to_op_base : public reactor_op
{
public:
  reactive_socket_send_request_to_op_base(const boost::system::error_code& success_ec,
      socket_type socket, const ConstBufferSequence& buffers,
      const Endpoint& endpoint, socket_base::message_flags flags,
      func_type complete_func)
    : reactor_op(success_ec,
        &reactive_socket_send_request_to_op_base::do_perform, complete_func),
      socket_(socket),
      id_(0),
      buffers_(buffers),
      destination_(endpoint),
      flags_(flags)
  {
  }

  static status do_perform(reactor_op* base)
  {
    BOOST_ASIO_ASSUME(base != 0);
    reactive_socket_send_request_to_op_base* o(
        static_cast<reactive_socket_send_request_to_op_base*>(base));

    typedef buffer_sequence_adapter<boost::asio::const_buffer,
        ConstBufferSequence> bufs_type;

    status result;
    // if (bufs_type::is_single_buffer)
    // {
    //   result = homa_ops::non_blocking_sendto1(o->socket_,
    //       bufs_type::first(o->buffers_).data(),
    //       bufs_type::first(o->buffers_).size(), o->flags_,
    //       o->destination_.data(), o->destination_.size(),
    //       o->ec_, o->bytes_transferred_) ? done : not_done;
    // }
    // else
    {
      printf("do_perform\n");
      bufs_type bufs(o->buffers_);
      result = homa_ops::non_blocking_send_request_to(o->socket_,
          bufs.buffers(), bufs.count(), o->flags_,
                                                      o->destination_.data(), o->destination_.size(), o->id_, 0,
          o->ec_, o->bytes_transferred_) ? done : not_done;
    }

    BOOST_ASIO_HANDLER_REACTOR_OPERATION((*o, "non_blocking_sendto",
                                          o->ec_, o->bytes_transferred_, o->id_));

    return result;
  }

  std::uint64_t id_;
private:
  socket_type socket_;
  ConstBufferSequence buffers_;
  Endpoint destination_;
  socket_base::message_flags flags_;
};

template <typename ConstBufferSequence, typename Endpoint,
    typename Handler, typename IoExecutor>
class reactive_socket_send_request_to_op :
  public reactive_socket_send_request_to_op_base<ConstBufferSequence, Endpoint>
{
public:
  typedef Handler handler_type;
  typedef IoExecutor io_executor_type;

  BOOST_ASIO_DEFINE_HANDLER_PTR(reactive_socket_send_request_to_op);

  reactive_socket_send_request_to_op(const boost::system::error_code& success_ec,
      socket_type socket, const ConstBufferSequence& buffers,
      const Endpoint& endpoint, socket_base::message_flags flags,
      Handler& handler, const IoExecutor& io_ex)
    : reactive_socket_send_request_to_op_base<ConstBufferSequence, Endpoint>(
        success_ec, socket, buffers, endpoint, flags,
        &reactive_socket_send_request_to_op::do_complete),
      handler_(static_cast<Handler&&>(handler)),
      work_(handler_, io_ex)
  {
  }

  static void do_complete(void* owner, operation* base,
      const boost::system::error_code& /*ec*/,
                          std::size_t /*bytes_transferred*/)
  {
    // Take ownership of the handler object.
    BOOST_ASIO_ASSUME(base != 0);
    reactive_socket_send_request_to_op* o(static_cast<reactive_socket_send_request_to_op*>(base));
    ptr p = { boost::asio::detail::addressof(o->handler_), o, o };

    BOOST_ASIO_HANDLER_COMPLETION((*o));

    // Take ownership of the operation's outstanding work.
    handler_work<Handler, IoExecutor> w(
        static_cast<handler_work<Handler, IoExecutor>&&>(
          o->work_));

    BOOST_ASIO_ERROR_LOCATION(o->ec_);

    // Make a copy of the handler so that the memory can be deallocated before
    // the upcall is made. Even if we're not about to make an upcall, a
    // sub-object of the handler may be the true owner of the memory associated
    // with the handler. Consequently, a local copy of the handler is required
    // to ensure that any owning sub-object remains valid until after we have
    // deallocated the memory here.
    detail::binder3<Handler, boost::system::error_code, std::size_t, std::uint64_t>
      handler(o->handler_, o->ec_, o->bytes_transferred_, o->id_);
    p.h = boost::asio::detail::addressof(handler.handler_);
    p.reset();

    // Make the upcall if required.
    if (owner)
    {
      fenced_block b(fenced_block::half);
      BOOST_ASIO_HANDLER_INVOCATION_BEGIN((handler.arg1_, handler.arg2_, handler.arg3_));
      w.complete(handler, handler.handler_);
      BOOST_ASIO_HANDLER_INVOCATION_END;
    }
  }

  static void do_immediate(operation* base, bool, const void* io_ex)
  {
    // Take ownership of the handler object.
    BOOST_ASIO_ASSUME(base != 0);
    reactive_socket_send_request_to_op* o(static_cast<reactive_socket_send_request_to_op*>(base));
    ptr p = { boost::asio::detail::addressof(o->handler_), o, o };

    BOOST_ASIO_HANDLER_COMPLETION((*o));

    // Take ownership of the operation's outstanding work.
    immediate_handler_work<Handler, IoExecutor> w(
        static_cast<handler_work<Handler, IoExecutor>&&>(
          o->work_));

    BOOST_ASIO_ERROR_LOCATION(o->ec_);

    // Make a copy of the handler so that the memory can be deallocated before
    // the upcall is made. Even if we're not about to make an upcall, a
    // sub-object of the handler may be the true owner of the memory associated
    // with the handler. Consequently, a local copy of the handler is required
    // to ensure that any owning sub-object remains valid until after we have
    // deallocated the memory here.
    detail::binder2<Handler, boost::system::error_code, std::size_t>
      handler(o->handler_, o->ec_, o->bytes_transferred_);
    p.h = boost::asio::detail::addressof(handler.handler_);
    p.reset();

    BOOST_ASIO_HANDLER_INVOCATION_BEGIN((handler.arg1_, handler.arg2_));
    w.complete(handler, handler.handler_, io_ex);
    BOOST_ASIO_HANDLER_INVOCATION_END;
  }

private:
  Handler handler_;
  handler_work<Handler, IoExecutor> work_;
};

} // namespace detail
} // namespace asio
} // namespace boost

#include <boost/asio/detail/pop_options.hpp>

#endif // BOOST_ASIO_DETAIL_REACTIVE_SOCKET_SEND_REQUEST_TO_OP_HPP
