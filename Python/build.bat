@echo off
python setup.py sdist bdist_wheel

::upload to pypi
::twine upload dist/*