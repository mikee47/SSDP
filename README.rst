SSDP
====

.. highlight:: text

`Simple Service Discovery Protocol <https://en.wikipedia.org/wiki/Simple_Service_Discovery_Protocol>`.

Key points from UPnP 2.0 specification.

I'm choosing to avoid the issue of 'multi-homed' devices, where both IPv4 and IPv6
are being used, and probably also WiFi+Ethernet combinations.
Basically whenever multiple IP stacks are involved.


Advertisement
-------------

Multicast using ``ssdp:alive`` messages.

3 messages for each root device::

   upnp:rootdevice
   uuid:device-UUID
   urn:<domain>:<deviceType:ver>

2 for each embedded device::

   uuid:device-UUID
   urn:<domain>:<deviceType:ver>

1 for each service type in each device::

   urn<domain>:<serviceType:ver>


Initial advertisements 'should be sent as quickly as possible'.
Subsequent advertisements 'are allowed to be spread over time'.

For orderly shutdown, multicast ``ssdp:byebye`` messages as for ``ssdp:alive``.

Note that ``ssdp:update`` messages are only mentioned in the spec. wrt. mult-homed devices.


::

   Devices should wait a random interval (e.g. 0 - 100 ms) before sending an initial
   set of advertisements in order to reduce the likelihood of network storms;
   this random interval should also be applied on occasions where the device obtains
   a new IP address or a new UPnP-enabled interface is installed.

   Due to the unreliable nature of UDP, devices should send the entire set of discovery
   messages more than once with some delay between sets e.g. a few hundred milliseconds.

   To avoid network congestion discovery messages should not be sent more than three times.

   The device shall re-send its advertisements periodically prior to expiration of the
   duration specified in the CACHE-CONTROL header field; it is Recommended that such
   refreshing of advertisements be done at a randomly-distributed interval of less than
   one-half of the advertisement expiration time, so as to provide the opportunity
   for recovery from lost advertisements before the advertisement expires, and to
   distribute over time the advertisement refreshment of multiple devices on the network
   in order to avoid spikes in network traffic.

   Note that UDP packets are also bounded in length (perhaps as small as 512
   Bytes in some implementations); each discovery message shall fit entirely in a single UDP
   packet. There is no guarantee that the above 3+2d+k messages will arrive in a particular
   order.


Search request
--------------

M-SEARCH: "Please tell me about yourselves, but don't all shout at once."

::

   ssdp:all
      Search for all devices and services.

   upnp:rootdevice
      Search for root devices only.

   uuid:device-UUID
      Search for a particular device.

   urn:<domain>:device:deviceType:ver
      Search for any device of this type.
      
   urn:<domain>:service:serviceType:ver
      Search for any service of this type.

   Period characters in <domain> are always substituted with hyphens (RFC 2141).


Not clear on how to handle version numbers at present. The specs. say only minor versions
are backward compatible, which why perhaps we only see major numbers in interface
definitions. e.g. ``Basic:1`` not ``Basic:1.0``.


Search reponse
--------------

::

   Any device responding to a unicast M-SEARCH should respond within 1 second.

In response to an M-SEARCH request, if ST header in request was::

   ssdp:all
      Respond 3+2d+k times for a root device with d embedded devices and s embedded services
      but only k distinct service types.
      Value for ST header must be the same as for the NT header in NOTIFY messages with ssdp:alive.

   upnp:rootdevice
      Respond once for root device.

   uuid:device-UUID
      Respond once for each matching device, root or embedded.

   urn:<domain>:device:deviceType:v
      Respond once for each matching device, root or embedded.
      Should specify the version of the device type contained in the M-SEARCH request.

   urn:<domain>:service:serviceType:v
      Respond once for each matching service type. 
      Should specify the version of the service type contained in the M-SEARCH request.



Descriptions
------------

The ``LOCATION`` field is for the device description *or enclosing device in the case of a service*.

This implies that we never respond with a service description, which makes sense:

-  The device description provides key information about its services
-  The service description contains action lists or state variable tables

Only the device description is required to learn about services, whilst the
service description is only required if the Control Point needs to interact with that
service.


Points arising
--------------

So we need a filter which then gets passed through the device stack.
Each response must be sent on a schedule, not all together, so we'll need to set up a timer.
We'll also need to track state something like the DescriptionStream.
Actually, what we can do is create an enumerator which iterates through the entire device
stack. That will take out the complexity from here and DescriptionStream.
We'll need an additional `Item tag` so we can differentiate. This can either be a virtual
method or we could use a union with all the different Item types plus a separate tag field.
That could also contain the search filter information as input.

Move all this stuff into an `SsdpResponder` class?
