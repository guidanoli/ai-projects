'''
Decision Tree
===================

Usage: <python> decisionTree.py
       [--kfoldk=<int>]
       [--maxDepth=<int>]
       [--purityMeasure=<str>]
       [--minForSplit=<int>]
    
    kfoldk : k value for k-fold
        1 < kfoldk <= 194
        Default = 10

    maxDepth : maximun depth for the tree
        maxDepth > 0
        Default = 100
    
    purityMeasure : purity measure used
        * giniIndex
        * entropy (Default)

    minForSplit : minimum number of samples to split a non leaf node
        minForSplit > 1
        Default = 2
'''
import instance
import utils
import numpy as np
import matplotlib.pyplot as plt
import operator
from collections import Counter

class DecisionTree(utils.Algorithm):

    def __init__(self,max_depth : int, purity_measure : str, min_for_split : int):

        self.max_depth = max_depth
        self.purity_measure = purity_measure
        self.min_for_split = min_for_split

        if(self.purity_measure == 'entropy'):
            self.purity_function = utils.entropy
            
        elif(self.purity_measure == 'giniIndex'):
            self.purity_function = utils.gini_impurity
        else:
            raise Exception("Measure '{}' not supported".format(self.purity_measure))

        if(max_depth <= 0):
            raise Exception("maxDepth must be greater than 0, is {}".format(max_depth))
        if(min_for_split < 2):
            raise Exception("minForSplit must be greater than 1, is {}".format(min_for_split))

    
    def buildTree(self,training_data,parent = None):
        '''
        Builds the decision tree recursively
        '''
        #
        # nodes parent
        #
        self.parent = parent

        #
        # instance counts dictionary
        # --------------------------
        # Dict where:
        # key:label ----- value:times it occurs in this node 
        #
        instance_counts = Counter(utils.get_labels(training_data))

        #
        # catch empty training_data
        #
        if(instance_counts == Counter([])):
            self.label=parent.label
            self.is_leaf = True
            self.entropy = 0
            return self

        #
        # label with most occurrences
        #
        self.label = max(instance_counts, key=instance_counts.get)

        #
        # impurity of this node
        #
        self.impurity = self.purity_function(instance_counts)



        if((instance_counts[self.label] == training_data.shape[1]) or # all instances in this node have the same label
            ((training_data).shape[1]<self.min_for_split) or          # node has too few instances to split
            (self.max_depth <= 1)):                                   # max tree depth has been reached 
            #
            # this is a leaf node
            #
            # TODO: DEAL WITH TIES
            self.is_leaf = True
            return self

        else:
            #
            # this is an inner node
            #
            self.is_leaf = False

            #
            # Select best attribute for split
            #
            best_mean_impurity = np.Inf
            best_attr = ""
            for (index,attr) in enumerate(utils._attr_names):

                attr_selection = training_data[utils._attr_map][index,...]

                mean_impurity = 0
                partitions = 0

                if(attr in utils._attr_names[utils._int_attr_map]):
                    #
                    # Must select spliting point for numerical attribute
                    #
                    is_numerical = True

                    best_split_impurity = np.Inf
                    best_split_point = -1

                    for attr_value in np.unique(attr_selection):
                        mean_impurity = self.purity_function(Counter(utils.get_labels(training_data[:,attr_selection <= attr_value])))
                        mean_impurity+= self.purity_function(Counter(utils.get_labels(training_data[:,attr_selection > attr_value])))

                        if(mean_impurity<best_split_impurity):
                            best_split_impurity = mean_impurity
                            best_split_point = attr_value

                    partitions=2
                    
                else:
                    #
                    # It is not a numeircal attribute
                    #
                    is_numerical = False

                    for attr_value in range(utils._max_attr_values[index]+1):
                        partitions+= 1
                        mean_impurity+= self.purity_function(Counter(utils.get_labels(training_data[:,attr_selection == attr_value])))


                mean_impurity/= partitions
                if(mean_impurity < best_mean_impurity):
                    #
                    # Update current best attribute
                    #
                    best_mean_impurity=mean_impurity
                    best_attr_name = attr
                    best_attr_index = index
                    is_best_attr_numerical = is_numerical
                    if(is_numerical):
                        best_attr_split = best_split_point
                    else:
                        best_attr_max = utils._max_attr_values[index]


            self.attr_name = best_attr_name
            self.attr_index = best_attr_index
            self.is_numerical = is_best_attr_numerical
            if(is_best_attr_numerical):
                self.best_split = best_attr_split
            else:
                self.max_value=best_attr_max

            #
            # Call recursively for children nodes
            #
            self.children = []
            if(self.is_numerical):
                self.children.append(DecisionTree(self.max_depth-1,self.purity_measure,self.min_for_split))
                self.children.append(DecisionTree(self.max_depth-1,self.purity_measure,self.min_for_split))
                self.children[0].buildTree(training_data[:,training_data[utils._attr_map][self.attr_index,...] <= self.best_split],self)
                self.children[1].buildTree(training_data[:,training_data[utils._attr_map][self.attr_index,...] > self.best_split],self)
            else:
                for val in range(self.max_value + 1):
                    self.children.append(DecisionTree(self.max_depth-1,self.purity_measure,self.min_for_split))
                    self.children[-1].buildTree(training_data[:,training_data[utils._attr_map][self.attr_index,...] == val],self)
            
            return self

                

    def fit(self, training_data : np.ndarray) -> None:
        '''
        Calls the tree building function
        '''
        self.buildTree(training_data)
        return

    def predict(self, testing_data : np.ndarray) -> np.ndarray:
        '''
        Predicts the label of testing data by the Decision Tree
        '''
        if(self.is_leaf):
            return np.zeros(testing_data.shape[1],dtype=int)+self.label
        else:
            result = np.zeros(testing_data.shape[1],dtype=int)

            attr_view = testing_data[self.attr_index,...]
            if(self.is_numerical):
                criteria_map = attr_view<=self.best_split
                less = self.children[0].predict(testing_data[:,criteria_map])
                more = self.children[1].predict(testing_data[:,~criteria_map])

                
                less_counter = 0
                more_counter = 0
                for i in range(len(criteria_map)):
                    if(criteria_map[i]):
                        result[i]=less[less_counter]
                        less_counter+=1
                    else:
                        result[i]=more[more_counter]
                        more_counter+=1
                return result
            else:
                child_results=[]
                child_counters={}
                for val,node in enumerate(self.children):
                    child_results.append(node.predict(testing_data[:,attr_view == val]))
                    child_counters[val]=0
                for (i,val) in enumerate(attr_view):
                    result[i]=child_results[val][child_counters[val]]
                    child_counters[val]+=1
                
                return result



if __name__ == '__main__':
    args = utils.get_command_line_arguments()
    decision_tree = DecisionTree(int(args.get("maxDepth",100)),args.get("purityMeasure","entropy"),int(args.get("minForSplit",2)))
    confm = utils.k_fold(int(args.get('kfoldk', '10')), decision_tree)
    print(confm)
