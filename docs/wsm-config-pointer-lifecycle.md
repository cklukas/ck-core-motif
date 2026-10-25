---
nav_exclude: true
---

# WSM configuration pointer lifetime

Screen configuration records now initialize all attribute-array pointers to
`NULL`.  Configuration clearing checks the pointer itself rather than the
attribute count, so zero-attribute formats with an allocated empty array are
also released safely before the count is reset.
