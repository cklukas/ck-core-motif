# Include the header for Desktop allocation limits

The Desktop child-array growth guard compares the requested allocation
against `UINT_MAX`.  `<stdint.h>` supplies the fixed-width size types used by
the surrounding arithmetic, but `UINT_MAX` is defined by `<limits.h>`.

Including the owning standard header keeps the guard portable across libc
implementations and restores clean compilation on platforms where the
transitive include did not expose `UINT_MAX`.
