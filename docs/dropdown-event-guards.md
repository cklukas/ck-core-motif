---
nav_exclude: true
---

# DropDown grabbed-event guards

`XmDropDown` receives button events while its popup grab is active. A grab may
surface an event for a foreign window or a window destroyed during dispatch.
The handler now treats an unmapped widget and an incomplete parent chain as a
no-op, preserving normal popup behavior without a null dereference.
