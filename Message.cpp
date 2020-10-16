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
 * You should have received a copy of the GNU General Public License along with FlashString.
 * If not, see <https://www.gnu.org/licenses/>.
 *
 ****/

#include "include/SSDP/Message.h"
#include <Data/CStringArray.h>

namespace SSDP
{
DEFINE_FSTR(SSDP_MAN_DISCOVER, "\"ssdp:discover\"");
DEFINE_FSTR(UPNP_ROOTDEVICE, "upnp:rootdevice");
DEFINE_FSTR(SSDP_ALL, "ssdp:all");

#define XX(t) #t "\0"
DEFINE_FSTR_LOCAL(fstr_MessageType, SSDP_MESSAGE_TYPE_MAP(XX));
#undef XX

String getMessageTypeString(MessageType type)
{
	return CStringArray(fstr_MessageType)[type];
}

http_errno BasicMessage::parse(char* data, size_t len)
{
	auto err = BasicHttpHeaders::parse(data, len, HTTP_BOTH);
	if(err != HPE_OK) {
		return err;
	}

	switch(BasicHttpHeaders::type()) {
	case HTTP_REQUEST:
		switch(BasicHttpHeaders::method()) {
		case HTTP_MSEARCH: {
			auto man = operator[]("MAN");
			if(SSDP_MAN_DISCOVER != man) {
				debug_e("[SSDP] MAN field wrong (%s)", man ?: "(null)");
				err = HPE_INVALID_HEADER_TOKEN;
				break;
			}
			type = MESSAGE_MSEARCH;
			break;
		}

		case HTTP_NOTIFY:
			type = MESSAGE_NOTIFY;
			break;

		default:
			err = HPE_INVALID_METHOD;
		}
		break;

	case HTTP_RESPONSE:
		type = MESSAGE_RESPONSE;
		break;

	default:
		err = HPE_INVALID_METHOD;
	}

	return err;
}

}; // namespace SSDP
