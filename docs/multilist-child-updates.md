---
nav_exclude: true
---
# MultiList optional-child updates

MultiList can remain usable when allocation pressure prevents its optional
find controls or title widget from being created.  Later `SetValues` calls now
manage or unmanage only children that exist, and label changes update the
find/title controls only when their widgets are available.

The normal fully constructed layout and resource behavior is unchanged.  This
keeps deferred configuration safe after partial construction instead of
passing null widget handles to Xt.
