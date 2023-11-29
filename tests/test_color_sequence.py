import pytest
import pysicgl


DEFAULT_COLORS = [2]
DEFAULT_INTERPOLATOR = pysicgl.interpolation.CONTINUOUS_CIRCULAR


def test_initialization():
    sequence = pysicgl.ColorSequence(
        colors=DEFAULT_COLORS, interpolator=DEFAULT_INTERPOLATOR
    )


@pytest.mark.skip(reason="Not implemented")
def test_has_len():
    sequence = pysicgl.ColorSequence(
        colors=DEFAULT_COLORS, interpolator=DEFAULT_INTERPOLATOR
    )
    assert hasattr(sequence, "__len__")


def test_len():
    sequence = pysicgl.ColorSequence(
        colors=DEFAULT_COLORS, interpolator=DEFAULT_INTERPOLATOR
    )
    assert len(sequence) == len(DEFAULT_COLORS)


def test_subscr():
    sequence = pysicgl.ColorSequence(
        colors=DEFAULT_COLORS, interpolator=DEFAULT_INTERPOLATOR
    )
    for idx in range(len(DEFAULT_COLORS)):
        assert sequence[idx] == DEFAULT_COLORS[idx]


@pytest.mark.skip(reason="Not implemented")
def test_length():
    sequence = pysicgl.ColorSequence(
        colors=DEFAULT_COLORS, interpolator=DEFAULT_INTERPOLATOR
    )
    assert len(sequence) == 0

    sequence.colors = ((0, 0, 0, 0),)
    assert len(sequence) == 1

    sequence.colors = ((0, 0, 0, 0), (0, 0, 0, 0))
    assert len(sequence) == 2


def test_iterator():
    sequence = pysicgl.ColorSequence(
        colors=DEFAULT_COLORS, interpolator=DEFAULT_INTERPOLATOR
    )
    assert hasattr(sequence, "__iter__")
    assert iter(sequence) is sequence
    assert len(list(sequence)) == len(DEFAULT_COLORS)

    for color in sequence:
        assert color == DEFAULT_COLORS[0]
