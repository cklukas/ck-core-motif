---
nav_exclude: true
---

# Mrm callback fixup count

Callback fixup now rejects negative descriptor counts as well as counts beyond
the supported list limit.  This prevents malformed descriptors from being
treated as empty callback lists while preserving valid callback ordering and
resolution.
