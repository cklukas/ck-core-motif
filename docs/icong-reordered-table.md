---
nav_exclude: true
---
# IconG reordered-table bounds

Icon gadget detail headers can be returned in an application-supplied order.
The order is one-based, but the old code accepted zero and indexed the source
table at `order[i] - 1`, reading before the table.  Its static reordered-table
buffer was also replaced without checking allocation failure.

Zero is now treated as an invalid order entry and produces a null output slot.
Buffer growth checks both host `size_t` and Xt `Cardinal` byte counts, and
commits the replacement only after allocation succeeds.
