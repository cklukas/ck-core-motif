---
nav_exclude: true
---
# Internal list-stack allocation failures

The internal cleanup stack now checks null stack handles and failed growth
reallocations. Growth checks both host `size_t` and Xt's `Cardinal` byte limits
and passes an explicit Xt-sized allocation. A failed growth leaves the
existing stack and its top index unchanged, so cleanup code does not write
through a null buffer.
