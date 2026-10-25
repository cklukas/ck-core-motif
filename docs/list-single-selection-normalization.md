---
nav_exclude: true
---

# List single-selection normalization

An application can change a List from a multiple-selection policy to single
or browse selection while several positions remain selected internally.  The
next item insertion used to assert that this transient state was impossible.

List insertion now keeps the first existing selection, clears stale extras,
and then applies the normal single-selection insertion rule.  Valid single,
browse, multiple, and extended selection behavior is unchanged.  The static
regression check is `tests/automated/check-list-single-selection.py`.
