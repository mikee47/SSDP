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
#include <FlashString/Vector.hpp>

namespace
{
#define XX(type, str) DEFINE_FSTR_LOCAL(str_subtype_##type, str)
SSDP_NOTIFY_SUBTYPE_MAP(XX)
#undef XX

#define XX(type, str) &str_subtype_##type,
DEFINE_FSTR_VECTOR(notifySubtypeStrings, FlashString, SSDP_NOTIFY_SUBTYPE_MAP(XX))
#undef XX
} // namespace

namespace SSDP
{
NotifySubtype getNotifySubtype(const char* subtype)
{
	int n = notifySubtypeStrings.indexOf(subtype);
	return (n < 0) ? NotifySubtype::OTHER : NotifySubtype(n);
}

} // namespace SSDP

String toString(SSDP::NotifySubtype subtype)
{
	return notifySubtypeStrings[unsigned(subtype)];
}

String toString(SSDP::SearchTarget target)
{
	using namespace SSDP;
	switch(target) {
	case SearchTarget::ROOT:
		return "root";
	case SearchTarget::TYPE:
		return "type";
	case SearchTarget::UUID:
		return "uuid";
	case SearchTarget::ALL:
		return "all";
	default:
		return "UNK";
	}
}

String toString(SSDP::SearchMatch match)
{
	using namespace SSDP;
	switch(match) {
	case SearchMatch::ROOT:
		return "root";
	case SearchMatch::UUID:
		return "uuid";
	case SearchMatch::TYPE:
		return "type";
	default:
		return "UNK";
	}
}
