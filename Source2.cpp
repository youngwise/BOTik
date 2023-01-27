#include "NN.h"

int main() {
    NN<double> ai("classification", {4, 10, 3});
    string answers[] = {"Iris-setosa", "Iris-versicolor", "Iris-virginica"};

    ai.load_dataset("iris.txt", answers, ',');

    ai.ALPHA = 0.002;
    ai.NUM_EPOCHS = 400;
    ai.BATCH_SIZE = 50;

    ai.train();
    cout << "Accuracy: " << ai.calc_accuracy() << endl;
}