---
nav_exclude: true
---

# WSM configuration pointer lifetime

Screen configuration records now initialize all attribute-array pointers to
`NULL`.  Configuration clearing checks the pointer itself rather than the
attribute count, so zero-attribute formats with an allocated empty array are
also released safely before the count is reset.

When a peer sends another configuration-format reply for the same screen, the
decoder clears those cached arrays before decoding the replacement.  The reply
therefore cannot orphan a prior global, window, or icon attribute array.  The
native WSM replacement fixture exchanges the configuration format twice and
checks that the replacement remains usable.
