# UIL output-size overflow handling

The UIL serializer accumulates resource and compression-table sizes in signed
`int` fields because those sizes are part of the historical resource-context
interface.  The checked accumulation helpers use `INT_MAX` as an overflow
marker.  That marker must never be passed to `UrmResizeResourceContext`,
`bzero`, or the resource-context size fields as if it were a valid allocation.

`UilP2Out.c` now rejects an overflowed literal, argument compression table, or
class compression table before allocating or writing it.  The existing
diagnostic path reports the rejected output and the caller can continue with
the next UIL input instead of attempting an impossible allocation.
