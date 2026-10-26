# MWM property-processor inputs

The WM protocol, Motif-message, and colormap-window property processors now
return immediately when no `ClientData` record is supplied. This prevents
null dereferences before state cleanup or Xlib property access; normal client
processing is unchanged.

The focused source contract is covered by
`tests/automated/check-mwm-property-processors-inputs.py`.
