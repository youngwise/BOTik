#include <vector>
#include <iostream>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <random>

using std::cout;
using std::endl;
using std::exp;
using std::vector;
using std::initializer_list;
using std::default_random_engine;
using std::shuffle;
using std::copy;
using std::string;

using std::chrono::time_point;
using std::chrono::system_clock;
using std::chrono::nanoseconds;

int64_t nano() {
    time_point<system_clock> now = system_clock::now();
    auto duration = now.time_since_epoch();
    auto nanosec = duration_cast<nanoseconds>(duration);
    return nanosec.count();
}

template <typename Type>
struct matrix {
public:
    /**
     * Объект класса можно задать как matrix<тип данных> x;
     * Если необходимо матрицу заполнить определёнными числами: matrix x = {{1, 2}, {2, 1}};
     * Конструктор также принимает vector.
     * В случае указания 1-мерного массива (matrix x = {1, 2, 3};) матрица примет вид вектора-строки.
     * При желании можно сразу указать размеры матрицы при создании объекта класса как martix<тип данных> x(m, n);
     * Реализованы такие операции как сложение и умножение матриц, умножение и деление матрицы на число,
     * поэлементное умножение элементов матриц.
     * Матрицы можно сравнивать только одинкового размера, а также реализавно сравнение с числом
     * Матрицы можно явно переобразовывать в double, int и bool. Числа не округляются!
     * Все действия с матрицами доступны только если они одного типа!
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

    matrix(vector<vector <Type>> l) {
        size_m = l.size();
        size_n = l[0].size();
        load_size(size_m, size_n);

        for (int i = 0; i < size_m; i++)
            for (int j = 0; j < size_n; j++)
                mtx[i][j] = l[i][j];
    }

    matrix(initializer_list <Type> l) {
        size_m = 1;
        size_n = l.size();
        load_size(size_m, size_n);
        const Type* ptr = l.begin();

        for (int i = 0; i < size_n; i++)
            mtx[0][i] = *(ptr+i);
    }

    matrix(vector <Type> l) {
        size_m = 1;
        size_n = l.size();
        load_size(size_m, size_n);

        for (int i = 0; i < size_n; i++)
            mtx[0][i] = l[i];
    }

    matrix(int m = 0, int n = 0){
        if (m != 0)
            load_size(m, n);
    }
    ~matrix() {
        mtx.clear();
    }

    /**
     * Вернуть строку/стобец матрицы.
     * Если необходимо заменить какой-то элемент матрицы matrix x: x[i][j] = y.
     */
    vector<Type>& operator[](int i) {
        return mtx[i];
    }

    friend matrix<Type> operator+(matrix<Type> m1, matrix<Type> m2) {
        if(m1.size_n == m2.size_n) {
            int s_m = m1.size_m;
            int s_n = m1.size_n;
            matrix<Type> c(s_m, s_n);

            for (int i = 0; i < s_m; i++)
                for (int j = 0; j < s_n; j++)
                    c[i][j] = m1[i][j] + m2[m2.size_m == 1 ? 0 : i][j];
            return c;
        } else {
            cout << "Error of matrix sum:\n\tmatrix sizes do not match!" << endl;
            abort();
        }
    }

    friend matrix<Type> operator+(matrix<Type> m1, Type x) {
        int s_m = m1.size_m;
        int s_n = m1.size_n;
        matrix<Type> c(s_m, s_n);

        for (int i = 0; i < s_m; i++)
            for (int j = 0; j < s_n; j++) {
                c[i][j] = m1[i][j] + x;
            }
        return c;
    }

    friend matrix<Type> operator+(Type x, matrix<Type> m1) {
        int s_m = m1.size_m;
        int s_n = m1.size_n;
        matrix<Type> c(s_m, s_n);

        for (int i = 0; i < s_m; i++)
            for (int j = 0; j < s_n; j++) {
                c[i][j] = m1[i][j] + x;
            }
        return c;
    }

