#!/usr/bin/env python
import os
import sys

env = SConscript("../../godot-cpp/SConstruct")

# For reference:
# - CCFLAGS are compilation flags shared between C and C++
# - CFLAGS are for C-specific compilation flags
# - CXXFLAGS are for C++-specific compilation flags
# - CPPFLAGS are for pre-processor flags
# - CPPDEFINES are for pre-processor defines
# - LINKFLAGS are for linking flags

# tweak this if you want to use different folders, or more folders, to store your source code in.
cpp_path = ["src/", "../mathorm/src/", "../fast_syntax/"]

env.Append(CPPPATH=cpp_path)

sources = []
for i in cpp_path:
    sources += Glob(i + "*.cpp")


# Self library
self_lib_name = "entity_body_2d_datalib"
self_lib_path = "bin/" + self_lib_name

def _get_os_lib_path(p_lib_path, p_lib_name, is_os):
    ret = ""

    if is_os == True:
        ret = (p_lib_path + ".{}.{}.framework/" + p_lib_name + ".{}.{}").format(env["platform"], env["target"], env["platform"], env["target"])
    else:
        ret = (p_lib_path + "{}{}").format(env["suffix"], env["SHLIBSUFFIX"])

    return ret

if env["platform"] == "macos":
    library = env.SharedLibrary(_get_os_lib_path(self_lib_path, self_lib_name, True), source=sources, )
else:
    library = env.SharedLibrary(_get_os_lib_path(self_lib_path, self_lib_name, False), source=sources, )

Default(library)