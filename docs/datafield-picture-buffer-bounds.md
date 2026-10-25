---
nav_exclude: true
---
# DataField picture splice buffer bounds

Picture verification now sizes its temporary splice buffer in bytes, using
the actual character length rather than pointer-size multiplication.  It
rejects malformed negative lengths and detects size addition overflow before
allocation, while preserving accepted and rejected picture-edit behavior.

The warning-enabled DataField build and Xvfb DataField regression provide the
native smoke checks for this path.
