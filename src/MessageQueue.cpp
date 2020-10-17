	/**
	 * MessageQueue.cpp
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

	#include "debug.h"
	#include "include/Network/SSDP/MessageQueue.h"

	namespace SSDP
	{
	MessageQueue::MessageQueue(MessageDelegate delegate) : delegate(delegate)

	{
		assert(delegate);

		timer.setCallback([this]() {
			timerSet = false;
			if(head == nullptr) {
				debug_e("[SSDP] Unexpected: Task queue empty");
				return;
			}

			// Remove item from queue
			auto ms = head;
			head = head->next;
			ms->next = nullptr;

			debug_d("[SSDP] Timer fired, %s for %p", toString(ms->messageType).c_str(), ms->object);

			// We're no longer responsible for ms
			this->delegate(ms);

			// If callback added another message then timer will already be set
			if(!timerSet) {
				setTimer();
			}
		});
	}

	void MessageQueue::clear()
	{
		timer.stop();

		auto p = head;
		while(p != nullptr) {
			auto next = p->next;
			delete p;
			p = next;
		}
		head = nullptr;
	}

	unsigned MessageQueue::count()
	{
		unsigned n = 0;
		auto p = head;
		while(p != nullptr) {
			++n;
			p = p->next;
		}
		return n;
	}

	void MessageQueue::add(MessageSpec* ms, uint32_t intervalMs)
	{
		assert(ms != nullptr);

		debug_d("[SSDP] MessageQueue::add(%u)", intervalMs);
		debug_d("  .object  = %p", ms->object);
		debug_d("  .remote  = %s:%u", IpAddress(ms->remoteIP).toString().c_str(), ms->remotePort);
		debug_d("  .message = %s", toString(ms->messageType).c_str());
		debug_d("  .notify  = %s", toString(ms->notifySubtype).c_str());
		debug_d("  .match   = %s", toString(ms->match).c_str());
		debug_d("  .target  = %s", toString(ms->target).c_str());
		debug_d("  .repeat  = %u", ms->repeat);

		uint32_t due = Timer::Clock::ticks() + Timer::Millis::timeToTicks(intervalMs);

		MessageSpec* prev = nullptr;
		auto p = head;
		while(p != nullptr) {
			if(int(p->due - due) > 0) {
				break;
			}
			prev = p;
			p = p->next;
		}

		ms->next = p;
		ms->due = due;
		if(prev == nullptr) {
			head = ms;
			setTimer();
		} else {
			prev->next = ms;
		}
	}

	void MessageQueue::setTimer()
	{
		if(head == nullptr) {
			return;
		}

		constexpr uint32_t minimumMessageInterval = Timer::Millis::timeToTicks<100>();

		int interval = head->due - Timer::Clock::ticks();
		if(interval < int(minimumMessageInterval)) {
			interval = minimumMessageInterval;
		}
		timer.setInterval(interval);
		timer.startOnce();
		timerSet = true;

		debug_d("[SSDP] timer set for %u ms", Timer::Millis::ticksToTime(interval).time);
	}

	bool MessageQueue::contains(MessageSpec* ms) const
	{
		assert(ms != nullptr);

		for(auto p = head; p != nullptr; p = p->next) {
			if(*p == *ms) {
				return true;
			}
		}

		return false;
	}

	} // namespace SSDP
