/****
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
 * You should have received a copy of the GNU General Public License along with this library.
 * If not, see <https://www.gnu.org/licenses/>.
 *
 ****/

#pragma once

#include "Message.h"
#include <IpAddress.h>

#define SSDP_NOTIFY_SUBTYPE_MAP(XX)                                                                                    \
	XX(alive, "ssdp:alive")                                                                                            \
	XX(byebye, "ssdp:byebye")                                                                                          \
	XX(update, "ssdp:update")                                                                                          \
	XX(event, "upnp:propchange")

namespace SSDP
{
/**
 * @brief SSDP Notification subtype
 */
enum class NotifySubtype {
#define XX(type, str) type,
	SSDP_NOTIFY_SUBTYPE_MAP(XX)
#undef XX
		OTHER
};

/**
 * @brief SSDP Search target types
 */
enum class SearchTarget {
	root, ///< Root devices only: `upnp:rootdevice`
	type, ///< Search for device/service type: `urn:{domain}:device:{deviceType}:{v}`
	///< or `urn:{domain}:service:{serviceType}:{v}`
	uuid, ///< Search for specific device: `uuid:{device-UUID}`
	all,  ///< All devices and services: `ssdp::all`
};

/**
 * @brief Determines the kind of match obtained when scanning incoming packets
 */
enum class SearchMatch {
	root, ///< Matched root device
	uuid, ///< Matched with device UUID
	type, ///< Matched device or service type
};

NotifySubtype getNotifySubtype(const char* subtype);

/**
 * @brief Defines the information used to create an outgoing message
 *
 * The message queue stores these objects as a linked list.
 */
class MessageSpec
{
public:
	MessageSpec(MessageType type)
	{
		data.messageType = uint8_t(type);
		next = nullptr;
	}

	MessageSpec(MessageType type, SearchTarget target, void* object = nullptr)
	{
		data.messageType = uint8_t(type);
		data.target = uint8_t(target);
		m_object = object;
		next = nullptr;
	}

	MessageSpec(NotifySubtype nts, SearchTarget target, void* object = nullptr)
		: MessageSpec(MessageType::notify, target, object)
	{
		data.notifySubtype = uint8_t(nts);
	}

	/**
	 * @brief Construct a new message spec for a specific match type
	 * @param ms Template message spec
	 * @param match The match type
	 * @param object Target for message
	 */
	MessageSpec(const MessageSpec& ms, SearchMatch match, void* object)
	{
		*this = ms;
		next = nullptr;
		data.match = uint8_t(match);
		m_object = object;
	}

	bool operator==(const MessageSpec& rhs) const
	{
		return m_object == rhs.m_object && remoteIp() == rhs.remoteIp() &&
			   (data.packed & packed_mask) == (rhs.data.packed & packed_mask);
	}

	/**
	 * @brief Get the remote IP address
	 */
	IpAddress remoteIp() const
	{
		return m_remoteIp;
	}

	/**
	 * @brief Get the remote port number
	 */
	uint16_t remotePort() const
	{
		return data.remotePort;
	}

	/**
	 * @brief Get the target object pointer
	 *
	 * This is templated to provide cleaner code. Example:
	 *
	 *   MyObject* object = ms.object<MyObject>();
	 */
	template <class Object> Object* object() const
	{
		return static_cast<Object*>(m_object);
	}

	/**
	 * @brief Get the message type
	 */
	MessageType type() const
	{
		return MessageType(data.messageType);
	}

	/**
	 * @brief Get the notification sub-type
	 */
	NotifySubtype notifySubtype() const
	{
		return NotifySubtype(data.notifySubtype);
	}

	/**
	 * @brief Get the search match type
	 */
	SearchMatch match() const
	{
		return SearchMatch(data.match);
	}

	/**
	 * @brief Get the search target
	 */
	SearchTarget target() const
	{
		return SearchTarget(data.target);
	}

	/**
	 * @brief Set the search target
	 */
	void setTarget(SearchTarget target)
	{
		data.target = uint8_t(target);
	}

	/**
	 * @brief Set the remote address and port
	 */
	void setRemote(IpAddress address, uint16_t port)
	{
		m_remoteIp = address;
		data.remotePort = port;
	}

	/**
	 * @brief Set number of times to repeat message
	 */
	void setRepeat(uint8_t count)
	{
		data.repeat = count;
	}

	/**
	 * @brief Get current repeat value
	 */
	uint8_t repeat() const
	{
		return data.repeat;
	}

	/**
	 * @brief Check if message should be repeated and adjust counter
	 */
	bool shouldRepeat()
	{
		if(data.repeat == 0) {
			return false;
		}
		--data.repeat;
		return true;
	}

private:
	void* m_object{nullptr}; ///< Defined by UPnP or application
	IpAddress m_remoteIp{};  ///< Where to send message
	union Data {
		struct {
			uint32_t remotePort : 16;   ///< Port to send message
			uint32_t messageType : 4;   ///< MessageType
			uint32_t notifySubtype : 4; ///< NotifySubtype
			uint32_t match : 2;			///< SearchMatch
			uint32_t target : 2;		///< SearchTarget
			uint32_t repeat : 4;		///< Number of times to repeat, 0 = send once, 1 = twice, etc.
		};
		uint32_t packed{0};
	};
	Data data;
	// Compare all but the repeat value
	static constexpr uint32_t packed_mask{0x03FFFFFF};

	// These fields are used by the message queue
	friend class MessageQueue;
	uint32_t due;	  ///< Absolute clock tick value when this message should be sent
	MessageSpec* next; ///< Next message in the list
};

} // namespace SSDP

String toString(SSDP::NotifySubtype subtype);
String toString(SSDP::SearchTarget target);
String toString(SSDP::SearchMatch match);
