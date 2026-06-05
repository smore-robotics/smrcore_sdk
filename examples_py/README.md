# Python Examples

Python examples will be added later and will mirror the C++ examples:

| Example | C++ counterpart |
|---------|-----------------|
| `01_connect.py` | `01_connect.cpp` |
| `02_read_state.py` | `02_read_state.cpp` |
| `03_movej.py` | `03_movej.cpp` |
| `04_movel.py` | `04_movel.cpp` |

For now, download the Python wheel from the GitHub Release and verify the
runtime SDK version:

```bash
VERSION=0.0.1
PY_TAG=cp310-cp310-linux_x86_64
curl -L --fail \
  "https://github.com/smore-robotics/smrore_sdk/releases/download/v${VERSION}/rcore_sdk_py-${VERSION}-${PY_TAG}.whl" \
  -o rcore_sdk_py-${VERSION}-${PY_TAG}.whl
pip install rcore_sdk_py-${VERSION}-${PY_TAG}.whl
python -c "import rcore_sdk; from rcore_sdk import _native; print(_native.linked_sdk())"
```
