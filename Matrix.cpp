#include <vector>
#include <cmath>
#include <iostream>
using namespace std;

template <typename Type>
struct matrix {
public:
    /*
     * Объект класса можно задать как matrix<тип данных> x;
     * Если необходимо матрицу заполнить определёнными числами: matrix x = {{1, 2}, {2, 1}};
     * В случае указания 1-мерного массива (matrix x = {1, 2, 3};) матрица примет вид вектора-строки.
     * Реализованы такие операции как сложение и умножение.
     * Можно сравнить матрицы пока только >=; Можно сравнивать с числом.
     * В процессе реализация преобразования матрицы с одного типа в другой.
     */
    matrix(initializer_list<initializer_list <Type>> l) {
        size_m = l.size();
        size_n = l.begin()->size();
        load_size(size_m, size_n);
        for (int i = 0; i < size_m; i++) {
            const initializer_list <Type>* m_ptr = l.begin()+i;
            for (int j = 0; j < size_n; j++)
                mtx[i][j] = *(m_ptr->begin()+j);
        }
    }
    matrix(initializer_list <Type> l) {
        size_m = 1;
        size_n = l.size();
        load_size(size_m, size_n);
        const Type* ptr = l.begin();
        for (int i = 0; i < size_n; i++)
            mtx[0][i] = *(ptr+i);
    }
    matrix(int m = 0, int n = 1): size_m(m), size_n(n){}
//    matrix(matrix<Type> &m) {
//        size_m = m.size_m;
//        size_n = m.size_n;
//        load_size(size_m, size_n);
//        for (int i = 0; i < size_m; i++)
//            for (int j = 0; j < size_n; j++)
//                mtx[i][j] = m[i][j];
//    }
//    operator Type() const{
//        matrix<Type> c;
//        c.load_size(size_m, size_n);
//        for (int i = 0; i < size_m; i++)
//            for (int j = 0; j < size_n; j++)
//                c[i][j] = mtx[i][j];
//        return c;
//    }
//
    vector<Type>& operator[](int i) {
        return mtx[i];
    }
    /*
     * Вернуть строку/стобец матрицы.
     * Если необходимо заменить какой-то элемент матрицы matrix x: x[i][j] = y.
     */


    friend matrix operator+(matrix m1, matrix m2) {
        if(m1.size_n+m1.size_m == m2.size_m + m2.size_n) {
            int s_m = m1.size_m;
            int s_n = m1.size_n;
            matrix c;
            c.load_size(s_m, s_n);
            for (int i = 0; i < s_m; i++)
                for (int j = 0; j < s_n; j++) {
                    c[i][j] = m1[i][j] + m2[i][j];
                }
            return c;
        } else {
            cout << "Error of matrix sum:\n\tmatrix sizes do not match!" << endl;
            abort();
        }
    }
    friend matrix operator-(matrix m1, matrix m2) {
        if(m1.size_n+m1.size_m == m2.size_m + m2.size_n) {
            int s_m = m1.size_m;
            int s_n = m1.size_n;
            matrix c;
            c.load_size(s_m, s_n);
            for (int i = 0; i < s_m; i++)
                for (int j = 0; j < s_n; j++) {
                    c[i][j] = m1[i][j] - m2[i][j];
                }
            return c;
        } else {
            cout << "Error of matrix sum:\n\tmatrix sizes do not match!" << endl;
            abort();
        }
    }

    friend matrix operator*(matrix m1, matrix m2) {
        if (m1.size_n == m2.size_m) {
            int s_m = m1.size_m;
            int s_n = m2.size_n;

            matrix c;
            c.load_size(s_m, s_n);

            for (int i = 0; i < s_m; i++)
                for (int j = 0; j < s_n; j++) {
                    double res = 0;
                    for (int mi = 0; mi < m1.size_n; mi++)
                        res += m1[i][mi]*m2[mi][j];
                    c[i][j] = res;
                }
            return c;
        } else {
            cout << "Matrix multiplication error:\n\tthe number of columns of matrix 1 and the number of rows of matrix 2 do not match!" << endl;
            abort();
        }
    }

    friend matrix operator*(matrix m, Type x) {
        for (int i = 0; i < m.size_m; i++)
            for (int j = 0; j < m.size_n; j++)
                m[i][j] *= x;
        return m;
    }

    friend matrix operator/(matrix m, Type x) {
        for (int i = 0; i < m.size_m; i++)
            for (int j = 0; j < m.size_n; j++)
                m[i][j] /= x;
        return m;
    }

    matrix<Type>& operator=(matrix<Type> m) {
        int s_m = m.size_m;
        int s_n = m.size_n;
        this->load_size(s_m, s_n);
        for (int i = 0; i < s_m; i++)
            for (int j = 0; j < s_n; j++) {
                mtx[i][j] = m[i][j];
            }
        return *this;
    }

    void get_matrix(vector<vector<Type>> m) {
        size_m = m.size();
        size_n = m.at(0).size();
        load_size(size_m, size_n);
        for (int i = 0; i < size_m; i++)
            for (int j = 0; j < size_n; j++)
                mtx[i][j] = m[i][j];
    }
    /*Получить матрицу из 2-йного вектора*/

