---
nav_exclude: true
---
# MultiList selection after partial construction

The public MultiList selection helpers now validate both the outer widget and
the internal I18List child.  If list creation failed, selection, visibility,
and deselection requests return without passing a null child to the I18List
implementation.  Row-array queries also return an empty result and reset the
reported count.

Fully constructed MultiLists retain their existing selection and callback
behavior; this only defines the failure behavior for partial construction.
