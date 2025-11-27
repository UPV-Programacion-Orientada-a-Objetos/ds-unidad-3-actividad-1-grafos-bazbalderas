from setuptools import setup, Extension
from Cython.Build import cythonize

extensions = [
    Extension(
        name="neuro_net",
        sources=["cython/neuro_net.pyx", "src/Graph.cpp"],
        include_dirs=["src"],
        language="c++",
        extra_compile_args=["-std=c++11", "-O3"],
    )
]

setup(
    name="neuro_net",
    ext_modules=cythonize(extensions, compiler_directives={'language_level': "3"}),
)
