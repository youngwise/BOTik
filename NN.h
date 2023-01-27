#include "Matrix.cpp"
#include <fstream>
#include <cstring>
#include <algorithm>
//#include <SFML/Graphics.hpp>

using std::ifstream;

//using sf::Event;
//using sf::RenderWindow;
//using sf::CircleShape;
//using sf::Color;
//using sf::VertexArray;

template <class Type>
class NN {
public:
    /**
     * Класс NN позволяет создавать нейросеть в пару строчек!
     * Для начала создаёте объект класса, где указываете тип нейросети и структуру нейросети.
     * Всё просто!
     * Чтобы задать структуру вы должны знать сколько у нейросети должно быть входных и выходных данных,
     * а также кол-во скрытых слоёв.
     * Например для нейросети, принимающая 4 входных параметра, имеющая 2 нейрона в 1 скрытом слое и возвращающая вектор,
     * состоящий из 3 чисел - вектор правильного класса вы должны указать как {4, 2, 3}.
     * Т.е. NN<double> ai("classification", {4, 2, 3});
     * Так как класс имеет шаблон, то не забудьте указать с какими данными вы работаете!
     *
     * После создания объекта класса вы должны обучить модель на датасете.
     * Для начала загрузите модель методом load_dataset(...), укажите, по желанию, необходимые гиперпараметры (ALPHA,
     * NUM_EPOCHS, BATCH_SIZE) и запустите обучение методом train().
     *
     * После мы можете узнать точность обучения, предсказать или же изменить модель нейросети.
     *
     * В процессе разработка функции, принимающая пользовательськие веса для нейросети.
     * */

    double ALPHA = 0.001;
    int NUM_EPOCHS = 100, BATCH_SIZE = 50;

    /**Соберите желаемую модель нейросети*/
    NN(const string& method, initializer_list<int> param) {
        if (method != "classification") {
            cout << "Method '"<< method << "' does not exist!" << endl;
            abort();
        }
        else {
            layers.resize(param.size());
            const int* ptr = param.begin();
            for (int i = 0; i < param.size(); i++)
                layers[i] = *(ptr++);

            type = method;
            build();
        }
    }

    /**Пересборка модели нейросети*/
    void rebuild(initializer_list<Type> param) {
        layers.resize(param.size());
        const Type* ptr = param.begin();
        for (int i = 0; i < param.size(); i++)
            layers[i] = *(ptr++);

        build();
    }

    /**
     * Загрузка датасета.
     * Принимает файл с данными и ответом в каждой сроке
     * Формат для нейросети которая имеет 4 входа и 2 выхода:
     * x1, x2, x3, x4, y1, y2
     * где x - входные данные, y - правильные ответы
    */
    void load_dataset(const string& src, string ans[], char dep) {
        string line;
        ifstream in(src);
        if (in.is_open()) {
            int pos = 0;
            int endp = 0;
            string result;
            vector<Type> xs;
            vector<Type> c;
            int y = layers[layers.size()-1];
            while (getline(in, line) && !line.empty()) {
                while (endp >= 0) {
                    endp = line.find(dep, pos);
                    result = line.substr(pos, endp-pos);
                    if (xs.size() < layers[0]-1)
                        xs.push_back(stod(result));
                    else if (xs.size() == layers[0] - 1) {
                        xs.push_back((Type) stod(result));
                        dataset.push_back(xs);
                    }
                    else {
                        c.push_back((Type) distance(ans, find(ans, ans+y-1, result)));
                        dataset[dataset.size()-1].push_back(c);
                    }
                    pos = endp+1;
                }
                pos = 0;
                endp = 0;
                xs.clear();
                c.clear();
            }
            in.close();
        }
        else {
            cout << "file '" << src << "' not found!";
            abort();
        }
    }

