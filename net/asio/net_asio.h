#ifndef _THEFOX_NET_ASIO_NETASIO_H_
#define _THEFOX_NET_ASIO_NETASIO_H_

#ifdef WIN32
#pragma warning(disable:4819)
#pragma warning(disable:4996)
#endif

#include <boost/asio.hpp>

using namespace boost::asio;

#define THEFOX_USE_BOOST_ASIO

#include <base/common.h>
#include <net/asio/callbacks.h>
#include <net/asio/buffer.h>
#include <net/asio/tcp_connection.h>

#endif // _THEFOX_NET_ASIO_NETASIO_H_