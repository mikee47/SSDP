/**
 * Usn.h - Construction of device/service Unique Service Names
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

#pragma once

#include "Uuid.h"

/**
 * @brief Four valid forms of USN
 */
#define UPNP_USN_KIND_MAP(XX)                                                                                          \
	XX(none, "invalid")                                                                                                \
	XX(root, "uuid:{uuid}::upnp:rootdevice")                                                                           \
	XX(device, "uuid:{uuid}")                                                                                          \
	XX(deviceType, "uuid:{uuid}::urn:{domain}:device:{deviceType}:{version}")                                          \
	XX(serviceType, "uuid:{uuid}::urn:{domain}:service:{serviceType}:{version}")

namespace UPnP
{
/**
 * @brief Structure for UPnP URNs
 */
class Usn
{
public:
	enum class Kind {
#define XX(tag, comment) tag,
		UPNP_USN_KIND_MAP(XX)
#undef XX
	};

	Usn()
	{
	}

	explicit Usn(const char* usn)
	{
		decompose(usn);
	}

	explicit Usn(const String& usn)
	{
		decompose(usn);
	}

	Usn(Kind kind, const Uuid& uuid) : kind(kind), uuid(uuid)
	{
	}

	Usn(const Usn& usn) : kind(usn.kind), uuid(usn.uuid), domain(usn.domain), type(usn.type), version(usn.version)
	{
	}

	Usn(Usn&& usn)
		: kind(usn.kind), uuid(usn.uuid), domain(std::move(usn.domain)), type(std::move(usn.type)), version(usn.version)
	{
	}

	Usn(Kind kind, const Uuid& uuid, const String& domain, const String& type, uint8_t version)
		: kind(kind), uuid(uuid), domain(domain), type(type), version(version ?: 1)
	{
	}

	Usn& operator=(const Usn& usn)
	{
		kind = usn.kind;
		uuid = usn.uuid;
		domain = usn.domain;
		type = usn.type;
		version = usn.version;
		return *this;
	}

	Usn& operator=(const String& usn)
	{
		decompose(usn);
		return *this;
	}

	bool decompose(const char* s);

	bool decompose(const String& s)
	{
		return decompose(s.c_str());
	}

	/**
	 * @brief Get usn string
	 *
	 * For example: "usn:upnp-org:service:Basic:1"
	 */
	String toString() const;

	operator String() const
	{
		return toString();
	}

	/**
	 * @brief Determine if usn is valid
	 */
	explicit operator bool() const
	{
		return kind != Kind::none;
	}

	Kind kind{};
	Uuid uuid{};		///< Unique device ID
	String domain;		///< e.g. PnP::schemas_upnp_org
	String type;		///< e.g. "Basic"
	uint8_t version{1}; ///< e.g. 1
};

/**
 * @brief A UPnP root device usn
 */
class RootDeviceUsn : public Usn
{
public:
	RootDeviceUsn(const Uuid& uuid) : Usn(Kind::root, uuid)
	{
	}
};

/**
 * @brief A UPnP Device usn
 */
class DeviceTypeUsn : public Usn
{
public:
	DeviceTypeUsn(const Uuid& uuid, const String& domain, const String& type, uint8_t version)
		: Usn(Usn::Kind::deviceType, uuid, domain, type, version)
	{
	}

	DeviceTypeUsn(const Uuid& uuid, const String& domain, const String& type, const String& version)
		: DeviceTypeUsn(uuid, domain, type, version.toInt())
	{
	}
};

/**
 * @brief A UPnP Service usn
 */
struct ServiceTypeUsn : public Usn {
public:
	ServiceTypeUsn(const Uuid& uuid, const String& domain, const String& type, uint8_t version)
		: Usn(Usn::Kind::serviceType, uuid, domain, type, version)
	{
	}

	ServiceTypeUsn(const Uuid& uuid, const String& domain, const String& type, const String& version)
		: ServiceTypeUsn(uuid, domain, type, version.toInt())
	{
	}
};

} // namespace UPnP

String toString(UPnP::Usn::Kind kind);

inline String toString(const UPnP::Usn& usn)
{
	return usn.toString();
}

inline bool fromString(const char* s, UPnP::Usn& usn)
{
	return usn.decompose(s);
}

inline bool fromString(const String& s, UPnP::Usn& usn)
{
	return fromString(s.c_str(), usn);
}
