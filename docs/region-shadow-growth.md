# Region shadow segment growth

The region shadow renderer keeps two segment arrays while it walks the
rectangle bands. Each shadow helper doubles its array when the current index
reaches the capacity.

Growth is transactional: the helper computes and validates the doubled count,
asks Xt for a replacement buffer, and updates the caller's pointer and capacity
only after the allocation succeeds. If the count cannot be represented or the
allocation fails, the existing buffer remains owned by the caller and its
capacity is reduced to the number of valid segments. Subsequent helpers retry
growth instead of overwriting valid segments or treating uninitialized entries
as drawable data. This preserves the normal Motif rendering API while making
allocation pressure safe.