    matrix<Type>& operator+=(matrix<Type> m) {
        if(size_n == m.size_n) {
            for (int i = 0; i < size_m; i++)
                for (int j = 0; j < size_n; j++) {
                    mtx[i][j] += m[m.size_m == 1 ? 0 : i][j];
                }
            return *this;
        } else {
            cout << "Error of matrix sum:\n\tmatrix sizes do not match!" << endl;
            abort();
        }
    }

    matrix<Type>& operator+=(Type x) {
        for (int i = 0; i < size_m; i++)
            for (int j = 0; j < size_n; j++) {
                mtx[i][j] += x;
            }
        return *this;
    }

    friend matrix<Type> operator-(matrix<Type> m1, matrix<Type> m2) {
        if(m1.size_n == m2.size_n) {
            int s_m = m1.size_m;
            int s_n = m1.size_n;
            matrix<Type> c(s_m, s_n);

            for (int i = 0; i < s_m; i++)
                for (int j = 0; j < s_n; j++) {
                    c[i][j] = m1[i][j] - m2[m2.size_m == 1 ? 0 : i][j];
                }
            return c;
        } else {
            cout << "Error of matrix subtract:\n\tmatrix sizes do not match!" << endl;
            abort();
        }
    }

    friend matrix<Type> operator-(matrix<Type> m1, Type x) {
        int s_m = m1.size_m;
        int s_n = m1.size_n;
        matrix<Type> c(s_m, s_n);

        for (int i = 0; i < s_m; i++)
            for (int j = 0; j < s_n; j++) {
                c[i][j] = m1[i][j] - x;
            }
        return c;
    }

    friend matrix<Type> operator-(Type x, matrix<Type> m1) {
        int s_m = m1.size_m;
        int s_n = m1.size_n;
        matrix<Type> c(s_m, s_n);

        for (int i = 0; i < s_m; i++)
            for (int j = 0; j < s_n; j++) {
                c[i][j] = x - m1[i][j];
            }
        return c;
    }

    matrix<Type>& operator-=(matrix<Type> m) {
        if(size_n == m.size_n) {
            for (int i = 0; i < size_m; i++)
                for (int j = 0; j < size_n; j++)
                    mtx[i][j] -= m[m.size_m == 1 ? 0 : i][j];
            return *this;
        } else {
            cout << "Error of matrix subtract:\n\tmatrix sizes do not match!" << endl;
            abort();
        }
    }

    matrix<Type>& operator-=(Type x) {
        for (int i = 0; i < size_m; i++)
            for (int j = 0; j < size_n; j++) {
                mtx[i][j] -= x;
            }
        return *this;
    }

