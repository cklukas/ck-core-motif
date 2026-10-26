---
nav_exclude: true
---
# Scrolled-window auto-drag allocation

Auto-drag support in `XmScrolledWindow` keeps rectangles used by scrollbar
drop callbacks.  The rectangle state is now checked when it is allocated and
before callbacks or geometry updates dereference it.  If the state cannot be
allocated, auto-drag is disabled for that instance.  Rectangle-provider
failures and empty results are also ignored safely, including provider methods
implemented by subclasses.
