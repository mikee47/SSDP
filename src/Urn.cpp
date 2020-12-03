/**
 * Urn.cpp
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
 * You should have received a copy of the GNU General Public License along with this library.
 * If not, see <https://www.gnu.org/licenses/>.
 *
 ****/

#include "include/Network/SSDP/Urn.h"

/*
 *
 *	none		invalid
 *	uuid		uuid:{uuid}
 *	root		             upnp:rootdevice
 *				uuid:{uuid}::upnp:rootdevice
 *	device		             urn:{domain}:device:{deviceType}:{version}
 *				uuid:{uuid}::urn:{domain}:device:{deviceType}:{version}
 *	service		             urn:{domain}:service:{serviceType}:{version}
 *				uuid:{uuid}::urn:{domain}:service:{serviceType}:{version}
 */
bool Urn::decompose(const char* s)
{
	*this = Usn{};
	auto p = strchr(s, ':');
	if(p == nullptr) {
		return false;
	}

	if(p - s == 4 && memcmp(s, "uuid", 4) == 0) {
		s = ++p;
		p = strchr(s, ':');
		if(p == nullptr) {
			//	uuid:{uuid}
			uuid = s;
			kind = Kind::uuid;
			return true;
		}

		uuid.setString(s, p - s);
		s = ++p;
		if(*s++ != ':') {
			return false;
		}
		s = ++p;

		p = strchr(s, ':');
	}

	if(strcmp(s, _F("upnp:rootdevice")) == 0) {
		kind = Kind::root;
		return true;
	}

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
		k = Kind::device;
	} else if(p - s == 7 && memcmp(s, "service", 7) == 0) {
		k = Kind::service;
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

	version = atoi(s);
	kind = k;
	return true;
}

String Urn::toString() const
{
	if(kind == Kind::none) {
		return nullptr;
	}
	String s;
	if(uuid) {
		s = F("uuid:");
		s += uuid;
		if(kind == Kind::uuid) {
			return s;
		}
		s += "::";
	}

	if(kind == Kind::root) {
		s += F("upnp:rootdevice");
		return s;
	}

	s += "urn:";
	s += domain;
	s += ':';
	if(kind == Kind::device) {
		s += F("device");
	} else if(kind == Kind::service) {
		s += F("service");
	}
	s += ':';
	s += type;
	s += ':';
	s += version;
	return s;
}

String toString(Urn::Kind kind)
{
	switch(kind) {
#define XX(tag, comment)                                                                                               \
	case Urn::Kind::tag:                                                                                               \
		return F(#tag);
		UPNP_URN_KIND_MAP(XX)
#undef XX
	default:
		return nullptr;
	}
}

bool Urn::operator==(const Urn& other) const
{
	if(kind != other.kind) {
		return false;
	}

	switch(kind) {
	case Kind::none:
	case Kind::root:
		return true;
	case Kind::uuid:
		return uuid == other.uuid;
	case Kind::device:
	case Kind::service:
		return version == other.version && domain == other.domain && type == other.type && uuid == other.uuid;
	default:
		return false;
	}
}
