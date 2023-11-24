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
        "drawing/blit.c",
        "drawing/compose.c",
        "drawing/field.c",
        "drawing/interface.c",
        "drawing/screen.c",
        "drawing/global.c",
        "submodules/color/module.c",
        "submodules/composition/module.c",
        "submodules/functional/module.c",
        "color_sequence.c",
        "compositor.c",
        "field.c",
        "interface.c",
        "module.c",
        "screen.c",
        "utilities.c",
    ]
)

sicgl_core = Extension(
    "pysicgl._sicgl_core",
    include_dirs=[*pysicgl_include_dirs, *sicgl_include_dirs],
    sources=[*pysicgl_sources, *sicgl_sources],
)

setup(
    ext_modules=[sicgl_core],
    packages=find_packages(where="packages"),
    package_dir={'': 'packages'},
    setup_requires=["setuptools_scm"],
)
