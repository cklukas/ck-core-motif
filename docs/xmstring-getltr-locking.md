# XmStringGetLtoR locking and initialization

`XmStringGetLtoR` now delegates synchronization to the public context helpers
it calls instead of holding the process lock across them. It also checks
`XmStringInitContext` before enumeration, so context allocation failure returns
`False` cleanly rather than entering iteration with a null context.
