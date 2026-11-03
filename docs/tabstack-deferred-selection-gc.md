---
nav_exclude: true
---
# TabStack deferred selection rendering

`XmTabStackSelectTab` records a selected page while the TabStack is unrealized
and restores it when the widget gains its X window.  The restore can repaint a
page with an explicit background.  TabStack now creates its private drawing GC
before that restore runs.

If the X server cannot allocate that GC, the selection and its public callback
still complete; only the optional immediate background repaint is deferred to a
later normal expose.  This prevents null-GC drawing requests without changing
the public TabStack API or selection semantics.
