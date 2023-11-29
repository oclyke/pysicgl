from setuptools import setup, Extension, find_packages
from pathlib import Path, PurePath

# source files for sicgl
sicgl_root_dir = "third-party/sicgl"
sicgl_include_dirs = list(
    str(PurePath(sicgl_root_dir, include))
    for include in [
        "include",
    ]
)
sicgl_sources = list(
    str(PurePath(sicgl_root_dir, "src", source))
    for source in [
        "compositors/alpha.c",
        "compositors/bitwise.c",
        "compositors/channelwise.c",
        "compositors/direct.c",
        "domain/global.c",
        "domain/interface.c",
        "domain/screen.c",
        "private/direct.c",
        "private/interpolation.c",
        "blend.c",
        "blenders.c",
        "blit.c",
        "color_sequence.c",
        "compose.c",
        "field.c",
        "gamma.c",
        "interface.c",
        "iter.c",
        "screen.c",
        "translate.c",
        "unity_color.c",
    ]
)

pysicgl_root_dir = "."
pysicgl_include_dirs = list(
    str(PurePath(pysicgl_root_dir, include))
    for include in [
        "include",
    ]
)
pysicgl_sources = list(
    str(PurePath(pysicgl_root_dir, "src", source))
    for source in [
        "submodules/composition/module.c",
        "submodules/functional/drawing/global.c",
        "submodules/functional/drawing/interface.c",
        "submodules/functional/drawing/screen.c",
        "submodules/functional/color.c",
        "submodules/functional/color_correction.c",
        "submodules/functional/module.c",
        "submodules/functional/operations.c",
        "submodules/interpolation/module.c",
        "types/color_sequence/type.c",
        "types/color_sequence_interpolator/type.c",
        "types/compositor/type.c",
        "types/scalar_field/type.c",
        "types/interface/type.c",
        "types/screen/type.c",
        "module.c",
    ]
)

sicgl_core = Extension(
    "pysicgl._core",
    include_dirs=[*pysicgl_include_dirs, *sicgl_include_dirs],
    sources=[*pysicgl_sources, *sicgl_sources],
    extra_compile_args=[
      # "-Werror",
      # "-Wall", "-Wextra", "-pedantic",
      # "-Wno-missing-field-initializers", "-Wno-sign-compare", "-Wno-sometimes-uninitialized",
    ],
)

setup(
    ext_modules=[sicgl_core],
    packages=find_packages(where="packages"),
    package_dir={'': 'packages'},
    setup_requires=["setuptools_scm"],
)
