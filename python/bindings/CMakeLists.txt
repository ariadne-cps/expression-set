include_directories(${Python3_INCLUDE_DIRS})
get_target_property(PYTHON_COMMON_SOURCE_DIR pyariadne-python-common SOURCE_DIR)

add_library(pyariadne-expression_set-submodule-obj OBJECT expression_set_submodule.cpp)
target_include_directories(pyariadne-expression_set-submodule-obj PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${PYTHON_COMMON_SOURCE_DIR}/include"
    $<TARGET_PROPERTY:pyariadne-set,INTERFACE_INCLUDE_DIRECTORIES>
)
target_link_libraries(pyariadne-expression_set-submodule-obj PRIVATE pyariadne-python-common pybind11::module expression-set)
set_target_properties(pyariadne-expression_set-submodule-obj PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(pyariadne-expression_set INTERFACE)
target_sources(pyariadne-expression_set INTERFACE $<TARGET_OBJECTS:pyariadne-expression_set-submodule-obj>)
target_include_directories(pyariadne-expression_set INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<TARGET_PROPERTY:pyariadne-set,INTERFACE_INCLUDE_DIRECTORIES>
)
target_link_libraries(pyariadne-expression_set INTERFACE pyariadne-set pyariadne-symbolic)

ariadne_add_python_module(NAME expression_set MODULE_SOURCE expression_set_module.cpp)
