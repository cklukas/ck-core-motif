---
nav_exclude: true
---
# Accessibility text allocation

The accessibility text provider already rejects unrepresentable substring
sizes before converting them to the Motif text APIs.  It now also checks the
bounded temporary `XtMalloc` before passing the buffer to
`XmTextGetSubstring` or `XmTextFieldGetSubstring`.

If the temporary buffer cannot be allocated, the provider returns no text
without dereferencing a null buffer or disturbing the widget and accessibility
model state.
