# Input-method reset input contract

`XmImMbResetIC` writes the reset result through its output pointer and then
resets the widget's input context.  It now returns when either the widget or
output pointer is missing, before app-context lookup or output dereference.
Valid reset operations are unchanged.
