---
nav_exclude: true
---
# Mrm zero-count descriptor inputs

Mrm widget-record argument-list builders reject zero-count inputs before
computing their one-entry-minus-one descriptor sizes. Interface modules may
legitimately contain zero top-level widgets; that case uses a header-only
descriptor instead of subtracting one. This prevents unsigned underflow and
oversized context growth while preserving valid empty and nonempty UID records.

The source-level regression is `tests/automated/check-mrm-zero-counts.py`.
