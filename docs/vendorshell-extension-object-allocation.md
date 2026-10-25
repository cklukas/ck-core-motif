# VendorShell extension-object allocation

VendorShell initialization creates a secondary extension object, a request
copy, and an extension-data record before importing resources. The initializer
now checks each allocation and releases any partial objects before returning,
so allocation pressure cannot cause a null dereference or leaked request
storage.
