---
nav_exclude: true
---
# DropDown child getters

The public `XmDropDownGetLabel`, `XmDropDownGetArrow`, `XmDropDownGetText`, and
`XmDropDownGetList` helpers return the control's actual internal child widgets.
They first reject a null or non-DropDown widget and return `NULL` when the
requested child has not been created.

The list getter uses the stored child pointer rather than looking up a child by
the default resource name.  Applications may therefore rename the list child
without making the public getter lose it.  Existing fully constructed controls
keep the same child identity and behavior.
