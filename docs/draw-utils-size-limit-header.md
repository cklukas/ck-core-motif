# DrawUtils size-limit header

`XmDrawBevel` checks rectangle allocation arithmetic against `UINT_MAX`, but
the source previously relied on a transitive declaration of that limit.  The
file now includes `<limits.h>` directly, keeping the overflow guard portable
across toolchains and include-order changes.
