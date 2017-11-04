# Set flags if compiler accepts GCC flags
macro(set_gcc_flags_if)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${ARGV0}")
        set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${ARGV0}")
    endif()
endmacro(set_gcc_flags_if)

# Set MVSC flags
macro(set_msvc_flags_if)
    if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${ARGV0}")
        set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${ARGV0}")
    endif()
endmacro(set_msvc_flags_if)

# Removes default warning to avoid redefenition
macro(remove_msvc_default_warning_level)
    if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        string(REGEX REPLACE "/W[0-4]" "" CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS}")
    endif()
endmacro(remove_msvc_default_warning_level)

# Statically links VC++ runtime
macro(enable_static_crt)
    if(MSVC)
        set(Boost_USE_STATIC_RUNTIME ON)
        foreach(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
            if(${flag_var} MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
            endif(${flag_var} MATCHES "/MD")
            if(${flag_var} MATCHES "/MDd")
                string(REGEX REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
            endif(${flag_var} MATCHES "/MDd")
        endforeach(flag_var)
    endif()
endmacro(enable_static_crt)

# Looks up boost
macro(lazy_find_boost)
    set(Boost_USE_STATIC_LIBS ON)
    find_package(Boost 1.64.0 COMPONENTS ${ARGN})

    if(Boost_FOUND)
        include_directories(${Boost_INCLUDE_DIRS})
        LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
    else()
        message(FATAL_ERROR "Cannot find Boost!")
    endif()
endmacro(lazy_find_boost)

macro(set_output_dir dir)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${dir}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${dir}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${dir}/bin)
endmacro(set_output_dir)
