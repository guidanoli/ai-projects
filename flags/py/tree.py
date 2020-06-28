###############################################################################
##                                                                           ##
##  Decision Tree                                                            ##
##                                                                           ##
###############################################################################

import instance
import analysis
import random
from sklearn import tree

# Flag data
data = instance.parse()

def get_input_names() -> list:
    '''
    Get a list of all Instance class attribute names
    that are considered as input to the ML algorithm.
    '''
    not_labels = ['name', 'religion']
    all_labels = analysis.get_fields()
    return [x for x in all_labels if x not in not_labels]

# Flat data labels
data_labels = get_input_names()

def get_output_name() -> str:
    '''
    Get a Instance class attribute names
    that is considered as output to the ML algorithm.
    '''
    return 'religion'

def get_output_value_label(r : int) -> str:
    '''
    Get output value label.
    '''
    return instance.Religion(r).name

def i2input(i : instance.Instance) -> list:
    '''
    Convert instance to input list.
    '''
    return [int(getattr(i, a)) for a in data_labels]

def i2output(i : instance.Instance) -> int:
    '''
    Convert instance to output value.
    '''
    return int(getattr(i, get_output_name()))

def il2input(il : list) -> list:
    '''
    Mapped i2input.
    '''
    return list(map(i2input, il))

def il2output(il : list) -> list:
    '''
    Mapped i2output.
    '''
    return list(map(i2output, il))

def il2io(il : list) -> tuple:
    '''
    Get tuple (input, output) for every instance
    in the given list of Instance objects.
    '''
    return (il2input(il), il2output(il))

def fit_data_to_tree(X : list, Y : list) -> tree.DecisionTreeClassifier:
    '''
    Get a decision tree that fits the input X
    and output Y.
    '''
    clf = tree.DecisionTreeClassifier()
    clf = clf.fit(X, Y)
    return clf

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

def accuracy(expected : list, obtained : list) -> float:
    '''
    Calculate accuracy based on how many items match
    between expected and obtained list.
    Assumes lists are equally sized.
    e.g.
    expected = [ 1, 1, 0, 0, 0, 1 ]
    obtained = [ 1, 1, 1, 1, 0, 0 ]
    matches      Y  Y  N  N  Y  N
    accuracy = #Y / (#N + #Y) = 50%
    '''
    return sum(1 for x,y in zip(expected, obtained)
               if x == y) / len(expected)

def accuracy_per_result(expected : list, obtained : list) -> dict:
    '''
    Calculate accuracy for each expected value.
    e.g.
    expected = [ 1, 1, 0, 0, 0, 1 ]
    obtained = [ 1, 1, 1, 1, 0, 0 ]
    exptected 1's = [ 1, 1, _, _, _, 1 ]
    obtained .... = [ 1, 1, _, _, _, 0 ]
    From the 3 expected '1's, 67% were correct.
    exptected 0's = [ _, _, 0, 0, 0, _ ]
    obtained .... = [ _, _, 1, 1, 0, _ ]
    From the 3 expected '0's, 33% were correct.
    '''
    result_set = set(expected)
    return {result : sum(1 for x,y in zip(expected, obtained)
                if x == y and x == result) /
                len([x for x in expected if x == result])
                    for result in result_set}

def test(training : list, testing : list) -> tuple:
    '''
    Fit a decision tree to the training data,
    then test it on the training data.
    At the end, return the accuracy of the test
    on the whole and by each expected value.
    '''
    clf = fit_data_to_tree(*il2io(training))
    Yt = clf.predict(il2input(testing))
    Y = il2output(testing)
    return accuracy(Y, Yt), accuracy_per_result(Y, Yt)

def mean_list(l : list) -> float:
    '''
    Obtain the mean value of a list
    '''
    return sum(l) / len(l)

def mean_dict(d : dict, k : int) -> dict:
    '''
    Divide the values of a dictionary by k
    '''
    return {key : value/k for key, value in d.items()}

def k_fold(k : int) -> tuple:
    '''
    Do a k-fold cross-validation on data
    with decision trees, returning the
    overall accuracy and the accuracy by
    expected value
    '''
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
        print("acc @ {} = {:.2f}%".format(get_output_value_label(key),
                                          value * 100))
