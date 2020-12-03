/**
 * Message.cpp
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
 * You should have received a copy of the GNU General Public License along with this library.
 * If not, see <https://www.gnu.org/licenses/>.
 *
 ****/

#include "include/Network/SSDP/Message.h"
#include <FlashString/Vector.hpp>

namespace
{
#define XX(tag) DEFINE_FSTR_LOCAL(str_msgtype_##tag, #tag)
SSDP_MESSAGE_TYPE_MAP(XX)
#undef XX

#define XX(tag) &str_msgtype_##tag,
DEFINE_FSTR_VECTOR(msgtypeStrings, FlashString, SSDP_MESSAGE_TYPE_MAP(XX))
#undef XX
} // namespace

namespace SSDP
{
DEFINE_FSTR(SSDP_DISCOVER, "\"ssdp:discover\"");
DEFINE_FSTR(UPNP_ROOTDEVICE, "upnp:rootdevice");
DEFINE_FSTR(SSDP_ALL, "ssdp:all");

Message::Message(const BasicMessage& msg)
{
	clear();

	type = msg.type;
	remoteIP = msg.remoteIP;
	remotePort = msg.remotePort;

	for(unsigned i = 0; i < msg.count(); ++i) {
		auto& header = msg[i];
		operator[](header.name) = header.value;
	}
}

HttpError BasicMessage::parse(char* data, size_t len)
{
	auto err = BasicHttpHeaders::parse(data, len, HTTP_BOTH);
	if(err != HPE_OK) {
		return err;
	}

	switch(BasicHttpHeaders::type()) {
	case HTTP_REQUEST:
		switch(BasicHttpHeaders::method()) {
		case HttpMethod::MSEARCH: {
			auto man = operator[]("MAN");
			if(SSDP_DISCOVER != man) {
				debug_e("[SSDP] MAN field wrong (%s)", man ?: "(null)");
				err = HPE_INVALID_HEADER_TOKEN;
				break;
			}
			type = MessageType::msearch;
			break;
		}

		case HttpMethod::NOTIFY:
			type = MessageType::notify;
			break;

		default:
			err = HPE_INVALID_METHOD;
		}
		break;

	case HTTP_RESPONSE:
		type = MessageType::response;
		break;

	default:
		err = HPE_INVALID_METHOD;
	}

	return err;
}

}; // namespace SSDP

String toString(SSDP::MessageType type)
{
	return msgtypeStrings[unsigned(type)];
}
