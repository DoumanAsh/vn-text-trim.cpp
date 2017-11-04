###########################
# Main exe
###########################
file(GLOB main_SRC "*.cpp")

add_executable(vn-text-trim ${main_SRC})
target_link_libraries(vn-text-trim ${Boost_LIBRARIES} ${TEXT_LIB} ${CLIPBOARD_LIB})
target_include_directories(vn-text-trim PUBLIC ${LIBS_INCLUDE} ${3PP_INCLUDE})

###########################
# TA DLL
###########################
file(GLOB dll_SRC "dll/*.cpp")
add_library(vn_text_trim SHARED ${dll_SRC})
target_link_libraries(vn_text_trim ${TEXT_LIB})
target_include_directories(vn_text_trim PUBLIC ${LIBS_INCLUDE})
