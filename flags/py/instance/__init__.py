###############################################################################
## This is a plain proxy script that tries to find the Python module         ##
## somewhere in the root directory and includes it                           ##
###############################################################################

from importlib.machinery import ExtensionFileLoader
from pathlib import Path

import glob
import re
import os
import sys

def __get_flags_root_dir():
    file_path = Path(__file__)
    file_path.resolve()
    return str(file_path.parents[2])

def __search_module(folder):
    extension = (sys.platform == 'win32' and '.pyd' or '.so')
    name = get_pyd_name() + extension
    
    found = sorted(Path(folder).rglob(name))
    
    if found:
        return str(found[0])
    else:
        return None

def get_pyd_path():
    root = __get_flags_root_dir()
    module = __search_module(root)
    if not module:
        raise ImportError('Módulo Python não encontrado em ' + root)
    return module

def get_pyd_name():
    return 'instancebindlib'

del sys.modules[__name__]
sys.modules[__name__] = ExtensionFileLoader(get_pyd_name(), get_pyd_path()).load_module()