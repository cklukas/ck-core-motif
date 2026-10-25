# MWM feedback dialog creation

Confirmation dialogs are created lazily.  `ConfirmAction` now checks the
popup shell, message box, and required OK, Cancel, and Help children before
using them.  A partial dialog is destroyed and the cached widget is cleared
when any required allocation fails.  Message-string comparison also tolerates
missing toolkit strings.  The normal dialog hierarchy and resources are
unchanged.
