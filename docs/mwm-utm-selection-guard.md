# MWM UTM selection guard

The UTM drawing area is optional in the startup path, but WM selection setup
also uses it later for conversion callbacks.  `OwnWMSelections` now skips that
selection registration when the drawing area was not created, avoiding a null
callback target while allowing other managed screens to continue.  Normal
UTM-backed selection ownership is unchanged.
