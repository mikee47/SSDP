/**
 * Server.h
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

#include <Network/UdpConnection.h>
#include "MessageQueue.h"

namespace SSDP
{
DECLARE_FSTR(SERVER_ID);

/**
 * @brief Callback type for handling an incoming message
 */
using ReceiveDelegate = Delegate<void(BasicMessage& message)>;

/**
 * @brief Callback type for sending outgoing message
 * @param msg Message with standard fields completed
 * @param ms Parameters for constructing message
 * @note The message spec. is provided by the UPnP Device Host, which then gets called
 * back to construct the message content. It then calls `sendMessage()`.
 */
using SendDelegate = Delegate<void(Message& msg, MessageSpec& ms)>;

/**
 * @brief Listens for incoming messages and manages queue of outgoing messages
 * @note The spec. talks about random intervals, etc. but to keep things simple we just
 * use a timer to spread all these messages out at regular intervals.
 * @todo Randomise the time as required by MX and keep queue ordered by time.
 * Each message is 12 bytes, adding time would make this 16.
 * Need to handle alives < 1/2 expiry time as well so timer will always be active.
 * Could also use a linked list so an additional pointer would make it 20 bytes.
 *
 * Note: This is basically another timer queue, so we could use software timers
 * directly but potentially there could be a lot of them. Better I think to use a single
 * `Timer` and drive it from that.
 */
class Server : private UdpConnection
{
public:
	Server() : messageQueue(MessageDelegate(&Server::onMessage, this))
	{
	}

	/**
	 * @brief Called from UPnP library
	 * @note May only be called once
	 */
	bool begin(ReceiveDelegate receiveCallback, SendDelegate sendCallback);

	void end();

	bool isActive()
	{
		return active;
	}

	/**
	 * @brief Send a message immediately
	 */
	bool sendMessage(const Message& msg);

	bool buildMessage(Message& msg, MessageSpec& ms);

public:
	MessageQueue messageQueue;

protected:
	void onReceive(pbuf* buf, IpAddress remoteIP, uint16_t remotePort) override;

private:
	void onTimer();
	void onMessage(MessageSpec* ms);

	ReceiveDelegate receiveDelegate = nullptr;
	SendDelegate sendDelegate = nullptr;
	bool active = false;
};

extern Server server;

} // namespace SSDP
