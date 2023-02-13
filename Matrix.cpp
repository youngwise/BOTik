#include "Matrix.hpp"


int64_t nano() {
    time_point<system_clock> now = system_clock::now();
    auto duration = now.time_since_epoch();
    auto nanosec = static_cast<nanoseconds>(duration);
    return nanosec.count();
}


template <class T>
matrix<T> maximum(matrix<T> m1, matrix<T> m2) {
    if (m1.sizes() == m2.sizes()) {
        int m = m1.sizes()[0];
        int n = m1.sizes()[1];
        matrix<T> c(m, n);
        for (int i = 0; i < m; i++)
            for (int j = 0; j < n; j++)
                c[i][j] = m1[i][j] > m2[i][j] ? m1[i][j] : m2[i][j];
        return c;
    }
    else {
        cout << "Error matrix:\n\tmatrix sizes do not match!" << endl;
        abort();
    }
}

template <class T>
matrix<T> maximum(matrix<T> m1, T x) {
    int m = m1.sizes()[0];
    int n = m1.sizes()[1];
    matrix<T> c(m, n);
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            c[i][j] = m1[i][j] > x ? m1[i][j] : x;
    return c;
}

template <class T>
matrix<T> maximum(T x, matrix<T> m1) {
    int m = m1.sizes()[0];
    int n = m1.sizes()[1];
    matrix<T> c(m, n);
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            c[i][j] = m1[i][j] > x ? m1[i][j] : x;
    return c;
}

/**Фунция активации RELU*/
template <class T>
matrix<T> relu(matrix<T> mtx) {
    matrix<T> c = mtx;
    return maximum(c, 0.);
}

template <class T>
matrix<T> softmax(matrix<T> mtx) {
    matrix out = mtx.expa();
    return out / out.sum();
}

template <class T>
matrix<T> softmax_batch(matrix<T> mtx) {
    matrix out = mtx.expa();
    return out / out.sum_axis(1);
}

/**Заполнить матрицу размерами m * n действительными числами от 0 до 1*/
matrix<double> randn(int m = 1, int n = 1) {
    matrix<double> c;
    c.load_size(m, n);
    c.randf();
    return c;
}

/**Заполнить матрицу размерами m * n 0*/
matrix<double> zeros(int m, int n) {
    matrix<double> c;
    c.load_size(m, n);
    return c;
}

template <class T>
double sparse_cross_entropy(matrix<T> z, int y) {
    return -log(z[0][y]);
}

template <class T>
matrix<T> sparse_cross_entropy_batch(matrix<T> z, matrix<T> y) {
    matrix<T> c(z.sizes()[0], 1);
    for (int i = 0; i < y.sizes()[0]; i++)
        c[i][0] = -log(z[i][y[i][0]]);
    return c;
}

/**Получить вектор правильного класса*/
matrix<double> to_full(int y, int num_classes) {
    matrix<double> y_full = zeros(1, num_classes);
    y_full[0][y] = 1;
    return y_full;
}

/**Получить вектор правильного класса*/
template <class T>
matrix<T> to_full_batch(matrix<T> y, int num_classes) {
    matrix y_full = zeros(y.sizes()[0], num_classes);
    for (int i = 0; i < y.sizes()[0]; i++)
        y_full[i][y[i][0]] = 1;
    return y_full;
}

/**Производная от функции активации RELU*/
template <class T>
matrix<double> relu_deriv(matrix<T> t) {
    return (matrix<double>) (t >= 0);
}
