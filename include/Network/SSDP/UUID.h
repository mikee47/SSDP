/**
 * UUID.h - Universal Unique Identifier
 *
 * See https://pubs.opengroup.org/onlinepubs/9629399/apdxa.htm.
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

#include <WString.h>

struct UUID {
	uint32_t time_low;				   // 0-3
	uint16_t time_mid;				   // 4-5
	uint16_t time_hi_and_version;	  // 6-7, version = top 4 bits
	uint8_t clock_seq_hi_and_reserved; // 8, variant = top 2 bits
	uint8_t clock_seq_low;			   // 9
	uint8_t node[6];				   // 10-15

	/**
	 * @brief Number of characters in a UUID string (excluding NUL terminator)
	 */
	static constexpr size_t stringSize = 36;

	/**
	 * @note System clock must be set or this will not produce correct results.
	 */
	bool generate();

	/**
	 * @note Get string representation of UUID
	 * @param uuid
	 * @param buffer
	 * @param bufSize Must be at least UUID_STRING_SIZE
	 * @retval size_t number of characters written (either 0 or UUID_STRING_SIZE)
	 * @note Converts UUID into a string of the form
	 *
	 * <time_low>-<time_mid>-<time_high_and_version>-<clock_seq_and_reserved><clock_seq_low>-<node>
	 *
	 * e.g. 2fac1234-31f8-11b4-a222-08002b34c003
	 */
	size_t toString(char* buffer, size_t bufSize) const;

	String toString() const;

	operator String() const
	{
		return toString();
	}
};

inline String toString(const UUID& uuid)
{
	return uuid.toString();
}