    friend matrix<Type> operator*(matrix<Type> m1, matrix<Type> m2) {
        if (m1.size_n == m2.size_m) {
            int s_m = m1.size_m;
            int s_n = m2.size_n;
            matrix<Type> c(s_m, s_n);

            for (int i = 0; i < s_m; i++)
                for (int j = 0; j < s_n; j++) {
                    Type res = 0;
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

    matrix<Type>& operator*=(matrix<Type> m) {
        if (size_n == m.size_m) {
            int s_m = size_m;
            int s_n = m.size_n;
            matrix<Type> c(s_m, s_n);

            for (int i = 0; i < s_m; i++)
                for (int j = 0; j < s_n; j++) {
                    Type res = 0;
                    for (int mi = 0; mi < size_n; mi++)
                        res += mtx[i][mi] * m[mi][j];
                    c[i][j] = res;
                }
            return c;
        } else {
            cout << "Matrix multiplication error:\n\tthe number of columns of matrix 1 and the number of rows of matrix 2 do not match!" << endl;
            abort();
        }
    }

    friend matrix<Type> operator*(matrix<Type> m, Type x) {
        for (int i = 0; i < m.size_m; i++)
            for (int j = 0; j < m.size_n; j++)
                m[i][j] *= x;
        return m;
    }

    friend matrix<Type> operator*(Type x, matrix<Type> m) {
        for (int i = 0; i < m.size_m; i++)
            for (int j = 0; j < m.size_n; j++)
                m[i][j] *= x;
        return m;
    }

    matrix<Type>& operator*=(Type x) {
        for (int i = 0; i < size_m; i++)
            for (int j = 0; j < size_n; j++)
                mtx[i][j] *= x;
        return *this;
    }

    friend matrix<Type> operator^(matrix<Type> m1, matrix<Type> m2) {
        if (m1.sizes() == m2.sizes()) {
            int s_m = m1.size_m;
            int s_n = m1.size_n;
            matrix<Type> c(s_m, s_n);

            for (int i = 0; i < s_m; i++)
                for (int j = 0; j < s_n; j++)
                    c[i][j] = m1[i][j] * m2[i][j];
            return c;
        }
        else if (m1.size_m == m2.size_m) {
            int s_m = m1.size_m;
            int s_n = m1.size_n;
            matrix<Type> c(s_m, s_n);

            for (int i = 0; i < s_m; i++)
                for (int j = 0; j < s_n; j++)
                    c[i][j] = m1[i][j] * m2[i][0];
            return c;
        }
        else {
            cout << "Matrix multiplication error:\n\tmatrix sizes do not match!" << endl;
            abort();
        }
    }

    matrix<Type>& operator^=(matrix<Type> m) {
        if (sizes() == m.sizes()) {
            for (int i = 0; i < size_m; i++)
                for (int j = 0; j < size_n; j++)
                    mtx[i][j] *= m[i][j];
            return *this;
        }
        else if (size_m == m.size_m) {
            for (int i = 0; i < size_m; i++)
                for (int j = 0; j < size_n; j++)
                    mtx[i][j] *= m[i][0];
            return *this;
        }
        else {
            cout << "Matrix multiplication error:\n\tmatrix sizes do not match!" << endl;
            abort();
        }
    }

    friend matrix<Type> operator/(matrix<Type> m1, matrix<Type> m2) {
        if (m1.sizes() == m2.sizes()) {
            int m = m1.sizes()[0];
            int n = m1.sizes()[1];
            matrix<Type> c(m, n);
            for (int i = 0; i < m; i++)
                for (int j = 0; j < n; j++)
                    c[i][j] = m1[i][j] / m2[i][j];
            return c;
        }
        else if (m1.size_m == m2.size_m) {
            int m = m1.size_m;
            int n = m1.size_n;
            matrix<Type> c(m, n);
            for (int i = 0; i < m; i++)
                for (int j = 0; j < n; j++)
                    c[i][j] = m1[i][j] / m2[i][0];
            return c;
        }
        else {
            cout << "Error matrix:\n\tmatrix sizes do not match!" << endl;
            abort();
        }
    }

    friend matrix<Type> operator/(matrix<Type> m, Type x) {
        for (int i = 0; i < m.size_m; i++)
            for (int j = 0; j < m.size_n; j++)
                m[i][j] /= x;
        return m;
    }

    friend matrix<Type> operator/(Type x, matrix<Type> m) {
        for (int i = 0; i < m.size_m; i++)
            for (int j = 0; j < m.size_n; j++)
                m[i][j] = x / m[i][j];
        return m;
    }

    matrix<Type>& operator/=(matrix<Type> m) {
        if (sizes() == m.sizes()) {
            for (int i = 0; i < size_m; i++)
                for (int j = 0; j < size_n; j++)
                    mtx[i][j] / m[i][j];
            return *this;
        }
        else if (size_m == m.size_m) {
            for (int i = 0; i < size_m; i++)
                for (int j = 0; j < size_n; j++)
                    mtx[i][j] /= m[i][0];
            return *this;
        }
        else {
            cout << "Error matrix:\n\tmatrix sizes do not match!" << endl;
            abort();
        }
    }

    matrix<Type>& operator/=(Type x) {
        for (int i = 0; i < size_m; i++)
            for (int j = 0; j < size_n; j++)
                mtx[i][j] /= x;
        return *this;
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

    friend matrix<bool> operator==(matrix<Type> m1, matrix<Type> m2) {
        if (m1.size_m+m1.size_n == m2.size_m+m2.size_n) {
            int s_m = m1.size_m;
            int s_n = m1.size_n;
            matrix<bool> c(s_m, s_n);

            for (int i = 0; i < s_m; i++)
                for (int j = 0; j < s_n; j++)
                    c[i][j] = m1[i][j] == m2[i][j];
            return c;
        } else {
            cout << "Error:\n\tmatrix sizes do not match!" << endl;
            abort();
        }
    }

    friend matrix<bool> operator==(matrix<Type> m1, Type x) {
        int s_m = m1.size_m;
        int s_n = m1.size_n;
        matrix<bool> c(s_m, s_n);

        for (int i = 0; i < s_m; i++)
            for (int j = 0; j < s_n; j++)
                c[i][j] = m1[i][j] == x;
        return c;
    }

    friend matrix<bool> operator==(Type x, matrix<Type> m1) {
        int s_m = m1.size_m;
        int s_n = m1.size_n;
        matrix<bool> c(s_m, s_n);

        for (int i = 0; i < s_m; i++)
            for (int j = 0; j < s_n; j++)
                c[i][j] = x == m1[i][j];
        return c;
    }

    friend matrix<bool> operator!=(matrix<Type> m1, matrix<Type> m2) {
        if (m1.size_m+m1.size_n == m2.size_m+m2.size_n) {
            int s_m = m1.size_m;
            int s_n = m1.size_n;
            matrix<bool> c(s_m, s_n);

            for (int i = 0; i < s_m; i++)
                for (int j = 0; j < s_n; j++)
                    c[i][j] = m1[i][j] != m2[i][j];
            return c;
        } else {
            cout << "Error:\n\tmatrix sizes do not match!" << endl;
            abort();
        }
    }

    friend matrix<bool> operator!=(matrix<Type> m1, Type x) {
        int s_m = m1.size_m;
        int s_n = m1.size_n;
        matrix<bool> c(s_m, s_n);

        for (int i = 0; i < s_m; i++)
            for (int j = 0; j < s_n; j++)
                c[i][j] = m1[i][j] != x;
        return c;
    }

    friend matrix<bool> operator!=(Type x, matrix<Type> m1) {
        int s_m = m1.size_m;
        int s_n = m1.size_n;
        matrix<bool> c(s_m, s_n);

        for (int i = 0; i < s_m; i++)
            for (int j = 0; j < s_n; j++)
                c[i][j] = x != m1[i][j];
        return c;
    }

    friend matrix<bool> operator>(matrix<Type> m1, matrix<Type> m2) {
        if (m1.size_m+m1.size_n == m2.size_m+m2.size_n) {
            int s_m = m1.size_m;
            int s_n = m1.size_n;
            matrix<bool> c(s_m, s_n);

            for (int i = 0; i < s_m; i++)
                for (int j = 0; j < s_n; j++)
                    c[i][j] = m1[i][j] > m2[i][j];
            return c;
        } else {
            cout << "Error:\n\tmatrix sizes do not match!" << endl;
            abort();
        }
    }

    friend matrix<bool> operator>(matrix<Type> m1, Type x) {
        int s_m = m1.size_m;
        int s_n = m1.size_n;
        matrix<bool> c(s_m, s_n);

        for (int i = 0; i < s_m; i++)
            for (int j = 0; j < s_n; j++)
                c[i][j] = m1[i][j] > x;
        return c;
    }

    friend matrix<bool> operator>(Type x, matrix<Type> m1) {
        int s_m = m1.size_m;
        int s_n = m1.size_n;
        matrix<bool> c(s_m, s_n);

        for (int i = 0; i < s_m; i++)
            for (int j = 0; j < s_n; j++)
                c[i][j] = x > m1[i][j];
        return c;
    }

    friend matrix<bool> operator<(matrix<Type> m1, matrix<Type> m2) {
        if (m1.size_m+m1.size_n == m2.size_m+m2.size_n) {
            int s_m = m1.size_m;
            int s_n = m1.size_n;
            matrix<bool> c(s_m, s_n);

            for (int i = 0; i < s_m; i++)
                for (int j = 0; j < s_n; j++)
                    c[i][j] = m1[i][j] < m2[i][j];
            return c;
        } else {
            cout << "Error:\n\tmatrix sizes do not match!" << endl;
            abort();
        }
    }

    friend matrix<bool> operator<(matrix<Type> m1, Type x) {
        int s_m = m1.size_m;
        int s_n = m1.size_n;
        matrix<bool> c(s_m, s_n);

        for (int i = 0; i < s_m; i++)
            for (int j = 0; j < s_n; j++)
                c[i][j] = m1[i][j] < x;
        return c;
    }

    friend matrix<bool> operator<(Type x, matrix<Type> m1) {
        int s_m = m1.size_m;
        int s_n = m1.size_n;
        matrix<bool> c(s_m, s_n);

        for (int i = 0; i < s_m; i++)
            for (int j = 0; j < s_n; j++)
                c[i][j] = x < m1[i][j];
        return c;
    }

    friend matrix<bool> operator>=(matrix m1, matrix m2) {
        if (m1.size_m+m1.size_n == m2.size_m+m2.size_n) {
            int s_m = m1.size_m;
            int s_n = m1.size_n;
            matrix<bool> c(s_m, s_n);

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
        int s_m = m1.size_m;
        int s_n = m1.size_n;
        matrix<bool> c(s_m, s_n);
        for (int i = 0; i < s_m; i++)
            for (int j = 0; j < s_n; j++)
                c[i][j] = m1[i][j] >= x;
        return c;
    }

    friend matrix<bool> operator>=(Type x, matrix<Type> m1) {
        int s_m = m1.size_m;
        int s_n = m1.size_n;
        matrix<bool> c(s_m, s_n);

        for (int i = 0; i < s_m; i++)
            for (int j = 0; j < s_n; j++)
                c[i][j] = x >= m1[i][j];
        return c;
    }

    friend matrix<bool> operator<=(matrix m1, matrix m2) {
        if (m1.size_m+m1.size_n == m2.size_m+m2.size_n) {
            int s_m = m1.size_m;
            int s_n = m1.size_n;
            matrix<bool> c(s_m, s_n);

            for (int i = 0; i < s_m; i++)
                for (int j = 0; j < s_n; j++)
                    c[i][j] = m1[i][j] <= m2[i][j];
            return c;
        } else {
            cout << "Error:\n\tmatrix sizes do not match!" << endl;
            abort();
        }
    }

    friend matrix<bool> operator<=(matrix m1, Type x) {
        int s_m = m1.size_m;
        int s_n = m1.size_n;
        matrix<bool> c(s_m, s_n);
        for (int i = 0; i < s_m; i++)
            for (int j = 0; j < s_n; j++)
                c[i][j] = m1[i][j] <= x;
        return c;
    }

    friend matrix<bool> operator<=(Type x, matrix<Type> m1) {
        int s_m = m1.size_m;
        int s_n = m1.size_n;
        matrix<bool> c(s_m, s_n);

        for (int i = 0; i < s_m; i++)
            for (int j = 0; j < s_n; j++)
                c[i][j] = x <= m1[i][j];
        return c;
    }

    explicit operator matrix<bool>() {
        matrix<bool> c(size_m, size_n);
        for (int i = 0; i < size_m; i++)
            for (int j = 0; j < size_n; j++)
                c[i][j] = mtx[i][j];
        return c;
    }

    explicit operator matrix<int>() {
        matrix<int> c(size_m, size_n);
        for (int i = 0; i < size_m; i++)
            for (int j = 0; j < size_n; j++)
                c[i][j] = mtx[i][j];
        return c;
    }

    explicit operator matrix<double>() {
        matrix<double> c(size_m, size_n);
        for (int i = 0; i < size_m; i++)
            for (int j = 0; j < size_n; j++)
                c[i][j] = mtx[i][j];
        return c;
    }

    /**Получить матрицу из 2-йного вектора*/
    void get_matrix(vector<vector<Type>> m) {
        size_m = m.size();
        size_n = m.at(0).size();
        load_size(size_m, size_n);
        for (int i = 0; i < size_m; i++)
            for (int j = 0; j < size_n; j++)
                mtx[i][j] = m[i][j];
    }

    /** Заполнить матрицу целыми числами в определённом диапозоне */
    void randint(int from, int to) {
        if (size_m != 0) {
            srand(nano());
            for (int i = 0; i < size_m; i++)
                for (int j = 0; j < size_n; j++)
                    mtx[i][j] = from + rand() % to;
        } else {
            cout << "Error: the matrix is not defined" << endl;
            abort();
        }
    }

    /** Заполнить матрицу действительными числами в определённом диапозоне */
    void randf(double from = 0, double to = 1) {
        if (size_m != 0) {
            srand(nano());
            for (int i = 0; i < size_m; i++)
                for (int j = 0; j < size_n; j++)
                    mtx[i][j] = from + rand() / ((double) RAND_MAX / (to - from));
        } else {
            cout << "Error: the matrix is not defined" << endl;
            abort();
        }
    }

    void print() {
        cout << '[';

        for (int i = 0; i < size_m; i++) {
            if (i != 0)
                cout << ' ';
            cout << '[';
                for (int j = 0; j < size_n; j++) {
                cout << mtx[i][j];
                if (j + 1 < size_n)
                    cout << ", ";
            }

            cout << ']';
            if (i + 1 < size_m)
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

    /**e^x где x - каждый элемент матрицы*/
    matrix<Type> expa() {
        matrix c;
        c.load_size(size_m, size_n);
        for (int i = 0; i < size_m; i++)
            for (int j = 0; j < size_n; j++)
                c[i][j] = exp(mtx[i][j]);
        return c;
    }

    Type sum() {
        Type summ = 0;
        for (int i = 0; i < size_m; i++)
            for (int j = 0; j < size_n; j++)
                summ += mtx[i][j];
        return summ;
    }

    matrix<Type> sum_axis(int axis) {
        if (axis == 1) {
            matrix<Type> summ;
            for (int i = 0; i < size_m; i++) {
                matrix l = mtx[i];
                summ.push_back(l.sum());
            }
            return summ;
        }
        else {
            matrix summ = mtx[0];
            for (int i = 1; i < size_m; i++) {
                matrix l = mtx[i];
                summ += l;
            }
            return summ;
        }
    }

    /**Максимум из кадого столбца матрицы*/
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

    /**Транспонирование матрицы*/
    matrix<Type> T(){
        matrix c;
        c.load_size(size_n, size_m);
        for (int i = 0; i < size_m; i++)
            for (int j = 0; j < size_n; j++)
                c[j][i] = mtx[i][j];
        return c;
    };

    void push_back(Type x) {
        vector<Type> c = {x};
        mtx.push_back(c);
        size_m = mtx.size();
        size_n = mtx[0].size();

    }

    void push_back(vector<Type> x) {
        mtx.push_back(x);
        size_m = mtx.size();
        size_n = mtx[0].size();
    }

    int size() {
        return size_m;
    }

    vector<int> sizes() {
        vector<int> c  = {size_m, size_n};
        return c;
    }

    void shuffle_matrix() {
        auto rng = default_random_engine();
        shuffle(mtx.begin(), mtx.end(), rng);
    }

    /**Получить срез матрицы*/
    matrix<Type> slice(int start, int end) {
        matrix<Type> c;
        for (int i = start; i < end; i++)
            c.push_back(mtx[i]);
        return c;
    }

    /**Получить матрицы из элементов каждого столбца*/
    template<class T>
    matrix<matrix<T>> split() {
        matrix<matrix<T>> c(1, size_n);
        for (int i = 0; i < size_n; i++)   {
            matrix<T> mat;
            for (int j = 0; j < size_m; j++)
                mat.push_back(mtx[j][i]);
            c[0][i] = mat;
        }
        return c;
    }


private:
    vector<vector<Type>> mtx;
    int size_m = 0, size_n = 0;
};

/**Фунция активации RELU*/
template <class T>
matrix<T> relu(matrix<T> mtx) {
    matrix c = mtx;
    return maximum(c, 0.);
}

template <class T>
matrix<T> softmax(matrix<T> mtx) {
    matrix out = mtx.expa();
    return out/out.sum();
}

template <class T>
matrix<T> softmax_batch(matrix<T> mtx) {
    matrix out = mtx.expa();
    return out/out.sum_axis(1);
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
    matrix y_full = zeros(1, num_classes);
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