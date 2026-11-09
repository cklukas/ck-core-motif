---
nav_exclude: true
---

# WSM reply counts

Reply packing now rejects negative and `CARD16`-unrepresentable list counts
before sizing or indexing reply arrays. This covers extension,
configuration-format, state, registered-window, and window-entry replies,
including their nested lists. An invalid count could otherwise skip validation
loops or be truncated while serialized, producing an invalid message.
