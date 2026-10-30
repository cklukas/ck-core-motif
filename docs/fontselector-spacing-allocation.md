---
nav_exclude: true
---
# FontSelector spacing allocation

The spacing control builder now checks its button box, merged arguments, and
all three spacing toggles before registering callbacks or storing child
pointers. Partial construction releases merged arguments and returns safely.
