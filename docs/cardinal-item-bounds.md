# Cardinal item-list allocation bounds

The public breadcrumb and segmented-control setters accept Xt `Cardinal`
item counts. Their item arrays now check both `size_t` multiplication and the
Xt `Cardinal` byte-width limit before allocation, preventing truncation on
32-bit Xt builds while retaining the existing `False`/empty-state failure
behavior.
