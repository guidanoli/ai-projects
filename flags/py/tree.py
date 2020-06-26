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

data = instance.parse()

ptcg_for_training = 1/2
cnt_for_training = int(len(data) * ptcg_for_training)

training_data = data[:cnt_for_training]
testing_data = data[cnt_for_training:]

X = [instance2data(i) for i in training_data]
Y = [instance2result(i) for i in training_data]
clf = tree.DecisionTreeClassifier()
clf = clf.fit(X, Y)

X_test = [instance2data(i) for i in testing_data]
Y_test = [instance2result(i) for i in testing_data]
Y_test_predict = clf.predict(X_test)

correct_predictions = 0
for i in range(len(X_test)):
    if Y_test[i] == Y_test_predict[i]:
        correct_predictions += 1

print("#training = {}".format(len(training_data)))
print("#testing = {}".format(len(testing_data)))
print("accurracy = {}%".format(correct_predictions/len(X_test)))