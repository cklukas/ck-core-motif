---
nav_exclude: true
---
# FontSelector resolution allocation

The resolution control builder now checks its button box, merged arguments,
and each resolution toggle before registering callbacks or storing child
pointers. If a merge or child creation fails, the temporary argument list is
released and construction returns without dereferencing an invalid widget.
