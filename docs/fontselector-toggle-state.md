---
nav_exclude: true
---
# FontSelector toggle callback state

`XmFontSelector` now normalizes the integer `set` member delivered by toggle
callbacks to Motif's `Boolean` representation before storing it in widget
resources or passing it to flag helpers. This preserves the public callback
contract while avoiding implementation-defined narrowing when callback state is
carried through Boolean state fields. The encoding matcher also makes its
non-positive capacity rejection an unambiguous standalone branch.

The focused source contract is covered by
`tests/automated/check-fontselector-toggle-state.py`.
