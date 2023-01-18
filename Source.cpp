#include "Matrix.cpp"
#include <fstream>
#include <cstring>
#include <algorithm>
using std::string;
using std::ifstream;
using std::getline;
using std::strtok;
using std::find;
using std::distance;

int INPUT_DIM = 4;
int OUT_DIM = 3;
int H_DIM = 5;

matrix<double> W1, b1, W2, b2;

/**
 * Получить случайное целое число в заданном диапазоне
 * */
int randint(int from, int to) {
    srand(clock());
    return rand() % to + from;
}

/**
 * Загрузка датасета.
 * Принимает файл с данными и ответом в каждой сроке
 * Формат для нейросети которая имеет 4 входа и 2 выхода:
 * x1, x2, x3, x4, y1, y2
 * где x - входные данные, y - правильные ответы
*/
template <class T>
matrix<vector<T>> load_dataset(string src, string ans[], char dep) {
    string line;
    ifstream in(src);
    if (in.is_open()) {
        matrix<vector<T>> mas;
        int pos = 0;
        int endp = 0;
        string result;
        vector<T> xs;
        vector<T> c;
        while (getline(in, line) && !line.empty()) {
            while (endp >= 0) {
                endp = line.find(dep, pos);
                result = line.substr(pos, endp-pos);
                if (xs.size() < INPUT_DIM-1)
                    xs.push_back(stod(result));
                else if (xs.size() == INPUT_DIM - 1) {
                    xs.push_back((T) stod(result));
                    mas.push_back(xs);
                }
                else {
                    c.push_back((T) distance(ans, find(ans, ans+OUT_DIM-1, result)));
                    mas[mas.size()-1].push_back(c);
                }
                pos = endp+1;
            }
            pos = 0;
            endp = 0;
            xs.clear();
            c.clear();
        }
        in.close();
        return mas;
    }
    else {
        cout << "file '" << src << "' not found!";
        abort();
    }
}

string class_names[] = {"Iris-setosa", "Iris-versicolor", "Iris-virginica"};
matrix dataset = load_dataset<double>("iris.txt", class_names, ',');

/**Предсказать*/
template <class T>
matrix<T> predict(matrix<T> x) {
    matrix t1 = x * W1 + b1;
    matrix h1 = relu(t1);
    matrix t2 = h1 * W2 + b2;
    matrix z = softmax(t2);
    return z;
}

/**Посчитать точность обучения*/
double calc_accuracy() {
    int correct = 0;
    for (int i = 0; i < dataset.size(); i++) {
        matrix x = dataset[i][0];
        int y = dataset[i][1][0];
        matrix z = predict(x);
        int y_pred = z.argmax();

        if (y_pred == y)
            correct += 1;
    }

    double acc = (double) correct / (double) dataset.size();
    return acc;
}

int main() {
    /**
     * Алгоритм обучения нейросети и с оценкой точности предсказывания.
     * В планах реализовать вывод графика как отчёта точности предсказывания,
     * а также реализовать класс для создания динамической нейросети!
     */

    W1 = randn(INPUT_DIM, H_DIM);
    b1 = randn(1, H_DIM);
    W2 = randn(H_DIM, OUT_DIM);
    b2 = randn(1, OUT_DIM);

    /**Задаём коэффициент обучения и кол-во эпох обучения*/
    double ALPHA = 0.001;
    int NUM_EPOCHS = 100;

    vector<double> loss_arr;

    for (int ep = 0; ep < NUM_EPOCHS; ep++)
        for (int i = 0; i < dataset.size(); i++) {
            matrix x = dataset[i][0];
            int y = dataset[i][1][0];

            // Forward
            matrix t1 = x * W1 + b1;
            matrix h1 = relu(t1);
            matrix t2 = h1 * W2 + b2;
            matrix z = softmax(t2);
            double E = sparse_cross_entropy(z, y);

            // Backward
            matrix y_full = to_full(y, OUT_DIM);
            matrix dE_dt2 = z - y_full;
            matrix dE_dW2 = h1.T() * dE_dt2;
            matrix dE_db2 = dE_dt2;
            matrix dE_dh1 = dE_dt2 * W2.T();
            matrix dE_dt1 = dE_dh1 ^ relu_deriv(t1);
            matrix dE_dW1 = x.T() * dE_dt1;
            matrix dE_db1 = dE_dt1;

            // Update
            W1 -= ALPHA * dE_dW1;
            b1 -= ALPHA * dE_db1;
            W2 -= ALPHA * dE_dW2;
            b2 -= ALPHA * dE_db2;

            loss_arr.push_back(E);
        }

    /**Найдём точность предсказания*/
    double accuracy = calc_accuracy();
    cout << "Accuracy: " << accuracy << endl;
    return 0;
}