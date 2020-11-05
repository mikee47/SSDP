/**
 * MessageSpec.h - Template specification for constructing SSDP message packets
 *
 * Rather than manually constructing a message from scratch, the framework uses a template
 * to define the type of standard message to be sent with various parameters. Callbacks
 * may be used to customise the message before sending.
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

#include "Message.h"

#define SSDP_NOTIFY_SUBTYPE_MAP(XX)                                                                                    \
	XX(ALIVE, "ssdp:alive")                                                                                            \
	XX(BYEBYE, "ssdp:byebye")                                                                                          \
	XX(UPDATE, "ssdp:update")                                                                                          \
	XX(EVENT, "upnp:propchange")

namespace SSDP
{
/**
 * @brief SSDP Notification subtype
 */
enum class NotifySubtype {
#define XX(type, str) NTS_##type,
	SSDP_NOTIFY_SUBTYPE_MAP(XX)
#undef XX
		NTS_OTHER
};

/**
 * @brief SSDP Search target types
 */
enum class SearchTarget {
	TARGET_ROOT, ///< Root devices only: `upnp:rootdevice`
	TARGET_TYPE, ///< Search for device/service type: `urn:{domain}:device:{deviceType}:{v}`
	///< or `urn:{domain}:service:{serviceType}:{v}`
	TARGET_UUID, ///< Search for specific device: `uuid:{device-UUID}`
	TARGET_ALL,  ///< All devices and services: `ssdp::all`
};

/**
 * @brief Determines the kind of match obtained when scanning incoming packets
 */
enum class SearchMatch {
	MATCH_ROOT, ///< Matched root device
	MATCH_UUID, ///< Matched with device UUID
	MATCH_TYPE, ///< Matched device or service type
};

NotifySubtype getNotifySubtype(const char* subtype);

/**
 * @brief Defines the information used to create an outgoing message
 *
 * The message queue stores these objects as a linked list.
 */
struct MessageSpec {
	void* object{nullptr}; ///< Defined by UPnP or application
	ip4_addr_t remoteIP;   ///< Where to send message
	union {
		struct {
			uint32_t remotePort : 16; ///< Port to send message
			MessageType messageType : 4;
			NotifySubtype notifySubtype : 4;
			SearchMatch match : 2;
			SearchTarget target : 2;
			uint32_t repeat : 4; ///< Number of times to repeat, 0 = send once, 1 = twice, etc.
		};
		uint32_t packed{0};
	};
	// Compare all but the repeat value
	static constexpr uint32_t packed_mask{0x03FFFFFF};

	MessageSpec(MessageType type)
	{
		messageType = type;
		next = nullptr;
	}

	MessageSpec(NotifySubtype nts)
	{
		messageType = MESSAGE_NOTIFY;
		notifySubtype = nts;
		next = nullptr;
	}

	MessageSpec(const MessageSpec& ms)
	{
		*this = ms;
		next = nullptr;
	}

	bool operator==(const MessageSpec& rhs) const
	{
		return object == rhs.object && ip_addr_cmp(&remoteIP, &rhs.remoteIP) &&
			   (packed & packed_mask) == (rhs.packed & packed_mask);
	}

private:
	// These fields are used by the message queue
	friend class MessageQueue;
	uint32_t due;	  ///< Absolute clock tick value when this message should be sent
	MessageSpec* next; ///< Next message in the list
};

} // namespace SSDP

String toString(SSDP::NotifySubtype subtype);
String toString(SSDP::SearchTarget target);
String toString(SSDP::SearchMatch match);
