# Modern Font Selector preview and live layout

With Xft enabled, Modern Light and Modern Dark preview the selected font with
antialiased scalable outlines. XLFD choices are resolved against the X server's
local `fonts.dir` entries as well as Fontconfig, so server-only Type 1 families
such as XQuartz Utopia retain their family and style. The selector's canonical
`regular` weight also matches a server face named `medium`, `book` or `normal`.
Unavailable client-side outlines fall back to Fontconfig; without Xft the
existing core X font path remains available. Classic retains core X previews.

The point size is rendered at 96 logical pixels per inch and follows the widget's
effective display scale. The selector owns and releases its preview table; the
rendition owns the Xft font reference. Theme and scale transactions refresh the
preview after all child resources have committed without emitting a user font
change. Public `currentFont` XLFD values and callback payloads are unchanged.

The selector's internal panes permit font-metric negotiation after realization.
Modern DropDown keeps its editor at natural height and aligns its baseline with
the adjacent label within the available row. Classic geometry is unchanged.
SegmentedControl finishes live appearance changes with a layout using the final
child font metrics. Natural width tracks the labels; application-constrained
width and height retain their established behavior.

`tests/automated/run-font-selector-modern.py --build ABS_BUILD --results ABS_NEW_DIR`
compiles a native regression and runs it on an isolated Xvfb display. It requires
Xft, Fontconfig, FreeType headers and a server Utopia outline family (XQuartz
ships it; Linux can provide the scalable X fonts). It checks repeated Classic,
Modern Light and Modern Dark switches at 100%, 150% and 200%, selected family,
point size, all four weight/slant combinations, rendered antialiasing,
preview ownership, quiet callbacks, both editor
baselines and all three segment allocations. Existing segmented geometry and
DropDown negotiation tests additionally cover explicitly constrained geometry.
