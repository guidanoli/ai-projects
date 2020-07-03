'''
K Nearest Neighbours
====================

Usage: <python> knn.py
       [--dist=<str>]
       [--kmin=<int>]
       [--kmax=<int>]
       [--kfoldk=<int>]

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
    
    def fit(self, training_data : list) -> None:
        self.data = training_data
    
    def predict(self, testing_input) -> int:
        testing_input_x = utils.i2x(testing_input)
        distance_from_input = lambda i: self.distance(utils.i2x(i), testing_input_x)
        distances = np.fromiter(
            map(distance_from_input, self.data),
            dtype=float)
        kclosest = list(
            map(lambda i: self.data[i],
                np.argpartition(distances, self.k)[:self.k]))
        voting = {int(k): 0 for k in instance.Religion.__members__.values()}
        for i in kclosest:
            voting[int(i.religion)] += 1
        return max(voting.items(), key=operator.itemgetter(1))[0]

def get_expected_values(il : list) -> list:
    return list(map(lambda i: int(i.religion), il))

def get_max_expected_value():
    return max(list(map(lambda x: int(x),
        instance.Religion.__members__.values())))

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

def plot_accuracy_versus_k(results):
    accuracies = np.fromiter(map(lambda r: r.get_accuracy(), results), dtype=float)
    ks = np.fromiter(map(lambda r: r.get_k(), results), dtype=int)
    acc_max = np.max(accuracies)
    k_acc_max, = np.where(accuracies == acc_max)
    mask = accuracies == acc_max
    print("maximum accurracy = {:.2f}% when k = {}".format(acc_max * 100, ks[k_acc_max[0]]))
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

if __name__ == '__main__':
    args = utils.get_command_line_arguments()
    results = list()
    for k in range(int(args.get('kmin', '3')), int(args.get('kmax', '50')) + 1):
        confm = utils.k_fold(utils._data, int(args.get('kfoldk', '10')), KNN(k, **args),
            get_expected_values, get_max_expected_value())
        results.append(KNNResult(k, confm))
    plot_accuracy_versus_k(results)
