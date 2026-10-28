---
nav_exclude: true
---

# Mrm resource-reference append inputs

The shared `UrmCWR__AppendResource` helper now rejects a null resource context
or output-offset pointer before entering its key-type handling. Argument and
callback resource-reference writers both use this helper, so malformed calls
fail safely without passing invalid pointers into record allocation.
