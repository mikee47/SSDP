/**
 * MessageSpec.cpp
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

#include "include/Network/SSDP/MessageSpec.h"
#include <Data/CStringArray.h>

namespace SSDP
{
namespace
{
#define XX(type, str) str "\0"
DEFINE_FSTR(fstr_NotifySubtype, SSDP_NOTIFY_SUBTYPE_MAP(XX))
#undef XX
} // namespace

NotifySubtype getNotifySubtype(const char* subtype)
{
	int n = CStringArray(fstr_NotifySubtype).indexOf(subtype);
	return (n < 0) ? NTS_OTHER : NotifySubtype(n);
}

} // namespace SSDP

String toString(SSDP::NotifySubtype subtype)
{
	return CStringArray(fstr_NotifySubtype)[subtype];
}

String toString(SSDP::SearchTarget target)
{
	switch(target) {
	case TARGET_ROOT:
		return "root";
	case TARGET_TYPE:
		return "type";
	case TARGET_UUID:
		return "uuid";
	case TARGET_ALL:
		return "all";
	default:
		return "UNK";
	}
}

String toString(SSDP::SearchMatch match)
{
	switch(match) {
	case MATCH_ROOT:
		return "root";
	case MATCH_UUID:
		return "uuid";
	case MATCH_TYPE:
		return "type";
	default:
		return "UNK";
	}
}
