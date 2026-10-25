---
nav_exclude: true
---

# WSM configuration reply arrays

Configuration reply packing now rejects a positive attribute count paired with
a null attribute array before sizing or serializing the reply.  This avoids
null traversal for malformed application-provided reply state while leaving
valid configuration replies unchanged.
