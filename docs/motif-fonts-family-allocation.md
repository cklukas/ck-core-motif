---
nav_exclude: true
---
# Font selector family-list allocation

Font selector family refresh now checks the temporary `XmString` array size
against both host `size_t` and Xt `Cardinal` byte limits, and checks the
allocation before filling it, passing the bounded size explicitly as an Xt
`Cardinal`. If the array cannot be created, the refresh
returns with the existing family list intact instead of dereferencing a null
pointer.
