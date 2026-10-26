# MWM graphics size limits

`ExtendRList` uses `SIZE_MAX` when checking rectangle-array byte counts. The
graphics source now includes `<stdint.h>` explicitly, so that checked arithmetic
does not depend on a transitive system-header include.
