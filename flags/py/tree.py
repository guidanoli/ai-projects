import instance
from sklearn import tree

def instance2data(i):
    data=  [i.area,
            i.bars,
            i.stripes,
            i.colours,
            i.red,
            i.green,
            i.blue,
            i.gold,
            i.white,
            i.black,
            i.orange,
            i.mainhue,
            i.circles,
            i.crosses,
            i.saltires,
            i.quarters,
            i.sunstars,
            i.crescent,
            i.triangle,
            i.icon,
            i.animate,
            i.text,
            i.topleft,
            i.botright]
    return [int(x) for x in data]

def instance2result(i):
    return int(i.religion)

def il2x(il):
    return list(map(instance2data, il))

def il2y(il):
    return list(map(instance2result, il))

def il2xy(il):
    return (il2x(il), il2y(il))

data = instance.parse()

def fit_dataset(X, Y):
    clf = tree.DecisionTreeClassifier()
    clf = clf.fit(X, Y)
    return clf

def split(l : list, k : int):
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

def test(training, testing):
    clf = fit_dataset(*il2xy(training))
    Yt = clf.predict(il2x(testing))
    return accuracy(il2y(testing), Yt)

def mean(l : list):
    return sum(l) / len(l)

def k_fold(k : int):
    chunks = split(data, k)
    acc_list = []
    for i in range(k):
        training = chunks[:i] + chunks[i+1:]
        training = sum(training, [])
        testing = chunks[i]
        acc = test(training, testing)
        acc_list.append(acc)
    return mean(acc_list)

if __name__ == '__main__':
    import sys
    k = 10
    for arg in sys.argv:
        if '=' in arg:
            key, val = arg.split('=')
            if key == 'k':
                k = int(val)
    acc = k_fold(k)
    print("acc = {:.2f}%".format(acc * 100))
