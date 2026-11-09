# MWM workspace tree query cleanup

`GetPCD` walks parent windows with `XQueryTree`. The server allocates the
returned child list for every successful query, even though this lookup only
needs the parent and root IDs. The function now releases each child list
immediately, including the initial query, preventing one X11 allocation leak
per parent lookup.

`GetParentWindow` uses the same pointer-based cleanup rule rather than assuming
that a zero child count implies no Xlib allocation.
