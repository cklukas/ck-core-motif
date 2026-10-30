---
nav_exclude: true
---
# FontSelector slant-copy bounds

Bold/italic refresh now copies the selected slant through the existing bounded
XLFD field helper. This preserves the fixed `SLANT_LEN` storage contract,
always terminates the field, and treats an unset family quark as an empty
slant instead of dereferencing a null string.
