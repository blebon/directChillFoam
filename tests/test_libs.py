#!/usr/bin/env python
"""Tests for shared libraries."""

from ctypes import cdll

def test_fvModels():
    """Test that `libmyFvModels.so` can be loaded."""
    cdll.LoadLibrary("libmyFvModels.so")

def test_fvConstraints():
    """Test that `libmyFvConstraints.so` can be loaded."""
    cdll.LoadLibrary("libmyFvConstraints.so")

def test_ThermophysicalTransportModels():
    """Test that `libmythermophysicalTransportModels.so` can be loaded."""
    cdll.LoadLibrary("libmythermophysicalTransportModels.so")
