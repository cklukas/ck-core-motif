---
nav_exclude: true
---
# Mrm buffer-pool size validation

The Mrm buffer pool has an externally visible size setting so embedders can
choose an appropriate cache capacity. A non-positive value cannot provide a
valid pool and would otherwise make later buffer acquisition dereference an
empty allocation. Initialization now rejects such values before allocation,
leaving the pool uninitialized and returning `MrmFAILURE`.

Initialization also rejects a positive count whose multiplication by the
record-buffer size would overflow either `size_t` or Xt's `Cardinal` allocation
width, protecting 32-bit and constrained builds before allocation.
