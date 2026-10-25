# MWM icon-box hierarchy creation

The icon box is built as a shell, frame, scrolled window, and bulletin board.
Each Xt creation call can fail, for example when a display-side allocation
fails.  The construction helpers now report success, and `MakeIconBox` stops
before realizing or linking a partial hierarchy.  Any created shell and
associated icon-box allocations are released on that path.

`RealizeIconBox` also validates the required widgets and the scrolled-window
clip window before using them.  Existing successful creation and widget names
are unchanged, so applications using MWM retain the same hierarchy and
resources while failure handling is deterministic.
