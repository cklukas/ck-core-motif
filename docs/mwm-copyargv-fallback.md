# MWM secondary-display argument fallback

WSM opens a second display connection using the copied command-line vector.
When copying either vector fails, `CopyArgv` falls back to the caller-owned
`argv`.  The fallback now also sets `dpy2Argc`, keeping the argument count and
vector consistent for `XtOpenDisplay`.

Successful copies retain the existing duplicated storage and argument count.
