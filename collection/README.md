# Collection

Native extension for CPython 3.6. This implemented a new data storage for dictionaries where keys and values are stored separately and shapes are reused across collection with the same layout (like js engines represent objects)

The bug is that when the extension checks if the list of keys has already been encountered it does a list equivalency check when it should do strict equality

As such `{a:1, b:1}` and `{b:1, a:1}` will be thought as having the same shape

We can use this to cause a type confusion between an integer and a `PyObject*` and get an object handle at an arbitrary address

my exploit fakes an array object (module array) and use the confusion to get a handle to it, we can therefore create arrays with arbitrary pointer and use them to read and write data. From there I read into the GOT to get the libc base address, read `program_invocation_name` to leak a stack address and read the stack backwards to find a known return address to write a ROP chain

The rop chain does a `readv(1023, ...)` followed by a `write` to get the flag
