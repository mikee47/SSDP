/**
 * Urn.h - Construction of device/service URNs
 *
 * Copyright 2020 mikee47 <mike@sillyhouse.net>
 *
 * This file is part of the Sming UPnP Library
 *
 * This library is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, version 3 or later.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with Sming UPnP.
 * If not, see <https://www.gnu.org/licenses/>.
 *
 ****/

#pragma once

#include <WString.h>
#include <Network/SSDP/Uuid.h>

#define UPNP_URN_KIND_MAP(XX)                                                                                          \
	XX(none, "invalid")                                                                                                \
	XX(uuid, "uuid:{uuid}")                                                                                            \
	XX(root, "upnp:rootdevice"                                                                                         \
			 "uuid:{uuid}::upnp:rootdevice")                                                                           \
	XX(device, "urn:{domain}:device:{deviceType}:{version}"                                                            \
			   "uuid:{uuid}::urn:{domain}:device:{deviceType}:{version}")                                              \
	XX(service, "urn:{domain}:service:{serviceType}:{version}"                                                         \
				"uuid:{uuid}::urn:{domain}:service:{serviceType}:{version}")

namespace UPnP
{
/**
 * @brief Structure for UPnP URNs
 */
class Urn
{
public:
	enum class Kind {
#define XX(tag, comment) tag,
		UPNP_URN_KIND_MAP(XX)
#undef XX
	};

	Urn(Kind kind = Kind::none) : kind(kind)
	{
	}

	Urn(const Uuid& uuid) : kind(Kind::uuid), uuid(uuid)
	{
	}

	Urn(const Urn& urn) : kind(urn.kind), uuid(urn.uuid), domain(urn.domain), type(urn.type), version(urn.version)
	{
	}

	Urn(Urn&& urn)
		: kind(urn.kind), uuid(urn.uuid), domain(std::move(urn.domain)), type(std::move(urn.type)), version(urn.version)
	{
	}

	Urn(Kind kind, const String& uuid, const String& domain, const String& type, uint8_t version)
		: kind(kind), uuid(uuid), domain(domain), type(type), version(version ?: 1)
	{
	}

	Urn(Kind kind, const Uuid& uuid, const String& domain, const String& type, uint8_t version)
		: Urn(kind, String(uuid), domain, type, version)
	{
	}

	Urn(Kind kind, const String& uuid, const String& domain, const String& type, const String& version)
		: Urn(kind, uuid, domain, type, version.toInt())
	{
	}

	explicit Urn(const char* urn)
	{
		decompose(urn);
	}

	explicit Urn(const String& urn)
	{
		decompose(urn);
	}

	Urn& operator=(const Urn& urn)
	{
		kind = urn.kind;
		uuid = urn.uuid;
		domain = urn.domain;
		type = urn.type;
		version = urn.version;
		return *this;
	}

	Urn& operator=(const String& urn)
	{
		decompose(urn);
		return *this;
	}

	bool decompose(const char* s);

	bool decompose(const String& s)
	{
		return decompose(s.c_str());
	}

	/**
	 * @brief Get URN string
	 *
	 * For example: "urn:upnp-org:service:Basic:1"
	 */
	String toString() const;

	explicit operator String() const
	{
		return toString();
	}

	/**
	 * @brief Determine if URN is valid
	 */
	explicit operator bool() const
	{
		return kind != Kind::none;
	}

	Kind kind{};
	String uuid;
	String domain;		///< e.g. PnP::schemas_upnp_org
	String type;		///< e.g. "Basic"
	uint8_t version{1}; ///< e.g. 1
};

/**
 * @brief A UPnP root device URN
 */
class RootDeviceUrn : public Urn
{
public:
	RootDeviceUrn() : Urn(Kind::root)
	{
	}
};

/**
 * @brief A UPnP root device URN
 */
using UuidUrn = Urn;

/**
 * @brief A UPnP Device URN
 */
class DeviceUrn : public Urn
{
public:
	template <typename TVersion>
	DeviceUrn(const String& domain, const String& type, const TVersion& version)
		: Urn(Kind::device, nullptr, domain, type, version)
	{
	}

	template <typename TUuid, typename TVersion>
	DeviceUrn(const TUuid& uuid, const String& domain, const String& type, const TVersion& version)
		: Urn(Kind::device, uuid, domain, type, version)
	{
	}
};

/**
 * @brief A UPnP Service URN
 */
struct ServiceUrn : public Urn {
public:
	template <typename TVersion>
	ServiceUrn(const String& domain, const String& type, const TVersion& version)
		: Urn(Kind::service, nullptr, domain, type, version)
	{
	}

	template <typename TUuid, typename TVersion>
	ServiceUrn(const TUuid& uuid, const String& domain, const String& type, const TVersion& version)
		: Urn(Kind::service, uuid, domain, type, version)
	{
	}
};

using Usn = Urn;

} // namespace UPnP

String toString(UPnP::Urn::Kind kind);

inline String toString(const UPnP::Urn& urn)
{
	return urn.toString();
}

inline bool fromString(const char* s, UPnP::Urn& urn)
{
	return urn.decompose(s);
}

inline bool fromString(const String& s, UPnP::Urn& urn)
{
	return fromString(s.c_str(), urn);
}
