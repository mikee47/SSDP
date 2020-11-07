/**
 * Message.h
 *
 * Copyright 2019 mikee47 <mike@sillyhouse.net>
 *
 * This file is part of the Sming SSDP Library
 *
 * This library is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, version 3 or later.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with FlashString.
 * If not, see <https://www.gnu.org/licenses/>.
 *
 ****/

#pragma once

#include <IpAddress.h>
#include <Network/Http/BasicHttpHeaders.h>
#include <Network/Http/HttpHeaders.h>

#define SSDP_MULTICAST_IP IpAddress(239, 255, 255, 250)
static constexpr uint16_t SSDP_MULTICAST_PORT = 1900;

#define SSDP_MESSAGE_TYPE_MAP(XX)                                                                                      \
	XX(notify)                                                                                                         \
	XX(msearch)                                                                                                        \
	XX(response)

namespace SSDP
{
DECLARE_FSTR(SSDP_MAN_DISCOVER);
DECLARE_FSTR(UPNP_ROOTDEVICE);
DECLARE_FSTR(SSDP_ALL);

enum class MessageType {
#define XX(tag) tag,
	SSDP_MESSAGE_TYPE_MAP(XX)
#undef XX
};

/**
 * @brief class template for messages
 */
template <class HeaderClass> class BaseMessage : public HeaderClass
{
public:
	MessageType type{MessageType::notify};
	IpAddress remoteIP;
	uint16_t remotePort{0};
};

/**
 * @brief Handles incoming messages
 * @note Contains name/value pairs as pointers.
 */
class BasicMessage : public BaseMessage<BasicHttpHeaders>
{
public:
	HttpError parse(char* data, size_t len);
};

/**
 * @brief Message using regular HTTP header management class
 * @note More flexible than BasicMessage but requires additional memory allocations
 */
class Message : public BaseMessage<HttpHeaders>
{
};

} // namespace SSDP

String toString(SSDP::MessageType type);
