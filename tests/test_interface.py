import pytest
import pysicgl


@pytest.fixture
def interface():
    WIDTH = 1
    HEIGHT = 1
    display_screen = pysicgl.Screen((WIDTH, HEIGHT))
    display_memory = pysicgl.allocate_pixel_memory(display_screen.pixels)
    _interface = pysicgl.Interface(display_screen, display_memory)
    return _interface
