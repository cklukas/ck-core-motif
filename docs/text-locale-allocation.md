---
nav_exclude: true
---
# Locale text aggregation allocation

`_XmTextToLocaleText` joins the strings returned by Xlib's text-property
conversion. The aggregate length now uses `size_t` with overflow checks,
rejects unexpected null entries, and checks the final allocation before
writing. Every failure releases Xlib's string list and reports conversion
failure through the existing `success` output.

Successful locale conversion retains the same concatenated text and ownership
contract.
