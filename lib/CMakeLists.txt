set(LIBS_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}" PARENT_SCOPE)

file(GLOB text_SRC "text/*.cpp")
add_library(text STATIC ${text_SRC})
#target_include_directories(text PUBLIC)
#target_link_libraries(text mecab)
set(TEXT_LIB "text" PARENT_SCOPE)

file(GLOB clip_SRC "clipboard/*.cpp")
add_library(clip STATIC ${clip_SRC})
#target_include_directories(text PUBLIC)
#target_link_libraries(text mecab)
set(CLIPBOARD_LIB "clip" PARENT_SCOPE)
