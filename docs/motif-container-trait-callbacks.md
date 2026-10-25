---
nav_exclude: true
---
# Container trait callback validation

Container item and navigator traits are extension points. A class may expose
a trait record with only part of the callback surface implemented. Container
now checks each callback before invoking it, using the existing fallback view,
width, and selection values when an item callback is absent and skipping a
navigator that cannot be read and updated. This keeps partial third-party
traits from causing null function-pointer calls while preserving the existing
trait ABI.
