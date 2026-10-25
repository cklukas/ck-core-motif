---
nav_exclude: true
---

# WSM nested window-array ownership

`WSM_WM_WINDOWS` request match attributes contain two variable-length property
arrays.  Decoding now allocates and initializes those arrays explicitly,
consumes their wire values on failure, and frees all nested storage during
request cleanup.  Window-entry reply arrays are likewise released by
`FreeReply`, preventing leaks while preserving the protocol layout.
