![popsicgl](./docs/assets/popsicgl.WEBP)

# pysicgl

pysicgl is a Python C extension interface for the [sicgl](https://github.com/oclyke/sicgl) graphics library.

both projects are young and would benefit from community involvement.

# getting started as a developer

**get submodules**

```bash
git submodule update --init --recursive
```

**set up the python environment**

* remove any existing virtual environment
* create a new virtual environment
* activate the virtual environment
* install development dependencies

```bash
rm -rf venv
python3 -m venv venv # use your Python 3 interpreter
source venv/bin/activate
pip install -r requirements.dev.txt
```

**build and develop pysicl**

```bash
python setup.py build
python setup.py develop
```

**run tests and install**

```bash
python -m pytest
python setup.py test
python setup.py install
```

# formatting

```
source venv/bin/activate
./scripts/third-party/run-clang-format/run-clang-format.py -r include src
black .
```

# design choices

## color sequences

color sequences are immutable. side effects are not allowed.
