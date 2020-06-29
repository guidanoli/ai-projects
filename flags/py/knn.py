###############################################################################
##                                                                           ##
##  K Nearest Neighbours                                                     ##
##                                                                           ##
###############################################################################

import instance
import utils
import numpy as np

class KNN(utils.Algorithm):

    def __init__(self, k):
        self.k = k
    
    def fit(self, training_data : list) -> None:
        self.data = training_data
    
    def predict(self, testing_input) -> int:
        import operator
        distance_from_input = lambda i: i.distance_from(testing_input)
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

if __name__ == '__main__':
    data = instance.parse()
    lowest = {'k': 0, 'acc': 0, 'confm': None}
    for k in range(3, 50):
        confm = utils.k_fold(data, 10, KNN(k),
            get_expected_values, get_max_expected_value())
        acc = confm.get_accuracy()
        if acc > lowest['acc']:
            lowest['acc'] = acc
            lowest['k'] = k
            lowest['confm'] = confm
    print("accurracy = {:.2f}% for k = {}".format(lowest['acc'] * 100, lowest['k']))
    lowest['confm'].plot(list(instance.Religion.__members__.keys()))
    
    