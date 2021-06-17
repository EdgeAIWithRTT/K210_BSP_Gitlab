# for module compiling
import os
from building import *

cwd = GetCurrentDir()
objs = []
list = os.listdir(cwd)

for d in list:
    path = os.path.join(cwd, d)
    if os.path.isfile(os.path.join(path, 'SConscript')):
        objs = objs + SConscript(os.path.join(d, 'SConscript'))

# scr = os.path.join(cwd + '/../..', 'openmv', 'SConscript')
# if (os.path.exists(scr)):
    # objs += SConscript(scr, variant_dir='openmv', duplicate=0)

# scr = os.path.join(cwd + '/../..', 'extmods', 'SConscript')
# if (os.path.exists(scr)):
    # objs += SConscript(scr, variant_dir='extmods', duplicate=0)

Return('objs')
