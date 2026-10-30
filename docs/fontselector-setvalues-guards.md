---
nav_exclude: true
---
# FontSelector SetValues guards

Resource updates now tolerate partial child construction. Toggle state and
text-row updates require their target widgets, encoding-menu rebuilds require
an existing option menu, and label refreshes use a shared null-safe helper.
Normal, fully constructed FontSelector updates retain their existing behavior.
