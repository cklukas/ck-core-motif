---
nav_exclude: true
---
# Property buffer guards

Counted property serializers now reject a missing data buffer when the count
is nonzero. Empty counts remain valid and continue to replace the property
with an empty value, preserving normal workspace and embedded-client behavior.