    /**Запустите процесс обучения*/
    void train() {
        for (int ep = 0; ep < NUM_EPOCHS; ep++) {
            dataset.shuffle_matrix();
            for (int i = 0; i < dataset.size() / BATCH_SIZE; i++) {
                matrix slice = dataset.slice(i*BATCH_SIZE, i*BATCH_SIZE+BATCH_SIZE);
                matrix batch = slice.template split<Type>();

                matrix x = batch[0][0];
                matrix y = batch[0][1];

                matrix z = forward(x);
                double E = sparse_cross_entropy_batch(z, y).sum();

                update_weighs(x, y, z);

                loss_arr.push_back(E);
            }
        }
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

    /**Предсказать и вернуть вектор правильного класса*/
    matrix<Type> predict(matrix<Type> x) {
        matrix<Type> t, h, z;
        for (int i = 0; i < weights.size(); i++) {
            t = (i == 0 ? x : h) * weights[i][0] + weights[i][1];
            if (i + 1 < weights.size())
                h = relu(t);
        }

        z = softmax(t);
        return z;
    }

//    /**Нарисовать график зависимости элемента массива от его индекса (Beta)*/
//    void draw_graph(float scale, int width = 600, int height = 600) {
//        int W = width, H = height;
//        RenderWindow window(sf::VideoMode(W, H), "Graph");
//
//        float x0 = 10;
//        float y0 = H-10;
//
//        int count = loss_arr.size();
//
//        // Массив точек, из которых образуется ломаная кривая
//        VertexArray lines(sf::LineStrip, count);
//
//        while (window.isOpen()) {
//            Event event;
//            while (window.pollEvent(event))
//            {
//                if (event.type == sf::Event::Closed)
//                    window.close();
//            }
//
//            // Заливка фона
//            window.clear(Color::White);
//
//            // Строим график зависимости, задавая координаты и цвет
//            for (int i = 0; i < count;  i++) {
//                double x = i/(scale*3);
//                double y = loss_arr[i];
//
//                double x1 = x0 + x * scale;
//                double y1 = y0 - y * scale;
//
//                lines[i].position.x = x1;
//                lines[i].position.y = y1;
//                lines[i].color = Color::Blue;
//            }
//            window.draw(lines);
//            window.display();
//        }
//    }


    /**Измените параметр модели нейросети*/
    void operator[](int i, int x) {
        layers[i] = x;
        weights.load_size(weights.sizes()[0], weights.sizes()[1]);
        build();
    }



private:
    vector<int> layers;
    string type;
    matrix<matrix<Type>> weights;
    matrix<vector<Type>> dataset;
    vector<Type> loss_arr;
    matrix<matrix<Type>> neurons;

    /**Сборка модели со случайным распределением чисел*/
    void build() {
        weights.load_size(layers.size()-1, 2);
        neurons.load_size(weights.size(), 2);
        for (int i = 0; i < layers.size()-1; i++) {
            weights[i][0] = (randn(layers[i], layers[i+1]) - 0.5) * 2 * sqrt(1./layers[i]);
            weights[i][1] = (randn(1, layers[i+1]) - 0.5) * 2 * sqrt(1./layers[i]);
        }
    }

    matrix<Type> forward(matrix<Type> x) {
        for (int i = 0; i < weights.size(); i++) {
            neurons[i][0] = (i == 0 ? x : neurons[i-1][1]) * weights[i][0] + weights[i][1];
            if (i + 1 < weights.size())
                neurons[i][1] = relu(neurons[i][0]);
        }

        return softmax_batch(neurons[neurons.size()-1][0]);
    }

    void update_weighs(matrix<Type> x, matrix<Type> y, matrix<Type> z) {
        matrix y_full = to_full_batch(y, layers[layers.size()-1]);
        matrix<Type> dE_dt, dE_dh, dE_dW, dE_db;
        for (int i = weights.size()-1; i >= 0; i--) {
            if (i == weights.size()-1)
                dE_dt = z - y_full;
            else
                dE_dt = dE_dh ^ relu_deriv(neurons[i][0]);

            dE_dW = (i > 0 ? neurons[i-1][1].T(): x.T()) * dE_dt;
            dE_db = dE_dt.sum_axis(0);

            if (i > 0)
                dE_dh = dE_dt * weights[i][0].T();

            weights[i][0] -= ALPHA * dE_dW;
            weights[i][1] -= ALPHA * dE_db;
        }
    }
};