    matrix& operator+=(matrix m) {
        for (int i = 0; i < size_m; i++)
            for (int j = 0; j < size_n; j++) {
                mtx[i][j]+= m[i][j];
            }
        return *this;
    }

    friend matrix operator>=(matrix m1, matrix m2) {
        if (m1.size_m+m1.size_n == m2.size_m+m2.size_n) {
            matrix c;
            int s_m = m1.size_m;
            int s_n = m1.size_n;
            c.load_size(s_m, s_n);
            for (int i = 0; i < s_m; i++)
                for (int j = 0; j < s_n; j++)
                    c[i][j] = m1[i][j] >= m2[i][j];
            return c;
        } else {
            cout << "Error:\n\tmatrix sizes do not match!" << endl;
            abort();
        }
    }

    friend matrix<bool> operator>=(matrix m1, Type x) {
        matrix<bool> c;
        int s_m = m1.size_m;
        int s_n = m1.size_n;
        c.load_size(s_m, s_n);
        for (int i = 0; i < s_m; i++)
            for (int j = 0; j < s_n; j++)
                c[i][j] = m1[i][j] >= x;
        return c;
    }

    void randint(int from, int to) {
        if (size_m != 0) {
            srand(clock());
            for (int i = 0; i < size_m; i++)
                for (int j = 0; j < size_n; j++)
                    mtx[i][j] = from + rand() % to;
        } else {
            cout << "Error: the matrix is not defined" << endl;
            abort();
        }
    }
    /* Заполнить матрицу целыми числами в определённом диапозоне */
    void randf(double from = 0, double to = 1) {
        if (size_m != 0) {
            srand(clock());
            for (int i = 0; i < size_m; i++)
                for (int j = 0; j < size_n; j++)
                    mtx[i][j] = from + rand() / ((double) RAND_MAX / (to - from));
        } else {
            cout << "Error: the matrix is not defined" << endl;
            abort();
        }
    }
    /* Заполнить матрицу действительными числами в определённом диапозоне */
    void print() {
        cout << '[';
        for (int i = 0; i < size_m; i++) {
            if (i != 0)
                cout << ' ';
            cout << '[';
            for (int j = 0; j < size_n; j++) {
                cout << mtx[i][j];
                if (j + 1 < size_n)
                    cout << ',';
            }
            cout << ']';
            if (i+1 != size_m)
                cout << endl;
        }
        cout << ']' << endl;
    }
    void load_size(int m, int n) {
        size_m = m;
        size_n = n;
        mtx.resize(m);
        for (int i = 0; i < m; i++)
            mtx.at(i).resize(n);
    }

    matrix maximum() {
        matrix c;
        double max;
        c.load_size(1, size_n);
        for (int i = 0; i < size_n; i++) {
            max = 0;
            for (int j = 0; j < size_m; j++)
                max = mtx[j][i] > max ? mtx[j][i] : max;
            c[0][i] = max;
        }
        return c;
    }
    matrix expa() {
        matrix c;
        c.load_size(size_m, size_n);
        for (int i = 0; i < size_m; i++)
            for (int j = 0; j < size_n; j++)
                c[i][j] = exp(mtx[i][j]);
        return c;
    }
    /*e^x где x - каждый элемент матрицы*/

    long double sum() {
        double sum = 0;
        for (int i = 0; i < size_m; i++)
            for (int j = 0; j < size_n; j++)
                sum += mtx[i][j];
        return sum;
    }
    int argmax() {
        double max = mtx[0][0];
        int index = 0;
        for (int i = 0; i < size_m; i++)
            for (int j = 0; j < size_n; j++)
                if (mtx[i][j] > max) {
                    max = mtx[i][j];
                    index = size_n * i + j;
                }
        return index;
    }
    /*Максимум из кадого столбца матрицы*/
    matrix T(){
        matrix c;
        c.load_size(size_n, size_m);
        for (int i = 0; i < size_m; i++)
            for (int j = 0; j < size_n; j++)
                c[j][i] = mtx[i][j];
        return c;
    };
    /*Транспонирование матрицы*/
private:
    vector<vector<Type>> mtx;
    int size_m = 0, size_n = 1;
};

template <class T>
matrix<T> relu(matrix<T> mtx) {
    return mtx.maximum();
}
/*Фунция активации RELU*/
template <class T>
matrix<T> softmax(matrix<T> mtx) {
    matrix out = mtx.expa();
    return out/out.sum();
}
/**/
matrix<double> randn(int m = 1, int n = 1) {
    matrix<double> c;
    c.load_size(m, n);
    c.randf();
    return c;
}
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
matrix<T> to_full(int y, int num_classes) {
    matrix<T> y_full = zeros(1, num_classes);
    y_full[0][y] = 1;
    return y_full;
}
template <class T>
matrix<T> relu_deriv(matrix<T> t) {
    return (double) (t >= 0);
}