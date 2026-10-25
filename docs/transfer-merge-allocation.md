# Transfer merge allocation

`XmeConvertMerge` builds a new buffer before replacing the existing conversion
value. If that allocation fails, it now reports the existing conversion error,
releases the process lock, and leaves the original value and length unchanged.

Successful merges retain the same byte ordering and ownership transfer. The
failure path avoids dereferencing a null merged buffer or losing valid source
data under memory pressure.
