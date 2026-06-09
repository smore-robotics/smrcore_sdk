#!/usr/bin/env python3
"""basics/linked_sdk - report the Python package and linked C++ SDK versions.

Usage:
    python examples_py/basics/linked_sdk.py

This does not connect to a robot. Use it to confirm the wheel is installed and
which native SDK build it is linked against before filing a bug report.
"""

import sys

import rcore_sdk
from rcore_sdk import _native


def main():
    print(f"rcore_sdk package version: {rcore_sdk.__version__}")

    info = _native.linked_sdk()
    # linked_sdk() returns a dict describing the bundled native SDK.
    for key in sorted(info):
        print(f"  {key}: {info[key]}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
