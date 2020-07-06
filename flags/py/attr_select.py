'''
Attribute Selection
===================

Usage: <python> attr_select.py
       [--alg=<str>]
       [...]

    alg : algorithm
        Name of module (as in 'import alg')
        Name of function (as in alg.alg(...))
        This function is called with all the
        command-line arguments (kwargs) passed
        here and must return, first, the accuracy
        of the algorithm.
        
        Example:
        # alg.py - dummy algorithm with 0% accuracy
        def alg(**kwargs):
            return 0
        
        * knn (Default)
    
    ... : arguments passed to specific argument
'''

import importlib
import numpy as np

# Clear unused attributes
open('unused_attrs.txt', 'w').close()

import utils

all_attrs = utils._names[~utils._label_map]

def set_unused_attrs(_unused_attr_map):
    with open('unused_attrs.txt', 'w') as f:
        for unused_attr in all_attrs[_unused_attr_map]:
            f.write(unused_attr + '\n')
    importlib.reload(utils)

if __name__ == '__main__':
    kwargs = utils.get_command_line_arguments()
    alg = kwargs.get('alg', 'knn')
    m = importlib.import_module(alg)
    f = getattr(m, alg)
    acc_max, *_ = f(**kwargs)
    improved_i = 0
    unused_attrs = np.zeros(len(all_attrs), dtype=bool)
    while True:
        improved = False
        for i in range(len(all_attrs)):
            if unused_attrs[i]:
                continue
            unused_attrs[i] = True
            set_unused_attrs(unused_attrs)
            acc, *_ = f(**kwargs)
            if acc > acc_max:
                print("{:.2f}% @ attrs \\ {}".format(acc_max * 100, all_attrs[unused_attrs]))
                improved = True
                acc_max = acc
                improved_i = i
            unused_attrs[i] = False
        if not improved:
            break
        unused_attrs[improved_i] = True
    print("Done")
    
        
    