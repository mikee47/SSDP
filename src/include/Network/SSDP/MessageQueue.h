/**
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
 * You should have received a copy of the GNU General Public License along with FlashString.
 * If not, see <https://www.gnu.org/licenses/>.
 *
 ****/

#pragma once

#include "MessageSpec.h"
#include <Timer.h>

namespace SSDP
{
/**
 * @brief Queue of objects managed by a single timer
 */
class MessageQueue
{
public:
	using MessageDelegate = Delegate<void(MessageSpec* ms)>;

	MessageQueue();

	~MessageQueue()
	{
		clear();
	}

	void clear();

	unsigned count();

	void setCallback(MessageDelegate delegate)
	{
		this->delegate = delegate;
	}

	void add(MessageSpec* ms, uint32_t intervalMs);

	bool contains(MessageSpec* ms) const;

private:
	void setTimer();

private:
	Timer timer;
	MessageDelegate delegate;
	MessageSpec* head = nullptr;
	bool timerSet = false;
};

} // namespace SSDP
