# Text source read-buffer allocation

`ReadSource` keeps a reusable conversion buffer for source blocks. A failed
initial allocation or resize must stop the read before the conversion code
dereferences the buffer. Growth now uses a temporary pointer and updates the
stored buffer and length only after the allocation succeeds; an inconsistent
null buffer is rejected as well. Existing source contents remain owned by the
text source when a resize cannot be satisfied.

The behavior is deliberately a safe read failure. It preserves the existing
Text source API and avoids turning transient memory pressure into a crash or a
partially updated buffer.
