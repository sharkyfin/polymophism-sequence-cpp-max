# Сборка

```bash
cmake -S . -B build
cmake --build build
```

```bash
cmake --build build --target main
cmake --build build --target tests
cmake --build build --target gui
```

Тесты:

```bash
ctest --test-dir build --output-on-failure
```

Запуск:

```bash
./build/main
./build/gui
```

Windows:

```bat
cmake --build build --config Debug --target main
cmake --build build --config Debug --target tests
cmake --build build --config Debug --target gui
ctest --test-dir build -C Debug --output-on-failure
build\Debug\main.exe
build\Debug\gui.exe
```

# Архитектура `Deque` и матриц

## Ключевая идея

Проект разделен на слои:

- `SegmentedBuffer<T>` — низкоуровневое сегментированное хранилище для обоих структур данных
- `Deque<T>` — кольцевой сегментированный deque
- `RectangularMatrix<T>` — прямоугольная матрица

---

## Связи типов

### Композиция

```text
Deque<T>
└── SegmentedBuffer<T>
    └── DynamicArray<DynamicArray<T>>

RectangularMatrix<T>
└── SegmentedBuffer<T>
    └── DynamicArray<DynamicArray<T>>

TriangularMatrix<T>
└── SquareMatrix<T>

DiagonalMatrix<T>
└── SquareMatrix<T>

Vector<T>
└── DynamicArray<T>
```

### Наследование

```text
SquareMatrix<T> : RectangularMatrix<T>

Sequence<T>
└── DequeSequenceBase<T>
    ├── MutableDequeSequence<T>
    └── ImmutableDequeSequence<T>
```

---

## `SegmentedBuffer<T>`

Файл: [segmented_buffer.hpp](core/segmented_buffer.hpp)

Хранит:

- `DynamicArray<DynamicArray<T>> segments`
- `int segmentSize`

Умеет:

- создать карту сегментов
- выделить сегмент
- очистить сегмент
- вернуть сегмент
- вернуть указатель на данные сегмента
- `Swap`

---

## `Deque<T>`

Файл: [deque.hpp](deque/deque.hpp)

Хранит:

- `SegmentedBuffer<T> buffer`
- индекс первого сегмента deque
- смещение первого элемента внутри первого сегмента
- `int length`

### Адресация элемента

Для логического индекса:

```cpp
linearOffset         = firstSegmentOffset + logicalIndex
segmentOffset        = linearOffset / segmentSize
physicalSegmentIndex = GetCircularSegmentIndex(firstSegmentIndex + segmentOffset)
segmentOffsetInside  = linearOffset % segmentSize
```

### Рост

`GrowSegmentMap()`:

1. создает новый буфер
2. переносит используемые сегменты в логическом порядке
3. ставит индекс первого сегмента в центр новой кольцевой таблицы сегментов

### Сложность

- `Get` / `Set` — `O(1)`
- `Append` — амортизированно `O(1)`
- `Prepend` — амортизированно `O(1)`
- `PopFront` / `PopBack` — амортизированно `O(1)`

### Пример

```cpp
Deque<int> deque;
deque.Append(10);
deque.Append(20);
deque.Prepend(5);
// [5, 10, 20]

deque.PopFront();
// [10, 20]
```

---

## `RectangularMatrix<T>`

Файл: [rectangular_matrix.hpp](matrix/rectangular_matrix.hpp)

Хранит:

- `int rows`
- `int cols`
- `SegmentedBuffer<T> data`

### Сложность

- `Get` / `Set` — `O(1)`
- `SwapRows` — `O(1)` по строкам
- `GetRow` — `O(cols)`
- `GetColumn` — `O(rows)`

### Пример

```cpp
RectangularMatrix<double> matrix(2, 3, 0.0);
matrix.Set(0, 0, 1.0);
matrix.Set(0, 1, 2.0);
matrix.Set(0, 2, 3.0);

matrix[1][0] = 4.0;
matrix[1][1] = 5.0;
matrix[1][2] = 6.0;
```

---

## `SquareMatrix<T>`

Файл: [square_matrix.hpp](matrix/square_matrix.hpp)

```cpp
class SquareMatrix : public RectangularMatrix<T>
```

Добавляет:

- `GetSize()`
- `Identity(size)` - единичная матрица
- `Trace()` - след матрицы

---

## `TriangularMatrix<T>`

Файл: [special_matrices.hpp](matrix/special_matrices.hpp)

Использует composition:

```cpp
SquareMatrix<T> data;
TriangleKind kind;
T zero;
```

Семантика:

- `Get(row, col)` вне треугольника возвращает `zero`
- `Set(row, col, value)` вне треугольника бросает исключение

Разрешенные типы:

- `TriangleKind::Lower`
- `TriangleKind::Upper`

---

## `DiagonalMatrix<T>`

Файл: [special_matrices.hpp](matrix/special_matrices.hpp)

Использует composition:

```cpp
SquareMatrix<T> data;
T zero;
```

Семантика:

- `Get(i, j)` при `i != j` возвращает `zero`
- изменяются только диагональные элементы

---

## Матричные алгоритмы

Файлы:

- [matrix_algorithms.hpp](matrix/matrix_algorithms.hpp)
- [matrix_solvers.hpp](matrix/matrix_solvers.hpp)

используются указатели на строки:

```cpp
const T* row = matrix[rowIndex];
```

---
