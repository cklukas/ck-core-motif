---
nav_exclude: true
---

# MWM session command-vector width

Session restore parsing grows a temporary vector of command-argument pointers
and later copies it into the session record. Both allocations now check the
element count against Xt's `Cardinal` width in addition to `size_t` overflow;
existing cleanup and allocation-failure behavior remain unchanged.
