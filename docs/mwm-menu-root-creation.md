# MWM menu root creation

`CreateMenuWidget` now treats failure to create either the menu shell or its
row-column pane as a failed menu construction.  The recursion marker is
cleared, and a shell created before a pane failure is destroyed.  This avoids
callback registration and menu-item construction against a null widget while
preserving the existing widget classes, names, and resource arguments on the
successful path.
