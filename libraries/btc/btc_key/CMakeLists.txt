set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSTATICLIB")

aux_source_directory(. SRC_LIST)

file(GLOB HEADERS "*.h")

add_library(btc_key ${SRC_LIST} ${HEADERS})

find_package(OpenSSL REQUIRED)

target_include_directories(btc_key
  PUBLIC ${OPENSSL_INCLUDE_DIR}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  )

target_link_libraries(btc_key ${OPENSSL_LIBRARIES} ${CMAKE_DL_LIBS})


install(TARGETS btc_key RUNTIME DESTINATION bin ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
