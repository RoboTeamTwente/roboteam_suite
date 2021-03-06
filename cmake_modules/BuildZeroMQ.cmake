include(ExternalProject)

ExternalProject_Add(project_zeromq
        BUILD_IN_SOURCE true
        URL https://github.com/zeromq/libzmq/releases/download/v4.3.3/zeromq-4.3.3.tar.gz
        PREFIX "${CMAKE_CURRENT_BINARY_DIR}/libzmq"
        CMAKE_ARGS
        -DENABLE_DRAFTS=ON
        -DBUILD_TESTING=OFF
        -DZMQ_BUILD_FRAMEWORK=OFF
        CONFIGURE_COMMAND ./configure --with-libsodium=${LIBSODIUM_LIB_DIR} --without-docs --prefix=${CMAKE_CURRENT_BINARY_DIR}/libzmq --enable-static --disable-libbsd
        DEPENDS lib::sodium
        )
set(ZEROMQ_INCLUDE ${CMAKE_CURRENT_BINARY_DIR}/libzmq/include)
set(ZEROMQ_LIBRARY_SHARED ${CMAKE_CURRENT_BINARY_DIR}/libzmq/lib/libzmq.so)
set(ZEROMQ_LIBRARY_STATIC ${CMAKE_CURRENT_BINARY_DIR}/libzmq/lib/libzmq.a)

add_library(lib::zmq STATIC IMPORTED)
add_dependencies(lib::zmq project_zeromq)
set_target_properties(lib::zmq PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/libzmq/lib/libzmq.a)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/libzmq/include)
target_link_libraries(lib::zmq
        INTERFACE lib::sodium)
target_include_directories(lib::zmq
        INTERFACE ${LIBSODIUM_INCLUDE_DIR})