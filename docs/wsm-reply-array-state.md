---
nav_exclude: true
---

# WSM reply array state

Reply packing now rejects positive counts paired with null arrays for
`GET_STATE`, `REG_WINDOW`, and WM-window entry replies.  This prevents null
traversal during size calculation while retaining valid zero-count replies and
the existing wire formats.
