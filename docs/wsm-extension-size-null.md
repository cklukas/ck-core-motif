---
nav_exclude: true
---

# WSM extension sizing

Request and reply size calculations now use the same null-safe string length
helper as the packer.  Null extension entries contribute an empty string's
length instead of reaching `strlen`, keeping buffer sizing consistent with the
serialized payload.
