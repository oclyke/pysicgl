import pytest
import pysicgl


DEFAULT_COLORS = [2]


def test_initialization():
    sequence = pysicgl.ColorSequence(DEFAULT_COLORS)


@pytest.mark.skip(reason="Not implemented")
def test_has_len():
    sequence = pysicgl.ColorSequence(DEFAULT_COLORS)
    assert hasattr(sequence, "__len__")


@pytest.mark.skip(reason="Not implemented")
def test_length():
    sequence = pysicgl.ColorSequence(DEFAULT_COLORS)
    assert len(sequence) == 0

    sequence.colors = ((0, 0, 0, 0),)
    assert len(sequence) == 1

    sequence.colors = ((0, 0, 0, 0), (0, 0, 0, 0))
    assert len(sequence) == 2
