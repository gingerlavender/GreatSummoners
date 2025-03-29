#ifndef MATRIX_HPP
#define MATRIX_HPP

#include "MatrixIterators.hpp"

/**
 * \file Matrix.hpp
 * \brief Реализация шаблонного класса матрицы.
 */

/**
 * \class Matrix
 * \brief Представляет матрицу.
 * 
 * \tparam T Тип элементов, хранящихся в матрице.
 * \tparam M Количество строк в матрице.
 * \tparam N Количество столбцов в матрице.
 */

template <class T, const size_t M, const size_t N>
class Matrix {
    public:
        /** \brief Типы данных для элементов и итераторов матрицы. */
        using value_type = T;                ///< Тип элементов в матрице
        using pointer = T*;                  ///< Указатель на элементы в матрице
        using const_pointer = const T*;      ///< Константный указатель на элементы в матрице
        using reference = T&;                ///< Ссылка на элемент в матрице
        using const_reference = const T&;    ///< Константная ссылка на элемент в матрице
        using difference_type = ptrdiff_t;   ///< Тип разности для итераторов
        using size_type = size_t;            ///< Тип для операций с размерам.
        using iterator = MatrixIterator<value_type, false>; ///< Изменяемый итератор
        using const_iterator = MatrixIterator<value_type, true>; ///< Константный итератор
        using rows_iterator = iterator;      ///< Итератор по строкам (изменяемый)
        using rows_const_iterator = const_iterator; ///< Итератор по строкам (константный)
        using columns_iterator = MatrixColumnIterator<value_type, false>; ///< Итератор по столбцам (изменяемый)
        using columns_const_iterator = MatrixColumnIterator<value_type, true>; ///< Итератор по столбцам (константный). 
        /**
        * \brief Конструктор по умолчанию.
        */
        Matrix() noexcept(std::is_nothrow_default_constructible_v<T>) = default;
        /**
        * \brief Конструктор копирования.
        * \param other Матрица, из которой выполняется копирование.
        */
        Matrix(const Matrix& other) requires std::copy_constructible<T> { std::copy(other.cbegin(), other.cend(), begin()); }
        /**
        * \brief Конструктор перемещения.
        * \param moved Матрица, из которой выполняется перемещение.
        */
        Matrix(Matrix&& moved) noexcept(std::is_nothrow_default_constructible_v<T>) : Matrix() { swap(moved); }
        /**
        * \brief Конструктор из списка инициализации.
        * \param il Список инициализации для создания матрицы.
        */
        Matrix(std::initializer_list<T> il) requires std::move_constructible<T> { std::move(il.begin(), il.end(), begin()); }
        /**
        * \brief Оператор присваивания (копирование).
        * \param other Матрица, из которой выполняется копирование.
        * \return Ссылка на текущую матрицу.
        */
        Matrix& operator=(const Matrix& other) requires std::copy_constructible<T> {
            if (this != &other) {
                std::copy(other.cbegin(), other.cend(), begin());
            }
            return *this;
        }
        /**
        * \brief Оператор присваивания (перемещение).
        * \param moved Матрица, из которой выполняется перемещение.
        * \return Ссылка на текущую матрицу.
        */
        Matrix& operator=(Matrix&& moved) noexcept {
            if (this != &moved) {
                swap(moved);
            }
            return *this;
        }
        /**
        * \brief Оператор присваивания из списка инициализации.
        * \param il Список инициализации для присваивания значений.
        * \return Ссылка на текущую матрицу.
        */
        Matrix& operator=(std::initializer_list<T> il) requires std::move_constructible<T> { std::move(il.begin(), il.end(), begin()); return *this; }
        /**
        * \brief Заполняет матрицу заданным значением.
        * \param val Значение для заполнения матрицы.
         */
        void fill(const value_type& val) { std::fill_n(begin(), size(), val); }
        /**
        * \brief Возвращает общий размер матрицы.
        * \return Количество элементов в матрице.
        */
        size_type size() const noexcept { return N * M; }
        /**
        * \brief Возвращает максимальный размер матрицы.
        * \return Максимальное количество элементов в матрице.
        */
        size_type max_size() const noexcept { return N * M; }
        /**
        * \brief Проверяет, пуста ли матрица.
        * \return true, если матрица пуста, иначе false.
        */
        bool empty() const noexcept { return size() == 0; }
        /**
        * \brief Меняет содержимое текущей матрицы с другой.
        * \param other Матрица для обмена данными.
        */
        void swap(Matrix& other) noexcept { std::swap_ranges(begin(), end(), other.begin()); }
        /**
        * \brief Доступ к элементу по строке и столбцу с проверкой границ.
        * \param i Индекс строки.
        * \param j Индекс столбца.
        * \return Ссылка на элемент в указанной позиции.
        * \throw std::out_of_range Если индексы выходят за границы.
        */
        reference at(size_t i, size_t j) {
            if (i >= M || j >= N) {
                throw std::out_of_range("No such element!");
            }
            return matrix_[N * i + j];
        }
        /**
        * \brief Доступ к элементу по строке и столбцу с проверкой границ (константная версия).
        * \param i Индекс строки.
        * \param j Индекс столбца.
        * \return Константная ссылка на элемент в указанной позиции.
        * \throw std::out_of_range Если индексы выходят за границы.
        */
        const_reference at(size_t i, size_t j) const {
            if (i >= M || j >= N) {
                throw std::out_of_range("No such element!");
            }
            return matrix_[N * i + j];
        }
        /**
        * \brief Трехстороннее сравнение матриц.
        * 
        * Сравнивает элементы двух матриц лексикографически.
        * \param one Первая матрица.
        * \param two Вторая матрица.
        * \return Результат трехстороннего сравнения:
        * - std::strong_ordering::less, если первая матрица меньше второй.
        * - std::strong_ordering::equal, если матрицы равны.
        * - std::strong_ordering::greater, если первая матрица больше второй.
        */
        friend std::strong_ordering operator<=>(const Matrix<T, M, N>& one, const Matrix<T, M, N>& two) { 
            return std::lexicographical_compare_three_way(one.cbegin(), one.cend(), two.cbegin(), two.cend()); 
        }
        /**
        * \brief Проверка равенства двух матриц.
        * 
        * Проверяет, содержат ли две матрицы одинаковые элементы в одинаковом порядке.
        * \param one Первая матрица.
        * \param two Вторая матрица.
        * \return true, если матрицы равны, иначе false.
        */
        friend bool operator==(const Matrix<T, M, N>& one, const Matrix<T, M, N>& two) {
           return std::equal(one.cbegin(), one.cend(), two.cbegin()); 
        }
        /**
         * \brief Возвращает указатель на внутренние данные матрицы.
        * \return Указатель на первый элемент.
        */
        pointer data() { return std::addressof(matrix_[0]); }
        /**
        * \brief Возвращает константный указатель на внутренние данные матрицы.
        * \return Константный указатель на первый элемент.
        */
        const_pointer data() const { return std::addressof(matrix_[0]); }
        /**
        * \brief Возвращает итератор на начало матрицы.
        * \return Итератор на первый элемент.
        */
        iterator begin() noexcept { return iterator(std::addressof(matrix_[0])); }
        /**
        * \brief Возвращает итератор на начало указанной строки.
        * \param row Индекс строки.
        * \return Итератор на первый элемент строки.
        */
        rows_iterator row_begin(size_t row) noexcept { return rows_iterator(std::addressof(matrix_[row * N])); }
        /**
        * \brief Возвращает итератор на начало указанного столбца.
        * \param column Индекс столбца.
        * \return Итератор на первый элемент столбца.
        */
        columns_iterator column_begin(size_t column) noexcept { return columns_iterator(std::addressof(matrix_[column]), N); }
         /**
        * \brief Возвращает константный итератор на начало матрицы.
        * \return Константный итератор на первый элемент.
        */
        const_iterator cbegin() const noexcept { return const_iterator(std::addressof(matrix_[0])); }
        /**
        * \brief Возвращает константный итератор на начало указанной строки.
        * \param row Индекс строки.
        * \return Константный итератор на первый элемент строки.
        */
        rows_const_iterator row_cbegin(size_t row) const noexcept { return rows_const_iterator((std::addressof(matrix_[row * N]))); }
        /**
        * \brief Возвращает константный итератор на начало указанного столбца.
        * \param column Индекс столбца.
        * \return Константный итератор на первый элемент столбца.
        */
        columns_const_iterator column_cbegin(size_t column) const noexcept { return columns_const_iterator(std::addressof(matrix_[column]), N); }
        /**
        * \brief Возвращает итератор на конец матрицы.
        * \return Итератор на элемент, следующий за последним.
        */
        iterator end() noexcept { return iterator(std::addressof(matrix_[N * M])); }
        /**
        * \brief Возвращает итератор на конец указанной строки.
        * \param row Индекс строки.
        * \return Итератор на элемент, следующий за последним элементом строки.
        */
        rows_iterator row_end(size_t row) noexcept { return rows_iterator(std::addressof(matrix_[N * (row + 1)])); }
        /**
        * \brief Возвращает итератор на конец указанного столбца.
        * \param column Индекс столбца.
        * \return Итератор на элемент, следующий за последним элементом столбца.
        */
        columns_iterator column_end(size_t column) noexcept { return columns_iterator(std::addressof(matrix_[column + N * M]), N); }
        /**
        * \brief Возвращает константный итератор на конец матрицы.
        * \return Константный итератор на элемент, следующий за последним.
        */
        const_iterator cend() const noexcept { return const_iterator(std::addressof(matrix_[N * M])); }
        /**
        * \brief Возвращает константный итератор на конец указанной строки.
        * \param row Индекс строки.
        * \return Константный итератор на элемент, следующий за последним элементом строки.
        */
        rows_const_iterator row_cend(size_t row) const noexcept { return rows_const_iterator(std::addressof(matrix_[N * (row + 1)])); }
        /**
        * \brief Возвращает константный итератор на конец указанного столбца.
        * \param column Индекс столбца.
        * \return Константный итератор на элемент, следующий за последним элементом столбца.
        */
        columns_const_iterator column_cend(size_t column) const noexcept { return columns_const_iterator(std::addressof(matrix_[column + N * M]), N); }
        private:
            value_type matrix_[M * N]; ///< Внутренний массив для хранения элементов матрицы.
};

#endif
