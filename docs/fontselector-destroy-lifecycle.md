---
nav_exclude: true
---
# FontSelector destroy lifecycle

FontSelector teardown now handles a widget whose font information failed to
initialize, avoiding a null dereference. Encoding-list cleanup also walks its
own pointer with an explicit null guard, making partial-initialization cleanup
deterministic.
