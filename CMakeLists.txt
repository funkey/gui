# find all files constituting this library
file(GLOB guifiles
  *.cpp
  linux/*.cpp)

# this is a library
add_library(gui ${guifiles})
