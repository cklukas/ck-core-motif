---
nav_exclude: true
---
# Virtual-key binding database input

Virtual-key initialization normally obtains bindings from the Motif properties
or its built-in fallback files. If every source is unavailable, allocation of
the fallback string can still fail. In that case Motif now skips database
population safely instead of passing a null string/database into the resource
parser and binding walker. Existing valid bindings are unchanged.
