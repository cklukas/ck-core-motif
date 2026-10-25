# VendorShell resource-list allocation

VendorShell class initialization merges ShellExt and VendorShellExt resource
tables into one list. The merged count and Xt byte size are now checked before
allocation, and a failed allocation releases the process lock and leaves the
class tables unchanged.
