###############################################################################
##                                                                           ##
##  Decision Tree                                                            ##
##                                                                           ##
###############################################################################

import instance
import analysis
import random
from sklearn import tree

data = instance.parse()

def get_data_labels():
    not_labels = ['name', 'religion']
    all_labels = analysis.get_fields()
    return [x for x in all_labels if x not in not_labels]

data_labels = get_data_labels()

def get_result_label():
    return 'religion'

def get_result_name(r : int):
    return instance.Religion(r).name

def instance2data(i):
    return [int(getattr(i, a)) for a in data_labels]

def instance2result(i):
    return int(getattr(i, get_result_label()))

def il2x(il):
    return list(map(instance2data, il))

def il2y(il):
    return list(map(instance2result, il))

def il2xy(il):
    return (il2x(il), il2y(il))

def fit_dataset(X, Y):
    clf = tree.DecisionTreeClassifier()
    clf = clf.fit(X, Y)
    return clf

def split(l : list, k : int):
    shuffled_l = l[:]
    random.shuffle(shuffled_l)
    avg = len(l) / float(k)
    out = []
    last = 0.0
    
    while last < len(l):
        out.append(l[int(last):int(last+avg)])
        last += avg

    return out

def accuracy(expected, obtained):
    return sum(1 for x,y in zip(expected, obtained)
               if x == y) / len(expected)

def accuracy_per_result(expected, obtained):
    result_set = set(expected)
    return {result : sum(1 for x,y in zip(expected, obtained)
                if x == y and x == result) /
                len([x for x in expected if x == result])
                    for result in result_set}

def test(training, testing):
    clf = fit_dataset(*il2xy(training))
    Yt = clf.predict(il2x(testing))
    Y = il2y(testing)
    return accuracy(Y, Yt), accuracy_per_result(Y, Yt)

def mean_list(l : list):
    return sum(l) / len(l)

def mean_dict(d : dict, k : int):
    return {key : value/k for key, value in d.items()}

def k_fold(k : int):
    chunks = split(data, k)
    acc_list = list()
    acc_per_result_dict = dict()
    for i in range(k):
        training = chunks[:i] + chunks[i+1:]
        training = sum(training, [])
        testing = chunks[i]
        acc, acc_per_result = test(training, testing)
        acc_list.append(acc)
        for key, value in acc_per_result.items():
            if key in acc_per_result_dict:
                acc_per_result_dict[key] += value
            else:
                acc_per_result_dict[key] = value
    return mean_list(acc_list), mean_dict(acc_per_result_dict, k)

if __name__ == '__main__':
    import sys
    k = 10
    for arg in sys.argv:
        if '=' in arg:
            key, val = arg.split('=')
            if key == 'k':
                k = int(val)
    acc, acc_per_result = k_fold(k)
    print("acc = {:.2f}%".format(acc * 100))
    for key, value in acc_per_result.items():
        print("acc @ {} = {:.2f}%".format(get_result_name(key),
                                          value * 100))
