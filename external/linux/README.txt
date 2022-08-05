SDL2: 

Build with cmake --build -DSDL_SHARED=ON -DSDL_STATIC_PIC=ON . 
Then do cmake --install .

This will build a shared/dynamic library and build it with Position
Independent Code (gcc parameter: -fPIC). Without it, we will get linker
errors.

crossguid:

Requires libuuid (libuuid-dev) to be linked as well, otherwise we'll get an
unreferenced symbol error at link time when trying to use crossguid.


