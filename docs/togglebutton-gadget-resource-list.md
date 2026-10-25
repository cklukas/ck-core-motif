# ToggleButtonGadget resource-list allocation

ToggleButtonGadget class initialization merges the LabelGadget and
ToggleButtonGadget resource tables into one cache-object list.  The merged
count and Xt byte size are now checked before allocation, and initialization
returns without writing through a null list when memory is unavailable.

The focused source-level regression is
`tests/automated/check-togglebutton-gadget-resource-list.py`.
