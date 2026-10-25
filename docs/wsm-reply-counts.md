---
nav_exclude: true
---

# WSM reply counts

Reply packing now rejects negative list counts before sizing or indexing any
reply arrays. This covers extension, configuration-format, state, registered
window, and window-entry replies, including their nested lists. A negative
count could otherwise skip validation loops while being serialized as a large
unsigned protocol count, producing an invalid message.
