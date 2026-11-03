---
nav_exclude: true
---

# UIL no-memmove fallback

On legacy targets without `memmove`, UIL’s `_move` compatibility helper now
copies bytes forward or backward according to the overlap direction. The old
fallback allocated a temporary buffer only for one overlap case and terminated
the compiler if that allocation failed. The replacement requires no allocation,
handles both overlap directions, and treats non-positive legacy lengths as a
no-op. Modern builds that provide `memmove` remain unchanged.

The focused regression is `tests/automated/check-uil-mmove-fallback.py`.
