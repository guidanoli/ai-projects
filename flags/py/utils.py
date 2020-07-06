'''
Utility functions and classes
=============================

Contains preprocessed data and handy-dandy functions and classes
'''

import os
import random
import instance
import matplotlib.pyplot as plt
import numpy as np

class Algorithm:
    '''
    A Machine Learning algorithm learns with a training data
    and tries to predict the output of new unknown input
    '''

    def fit(self, training_data : np.ndarray) -> None:
        '''
        Fit the model to the training data
        
        Argument:
            training_data - array of instance values
                            training_data[:,i] = values of the i-th instance
                            training_data[i,:] = i-th value of all instances
        '''
        pass
    
    def predict(self, testing_data : np.ndarray) -> np.ndarray:
        '''
        Try to predict the output for a given testing data
        
        Argument:
            testing_input - array of instance attributes
                            training_data[:,i] = attributes of the i-th instance
                            training_data[i,:] = i-th attribute of all instances
        
        Return:
            predicted values
        '''
        pass

class ConfusionMatrix:
    '''
    Relates exptected and predicted outputs in a
    square matrix
    '''

    def __init__(self, n : int):
        self.n = n
        self.m = np.zeros(shape=(n, n), dtype=np.int32)
    
    def update(self, expected : np.array, predicted : np.array) -> None:
        '''
        Updates confusion matrix
        1. expected and predicted must have the same size
        2. values in both containers must be contained
           in the [0,n) interval, and must be integers
       
       Arguments:
            expected - expected output
            predicted - predicted output
        '''
        size = len(expected)
        h = np.zeros(shape=(size,self.n,self.n), dtype=np.int32)
        h[np.arange(size), expected, predicted] += 1
        hsum = np.sum(h, axis=0)
        self.m += hsum
    
    def get_matrix(self) -> np.matrix:
        return self.m
    
    def get_accuracy(self) -> float:
        return float(np.sum(np.diagonal(self.m))) / np.sum(self.m)
    
    def plot(self, names) -> None:
        fig = plt.figure()
        ax = fig.add_subplot(111)
        cax = ax.matshow(self.m, cmap='Greens')
        fig.colorbar(cax)
        ax.set_xticklabels(['']+list(map(lambda name: name[:3] + '.',names)))
        ax.set_yticklabels(['']+names)
        plt.show()

def k_fold(k : int, alg : Algorithm) -> tuple:
    '''
    K-fold cross-validation algorithm
    where k is the number of chunks into which the data will be divided
    '''
    confm = ConfusionMatrix(_max_label_value + 1)
    chunks = np.array_split(_matrix, k, axis=1)
    for i in range(k):
        testing = chunks[i]
        training = np.concatenate(chunks[:i] + chunks[i+1:], axis=1)
        alg.fit(training)
        predicted = alg.predict(testing[_attr_map])
        expected = testing[_label_map][0,:]
        confm.update(expected, predicted)
    return confm

def i2x(i : instance.Instance) -> np.ndarray:
    '''
    Get attributes from instance
    '''
    return i.load_attr_values()[_attr_map]

def i2y(i : instance.Instance) -> int:
    '''
    Get label from instance
    '''
    return i.load_attr_values()[_label_map]

def get_int_attr_map(i : instance.Instance) -> np.ndarray:
    '''
    Get numpy boolean array that maps integer attributes
    '''
    is_int_name = (lambda l: type(getattr(i, l)) is int)
    return np.fromiter(map(is_int_name, _attr_names), dtype=bool)

def get_max_attr_values(m : np.ndarray) -> np.ndarray:
    '''
    Get maximum values for attributes
    '''
    return np.max(_matrix[_attr_map], axis=1)

def get_max_label_value(il : list) -> int:
    '''
    Get maximum value for label
    '''
    return np.max(_matrix[_label_map])

def il2m(il : list) -> np.ndarray:
    '''
    Transform instance list to matrix
    '''
    m = np.zeros(shape=(len(_names), len(il)), dtype=np.int32)
    for index, data in enumerate(il):
        m[:, index] = data.load_attr_values()
    return m

def euclidean_distance(a, b) -> float:
    '''
    Euclidean distance
    sum(|a_i - b_i|**2) [[int, normalized]]
    sum(|a_i == b_i ? 0 : 1|) [[bool, enum]]
    '''
    dab = a - b
    dab_int_norm = np.apply_along_axis(np.divide, 0,
                                       dab[_int_attr_map],
                                       _max_int_attr_values)
    dab_nint_sum = np.sum(dab[~_int_attr_map] != 0, axis=0)
    return np.linalg.norm(dab_int_norm, axis=0) ** 2 + dab_nint_sum

def hamming_distance(a, b) -> int:
    '''
    Hamming distance
    sum(|a_i == b_i ? 0 : 1|)
    '''
    return np.sum((a - b) != 0, axis=0)

def get_labels(arr : np.ndarray) -> np.ndarray:
    '''
    Returns array of labels
    '''
    return arr[_label_map][0]

def gini_impurity(label_occurrences : dict) -> float:
    '''
    Returns gini impurity of the set discribed by label_occurrences
    '''
    total = sum(label_occurrences.values())
    gini = 0
    for label in label_occurrences:
        gini+= (label_occurrences.get(label,0)/total)**2
    return 1-gini

def entropy(label_occurrences : dict) -> float:
    '''
    Returns entropy of the set discribed by label_occurrences
    '''
    total = sum(label_occurrences.values())
    entropy = 0
    for label in label_occurrences:
        frac = (label_occurrences.get(label,0)/total)
        if(frac != 0):
            entropy+= frac* np.log2(frac)
        
    return -entropy


def get_command_line_arguments():
    '''
    Parses command line arguments
    as containing two types of arguments
    which are interpreted as the following
    table shows.
    
    CMD line        dictionary (return)
    -----------------------------------
    --key=value     'key' : 'value'
    --boolean       'boolean' : True
    
    The second column represents the dictionary
    entries created from the command line arguments.
    '''
    from sys import argv
    args = dict()
    for arg in argv:
        if '--' == arg[:2]:
            arg = arg[2:]
            if '=' in arg:
                k, v = arg.split('=')
                args[k] = v
            else:
                args[arg] = True
    return args

def get_unused_attrs():
    fpath = 'unused_attrs.txt'
    if not os.path.isfile(fpath):
        return []
    with open(fpath, 'r') as f:
        text = f.read()
        return text.split('\n')

## Preprocessed data

# Instance list
_data = instance.parse()

# Instance attribute names
_names = instance.load_attr_names()

# Instance matrix
_matrix = il2m(_data)

# Label name
_label_name = 'religion'

# Label mapping
_label_map = _names == _label_name

# Unused attribute labels
_unused_attr_labels = get_unused_attrs()

# Unused attribute mapping
_unused_attr_map = np.fromiter((x in _unused_attr_labels
                                for x in _names), dtype=bool)

# Attribute mapping
_attr_map = ~_label_map & ~_unused_attr_map

# Attribute names
_attr_names = _names[_attr_map]

# Integer attribute names
_int_attr_map = get_int_attr_map(_data[0])

# Maximum attribute values
_max_attr_values = get_max_attr_values(_matrix)

# Maximum label value
_max_label_value = get_max_label_value(_matrix)

# Maximum integer attribute values
_max_int_attr_values = _max_attr_values[_int_attr_map]