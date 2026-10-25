# Region rectangle-shrink metadata

Region operations periodically shrink their rectangle array.  The previous
path changed `size` before `XtRealloc`; if the allocation failed it restored
the pointer but left the smaller capacity recorded, so a later insertion
could overrun the allocation.

The pointer and capacity are now committed together only after a successful
shrink.  Failed shrink attempts leave the original region storage unchanged.
