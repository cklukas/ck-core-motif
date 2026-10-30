---
nav_exclude: true
---
# FontSelector Other-list guards

The Other-font list builder now validates selector/font/list state and the
family child before use. It handles a missing current-font string safely and
only publishes successfully allocated compound-string items.
