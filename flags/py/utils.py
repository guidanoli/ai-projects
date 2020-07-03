'''
Utility functions and classes
=============================

Contains preprocessed data and handy-dandy functions and classes
'''

import random
import instance
import matplotlib.pyplot as plt
import numpy as np

class Algorithm:
    '''
    A Machine Learning algorithm learns with a training data
    and tries to predict the output of new unknown input
    '''

    def fit(self, training_data : list) -> None:
        '''
        Fit the model to the training data
        
        Argument:
            training_data - list of instances
        '''
        pass
    
    def predict(self, testing_input : instance.Instance) -> int:
        '''
        Try to predict the output for a given testing data
        
        Argument:
            testing_input - input instance
        
        Return:
            predicted value
        '''
        pass

class ConfusionMatrix:
    '''
    Relates exptected and predicted outputs in a
    square matrix
    '''

    def __init__(self, n : int):
        self.m = np.zeros(shape=(n, n), dtype=np.int32)
    
    def update(self, expected : list, predicted : list) -> None:
        '''
        Updates confusion matrix
        1. expected and predicted must have the same size
        2. values in both containers must be contained
           in the [0,n) interval, and must be integers
       
       Arguments:
            expected - expected output
            predicted - predicted output
        '''
        for exp, pred in zip(expected, predicted):
            self.m[exp][pred] += 1
    
    def get_matrix(self) -> np.matrix:
        return self.m
    
    def get_accuracy(self) -> float:
        return float(np.sum(np.diagonal(self.m))) / np.sum(self.m)
    
    def plot(self, labels) -> None:
        fig = plt.figure()
        ax = fig.add_subplot(111)
        cax = ax.matshow(self.m, cmap='Greens')
        fig.colorbar(cax)
        ax.set_xticklabels(['']+list(map(lambda name: name[:3] + '.',labels)))
        ax.set_yticklabels(['']+labels)
        plt.show()

def split(l : list, k : int) -> list:
    '''
    Split list into k equally sized chunks (list of lists)
    The list is first shuffled to avoid positional bias.
    '''
    shuffled_l = l[:]
    random.shuffle(shuffled_l)
    avg = len(l) / float(k)
    out = []
    last = 0.0
    while last < len(l):
        out.append(l[int(last):int(last+avg)])
        last += avg
    return out

def k_fold(data : list, k : int, alg : Algorithm, get_expected, max_expected_value : int) -> tuple:
    '''
    K-fold cross-validation algorithm
    where k is the number of chunks into which the data will be divided
    '''
    confm = ConfusionMatrix(max_expected_value + 1)
    chunks = split(data, k)
    for i in range(k):
        testing = chunks[i]
        training = chunks[:i] + chunks[i+1:]
        training = sum(training, [])
        alg.fit(training)
        predicted = list(map(alg.predict, testing))
        expected = get_expected(testing)
        confm.update(expected, predicted)
    return confm

def i2x(i : instance.Instance) -> np.ndarray:
    '''
    Transforms an instance into an array of input attributes
    '''
    return i.load_attr_values()[_attr_map]

def i2y(i : instance.Instance) -> int:
    '''
    Transforms an instance into the attribute to be predicted
    '''
    return i.load_attr_values()[_not_attr_map][0]

def get_int_attr_map(i : instance.Instance) -> np.ndarray:
    '''
    Get numpy boolean array that maps integer attributes
    '''
    is_int_label = (lambda l: type(getattr(i, l)) is int)
    return np.fromiter(map(is_int_label, _attr_labels), dtype=bool)

def get_max_int_attr_values(il : list) -> np.ndarray:
    '''
    Get maximum values for instance list
    '''
    max_values = i2x(il[0])[_int_attr_map]
    for i in il:
        x = i2x(i)[_int_attr_map]
        np.maximum(max_values, x, max_values)
    return max_values

def euclidean_distance(a, b) -> float:
    '''
    Euclidean distance
    sum(|a_i - b_i|**2) [[int, normalized]]
    sum(|a_i == b_i ? 0 : 1|) [[bool, enum]]
    '''
    dab = a - b
    return (np.linalg.norm(dab[_int_attr_map] / _max_int_attr_values) ** 2 +
            np.sum(dab[~_int_attr_map] != 0))

def hamming_distance(a, b) -> int:
    '''
    Hamming distance
    sum(|a_i == b_i ? 0 : 1|)
    '''
    dab = a - b
    return np.sum(dab != 0)

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

## Preprocessed data

# Instance list
_data = instance.parse()

# Instance attribute labels
_labels = instance.load_attr_labels()

# Attribute mapping
_attr_map = _labels != 'religion'

# Not-Attribute mapping
_not_attr_map = ~_attr_map

# Attribute labels
_attr_labels = _labels[_attr_map]

# Integer attribute labels
_int_attr_map = get_int_attr_map(_data[0])

# Maximum integer attribute values
_max_int_attr_values = get_max_int_attr_values(_data)
