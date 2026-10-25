---
nav_exclude: true
---
# Mrm data-payload validation

Mrm data records store either a simple payload or a chain of fixed-size
overflow segments. The reader now checks simple payload capacity against the
record boundary and validates every overflow segment header, payload size,
segment number, and cumulative byte count before copying. Malformed records
therefore fail with `MrmNOT_VALID` instead of reading beyond the record or the
resource context.
