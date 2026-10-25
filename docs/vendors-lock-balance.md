# VendorS secondary-object lock balance

VendorS secondary-object initialization now reads the class size under one
process lock, releases it, and only then performs allocations. The previous
double acquisition and mismatched release could deadlock on nonrecursive
locks before a vendor shell was initialized.
