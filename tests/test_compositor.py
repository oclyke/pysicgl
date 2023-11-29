import pysicgl


def test_builtin_compositors():
    expected_compositors = (
        "DIRECT_SET",
        "DIRECT_CLEAR",
        "DIRECT_NONE",
        "BIT_AND",
        "BIT_OR",
        "BIT_XOR",
        "BIT_NAND",
        "BIT_NOR",
        "BIT_XNOR",
        # # These bitwise compositors are not implemented yet in sicgl.
        # "BIT_NOT_SOURCE",
        # "BIT_NOT_DESTINATION",
        "CHANNEL_MIN",
        "CHANNEL_MAX",
        "CHANNEL_SUM",
        "CHANNEL_DIFF",
        "CHANNEL_DIFF_REVERSE",
        "CHANNEL_MULTIPLY",
        "CHANNEL_DIVIDE",
        "CHANNEL_DIVIDE_REVERSE",
        "CHANNEL_SUM_CLAMPED",
        "CHANNEL_DIFF_CLAMPED",
        "CHANNEL_DIFF_REVERSE_CLAMPED",
        "CHANNEL_MULTIPLY_CLAMPED",
        "CHANNEL_DIVIDE_CLAMPED",
        "CHANNEL_DIVIDE_REVERSE_CLAMPED",
        "ALPHA_CLEAR",
        "ALPHA_COPY",
        "ALPHA_DESTINATION",
        "ALPHA_SOURCE_OVER",
        "ALPHA_DESTINATION_OVER",
        "ALPHA_SOURCE_IN",
        "ALPHA_DESTINATION_IN",
        "ALPHA_SOURCE_OUT",
        "ALPHA_DESTINATION_OUT",
        "ALPHA_SOURCE_ATOP",
        "ALPHA_DESTINATION_ATOP",
        "ALPHA_XOR",
        "ALPHA_LIGHTER",
    )

    for compositor_name in expected_compositors:
        assert hasattr(pysicgl.composition, compositor_name)
        compositor = getattr(pysicgl.composition, compositor_name)
        assert isinstance(compositor, pysicgl.Compositor)
