mkdir -p build && pushd build
# Generate makefiles with CMake.  Pass in the path to your source directory.
cmake ..
# Build.
make -j
popd
# Run echo under DR to see if it works.  If you configured for a debug or 32-bit
# build, your path will be different.  For example, a 32-bit debug build would
# put drrun in ./bin32/ and need -debug flag to run debug build.
#../bin64/drrun -- echo "hello world"
