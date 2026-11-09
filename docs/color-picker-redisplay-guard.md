---
nav_exclude: true
---
# ColorPicker rendering ownership

ColorPicker now delegates text, focus and buttons to native controls. Its only
custom painting is the passive alpha-aware swatch, clipped to its allocated
bounds. Its cached XImage validates the row stride and checked strip allocation
before writing pixel data. The obsolete Primitive focus rectangle and text GC
have been removed.
Native geometry and rendering are checked by `run-colorpicker-native.py`; see
[ColorPicker](color-picker.md) for the current interaction contract.
