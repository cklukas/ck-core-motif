# Prompt text

`XmPrompt` is a standalone, non-interactive presentation for placeholder or
instructional text.  Its copied `XmNpromptText` is never inserted into a text
field, submitted, or treated as user input.  `XmPromptSetVisible` controls the
presentation explicitly; hiding it does not mutate any sibling or host
widget.  Applications place it next to a text field or compose it in their
own layout, so the toolkit never creates an overlay or silently changes host
geometry.

The control is not focusable and has no activation callbacks.  Accessibility
exposes visible prompt text as a label and exposes no name when hidden.  The
installed fixture verifies visibility transitions, sibling value preservation,
accessibility and teardown.
