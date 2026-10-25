---
nav_exclude: true
---
# RCMenu dynamic-array bounds

Menu callback snapshots, exception-pane ancestry, and post-from widget lists
now check their counts before allocation growth. Unrepresentable menu state
returns safely or stops the ancestry walk; ordinary callback and posting
behavior is unchanged.

The regression checks the normalized local growth variable (`new_size`) used
by the exception-pane path, so refactoring the owning field name does not
silently invalidate coverage of the `short` capacity limit.

The source-level regression is `tests/automated/check-rcmenu-bounds.py`.
