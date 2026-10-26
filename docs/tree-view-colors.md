# TreeView color changes

TreeView updates its private drawing context whenever `XmNforeground` or
`XmNbackground` changes. Text, disclosure marks and selection outlines therefore
follow the widget's current resources, including appearance defaults applied
after initialization and live theme changes. Explicit application palette
choices continue to use the ordinary resource-ownership rules.

Previously the context retained its initialization colors. In particular, a dark
background could be combined with the original black text even though querying
`XmNforeground` returned the correct light color.

Run `python3 tests/automated/run-tree-view-colors.py --prefix PREFIX --out NEW_DIR`
on Linux with Xvfb. The native regression checks the drawing context and rendered
pixels for four initial profiles, live profile changes and explicit palettes.
It passed with both Xft/SVG and core-font/non-SVG ABI 9 development builds.
The actual gallery window was also inspected with a verified executable and
loaded library; dark navigation labels, disclosure marks and selection outlines
use the light foreground. This fix does not claim a complete TreeView redesign.
