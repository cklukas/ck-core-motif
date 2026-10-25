---
nav_exclude: true
---
# Form child-count bounds

`XmForm` stores its child count as an Xt `Cardinal`. Form layout, geometry
queries, managed-child updates, and dependency sorting now use `Cardinal`
indices when traversing the composite child array. Attachment-type loops
remain signed because they iterate the fixed attachment enum, not the child
array.

This keeps traversal types aligned with the Xt count while retaining the
existing layout and dependency-ordering behavior.
