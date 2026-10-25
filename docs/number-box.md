---
nav_exclude: true
---
# NumberBox

`XmNumberBox` is an additive bounded integer entry and spin control. Its
numeric contract is deliberately explicit: values are signed integers,
`XmNnumberMinimum`/`XmNnumberMaximum` define an inclusive range,
`XmNnumberStep` and `XmNnumberPageStep` define keyboard and pointer increments,
and `XmNnumberWrap` controls boundary wrapping. Existing `XmTextField` and
`XmSpinBox` resources and records are unchanged.

The visible text is a tentative ASCII decimal edit. Digits, a leading minus,
backspace/delete, Return, Escape, Up/Down and PageUp/PageDown are handled by
the additive control. A complete edit is committed only when it parses and is
inside the configured range. Invalid text remains visible, sets
`XmNumberBoxIsInvalid`, and invokes `XmNnumberInvalidCallback`; it is never
silently converted to another value. Programmatic setters normalize and
format the committed value.

`XmNnumberValueChangedCallback` receives normalized range, value and step
information. Explicit setters and stepping accept a `notify` flag. The
control draws compact up/down affordances and honors explicit dimensions,
focus, sensitivity, foreground and high-contrast colors.

The accessibility provider exposes the committed numeric value and inclusive
bounds. The installed Xvfb fixture covers range normalization, stepping,
wrapping, programmatic clamping, focus, explicit sizing and accessibility.
