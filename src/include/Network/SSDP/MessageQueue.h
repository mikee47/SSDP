/****
 * MessageQueue.h
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

#include "MessageSpec.h"
#include <Timer.h>

namespace SSDP
{
/**
 * @brief A callback function must be provided to do the actual sending
 * @param ms Message spec. to action, must delete when finished with it
 */
using MessageDelegate = Delegate<void(MessageSpec* ms)>;

/**
 * @brief Queue of objects managed by a single timer
 */
class MessageQueue
{
public:
	MessageQueue(MessageDelegate delegate);

	~MessageQueue()
	{
		clear();
	}

	void clear();

	unsigned count();

	/**
	 * @brief Set a callback to handle sending a message
	 * @Param delegate
	 */
	void setCallback(MessageDelegate delegate)
	{
		this->delegate = delegate;
	}

	/**
	 * @brief Schedule a message to start after the given interval has elapsed
	 * @param ms The template spec. for constructing the message(s)
	 * @param intervalMs How long to wait before sending
	 *
	 * The UPnP spec. requires that messages are sent after random delays, hence the interval.
	 * MessagesSpec objects must be created using the `new` allocator and are deleted after sending.
	 */
	void add(MessageSpec* ms, uint32_t intervalMs);

	/**
	 * @brief Determine if a matching message specification is already queued.
	 * @param ms
	 * @retval bool true if the given spec. is already queued.
	 *
	 * See `MessageSpec` operator== definition for how comparison is performed.
	 */
	bool contains(const MessageSpec& ms) const;

	/**
	 * @brief Remove any messages for this object
	 * @retval unsigned Number of messages removed
	 */
	unsigned remove(void* object);

private:
	void setTimer();

	Timer timer;
	MessageDelegate delegate;
	MessageSpec* head{nullptr};
	bool timerSet{false};
};

} // namespace SSDP
