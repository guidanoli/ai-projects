'''
K Nearest Neighbours
====================

Usage: <python> knn.py
       [--dist=<str>]
       [--kmin=<int>]
       [--kmax=<int>]
       [--kfoldk=<int>]
       [--plot]
       [--supress-io]
       [--time-fitting]
       [--time-predicting]

    dist : distance criterion
        * euclidean
        * hamming (Default)
    
    kmin : minimum value for k for knn
        kmin > 0
        Default = 3
   
    kmax : maximum value for k for knn
        kmax >= kmin
        Default = 50
    
    kfoldk : k value for k-fold
        1 < kfoldk <= 194
        Default = 10
    
    plot : plot accuracy versus k graph
        Default = False
    
    supress-io : supress IO calls
        Default = False
    
    time-fitting : time fitting
        Default = false
    
    time-predicting : time predicting
        Default = false
'''

import instance
import utils
import numpy as np
import matplotlib.pyplot as plt
import operator

class KNN(utils.Algorithm):

    def __init__(self, k, **kwargs):
        self.k = k
        self.distances = {
            'hamming' : utils.hamming_distance,
            'euclidean' : utils.euclidean_distance
        }
        if 'dist' in kwargs:
            if kwargs['dist'] in self.distances:
                self.distance = self.distances[kwargs['dist']]
            else:
                raise Exception("Distance '{}' not supported".format(kwargs['dist']))
        else:
            self.distance = self.distances['hamming']
    
    def fit(self, training_data : np.ndarray) -> None:
        '''
        Simply stores training data internally
        '''
        self.training_attrs = training_data[utils._attr_map]
        self.training_labels = training_data[utils._label_map][0]
    
    def predict(self, testing_attrs : np.ndarray) -> np.ndarray:
        '''
        Predicts the label of testing data by the K nearest
        neighbours in the training dataset (majority)
        '''
        
        #
        # distance matrix (MxN)
        # ---------------------
        # where [i,j] = distance between the i-th testing data
        #               and the j-th training data
        # where M = #testing, N = #training
        #
        distances = self.distance(testing_attrs[:,:,None],
                                  self.training_attrs[:,None,:])
        
        #
        # k closest neighbours (MxK)
        # --------------------------
        # where [i,j] = index of training data that is in the
        #               set of the K nearest neighbours of the
        #               i-th testing data
        # where M = #testing, K = neighbourhood size
        #
        kclosest = np.argpartition(distances, self.k)[:,:self.k]
        
        #
        # k closest neighbours' labels (MxK)
        # ----------------------------------
        # where [i,j] = label of training data that is in the
        #               set of the K nearest neighbours of the
        #               i-th testing data
        # where M = #testing, K = neighbourhood size
        #
        kclosest_labels = self.training_labels[kclosest]
        
        #
        # winning labels (M)
        # ------------------
        # where [i] = label assigned to the i-th testing data
        #             for being the most common label between
        #             its k closest neighbours
        # where M = #testing
        #
        winning_labels = np.zeros(len(kclosest_labels), dtype=np.int32)
        
        for i, v in enumerate(kclosest_labels):
            winning_labels[i] = np.bincount(v).argmax()
        
        return winning_labels

class KNNResult(object):

    def __init__(self, k, confm):
        self.k = k
        self.confm = confm
    
    def get_k(self):
        return self.k
    
    def get_confm(self):
        return self.confm
    
    def get_accuracy(self):
        return self.confm.get_accuracy()

def plot_accuracy_versus_k(accuracies, ks, acc_max=None):
    if acc_max is None:
        acc_max = np.max(accuracies)
    mask = accuracies == acc_max
    color = np.where(mask, 'red', 'blue')
    fig = plt.figure()
    ax = fig.add_subplot(111)
    ax.scatter(ks, accuracies, color=color)
    plt.ticklabel_format(useOffset=False)
    plt.plot(ks, accuracies)
    plt.suptitle('Accuracy of KNN for different values of K')
    plt.ylabel('Accuracy (%)')
    plt.xlabel('K value')
    plt.show()

def knn(*args, **kwargs):
    '''
    Run KNN with different K values and return:
    
        - the maximum accuracy obtained
        - the k for which such accuracy was reached
    '''
    results = list()
    for k in range(int(kwargs.get('kmin', '3')), int(kwargs.get('kmax', '50')) + 1):
        confm = utils.k_fold(int(kwargs.get('kfoldk', '10')),
                             KNN(k, **kwargs),
                             kwargs.get('time-fitting', False),
                             kwargs.get('time-predicting', False))
        results.append(KNNResult(k, confm))
    accuracies = np.fromiter(map(lambda r: r.get_accuracy(), results), dtype=float)
    ks = np.fromiter(map(lambda r: r.get_k(), results), dtype=int)
    acc_max = np.max(accuracies)
    k_acc_max_index, = np.where(accuracies == acc_max)
    mask = accuracies == acc_max
    k_acc_max = ks[k_acc_max_index[0]]
    if 'plot' in kwargs:
        plot_accuracy_versus_k(accuracies, ks, acc_max)
    return acc_max, k_acc_max

if __name__ == '__main__':
    kwargs = utils.get_command_line_arguments()
    acc_max, k_acc_max = knn(**kwargs)
    if 'supress-io' not in kwargs:
        print("{:.2f}% @ k = {}".format(acc_max * 100, k_acc_max))
