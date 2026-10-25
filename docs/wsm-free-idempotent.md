---
nav_exclude: true
---

# WSM cleanup idempotence

`FreeRequest` and `FreeReply` now clear the common `allocated` flag after
releasing owned storage.  A repeated cleanup call therefore returns through
the existing no-op guard instead of attempting to free the same protocol
buffers twice.  The decoded data and wire formats are unchanged.
