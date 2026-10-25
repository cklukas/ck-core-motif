---
nav_exclude: true
---

# WSM extension array state

Request and reply packing now rejects a positive extension count paired with a
null extension pointer array before sizing or indexing it.  Null individual
strings remain supported as empty strings; valid extension lists are unchanged.
