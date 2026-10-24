# ColorPicker

`XmColorPicker` is an additive packed-RGBA control.  Its committed value is an
unsigned 32-bit color written as `0xRRGGBBAA`; each channel is exactly eight
bits and no color-space or gamut conversion is implied.  `XmColorPickerPreviewValue`
updates the swatch and emits `XmNcolorPickerPreviewCallback` without committing;
`XmColorPickerSetValue` commits and optionally emits
`XmNcolorPickerValueChangedCallback`.  Escape restores the committed value and
Return commits the preview.  The visible value is always shown as `#RRGGBBAA`.

The control uses the screen colormap for its swatch and falls back to the
foreground pixel if allocation fails; it never substitutes a different
numeric value.  The accessibility provider exposes the packed value, a
0..0xffffffff range and the same hex value text.  The existing
`XmColorSelector` extension and its resource/API behavior are unchanged.
