import pytest
import pysicgl


def test_module_exists():
    assert hasattr(pysicgl, "functional")


def test_has_gamma_correct():
    assert hasattr(pysicgl.functional, "gamma_correct")


def test_has_get_pixel_at_offset():
    assert hasattr(pysicgl.functional, "get_pixel_at_offset")


def test_has_get_pixel_at_coordinates():
    assert hasattr(pysicgl.functional, "get_pixel_at_coordinates")
