/**
 * Usn.cpp
 *
 * Copyright 2020 mikee47 <mike@sillyhouse.net>
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
 * You should have received a copy of the GNU General Public License along with Sming SSDP.
 * If not, see <https://www.gnu.org/licenses/>.
 *
 ****/

#include "include/Network/SSDP/Usn.h"

namespace UPnP
{
bool Usn::decompose(const char* s)
{
	*this = Usn();
	auto p = strchr(s, ':');
	if(p == nullptr) {
		return false;
	}
	if(p - s != 4 || memcmp(s, "uuid", 4) != 0) {
		return false;
	}
	s = ++p;

	p = strchr(s, ':');
	if(p == nullptr) {
		//	uuid:{uuid}
		if(!uuid.decompose(s)) {
			return false;
		}
		kind = Kind::device;
		return true;
	}

	if(!uuid.decompose(String(s, p - s))) {
		return false;
	}
	s = ++p;

	if(*s++ != ':') {
		return false;
	}

	p = strchr(s, ':');
	if(p == nullptr) {
		//	uuid:{uuid}::upnp:rootdevice
		if(strcmp(s, _F("upnp:rootdevice")) == 0) {
			kind = Kind::root;
			return true;
		}

		return false;
	}

	//	uuid:{uuid}::urn:{domain}:device:{deviceType}:{version}
	//	uuid:{uuid}::urn:{domain}:service:{serviceType}:{version}
	if(p - s != 3 || memcmp(s, "urn", 3) != 0) {
		return false;
	}
	s = ++p;

	p = strchr(s, ':');
	if(p == nullptr) {
		return false;
	}

	domain.setString(s, p - s);
	s = ++p;

	p = strchr(s, ':');
	if(p == nullptr) {
		return false;
	}

	Kind k;
	if(p - s == 6 && memcmp(s, "device", 6) == 0) {
		k = Kind::deviceType;
	} else if(p - s == 7 && memcmp(s, "service", 7) == 0) {
		k = Kind::serviceType;
	} else {
		return false;
	}
	s = ++p;

	p = strchr(s, ':');
	if(p == nullptr) {
		return false;
	}
	type.setString(s, p - s);
	s = ++p;

	int v = atoi(s);
	if(v <= 0 || v > 255) {
		return false;
	}
	version = v;
	kind = k;
	return true;
}

} // namespace UPnP
