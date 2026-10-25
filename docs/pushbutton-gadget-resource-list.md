# PushButtonGadget resource-list allocation

PushButtonGadget class initialization merges the LabelGadget and PushButton
resource tables into one cache-object list. The merged count and Xt byte size
are now checked before allocation, and initialization returns without writing
through a null list when memory is unavailable.
