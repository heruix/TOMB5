if(EMSCRIPTEN)

Message("Compiling for Emscripten!")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2 --emrun -s ASSERTIONS=1 --std=c++11 -s GLOBAL_BASE=1048576 -s USE_WEBGL2=1 -s FULL_ES3=1 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1")
set(SDL2_LIBRARIES "-s USE_SDL=2")

endif()