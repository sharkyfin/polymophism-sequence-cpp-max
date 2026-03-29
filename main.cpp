#include <iostream>
#include <string>

#include "exceptions.hpp"
#include "immutable_array_sequence.hpp"
#include "immutable_deque_sequence.hpp"
#include "immutable_list_sequence.hpp"
#include "mutable_array_sequence.hpp"
#include "mutable_deque_sequence.hpp"
#include "mutable_list_sequence.hpp"
#include "pair.hpp"
#include "sequence_utils.hpp"
#include "tests.hpp"

int ReadInt(const std::string& prompt) {
    std::cout << prompt;
    int value = 0;
    while (!(std::cin >> value)) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Некорректный ввод. Повторите: ";
    }
    return value;
}

void PrintSequence(const Sequence<int>* sequence) {
    if (sequence == nullptr) {
        std::cout << "Последовательность не создана.\n";
        return;
    }

    std::cout << "Длина: " << sequence->GetLength() << "\n";
    std::cout << "Элементы: " << *sequence << "\n";
}

void PrintPairSequence(const Sequence<Pair<int, int>>* sequence) {
    if (sequence == nullptr) {
        std::cout << "Последовательность пар не создана.\n";
        return;
    }

    std::cout << "Длина: " << sequence->GetLength() << "\n";
    std::cout << "Элементы: " << *sequence << "\n";
}

Sequence<int>* CreateSequenceByType(int type) {
    int count = ReadInt("Введите количество элементов: ");
    if (count < 0) {
        throw InvalidArgumentException("Количество элементов не может быть отрицательным");
    }

    int* items = nullptr;
    if (count > 0) {
        items = new int[count];
        for (int i = 0; i < count; ++i) {
            items[i] = ReadInt("Элемент [" + std::to_string(i) + "]: ");
        }
    }

    Sequence<int>* result = nullptr;
    try {
        switch (type) {
            case 1:
                result = new MutableArraySequence<int>(items, count);
                break;
            case 2:
                result = new ImmutableArraySequence<int>(items, count);
                break;
            case 3:
                result = new MutableListSequence<int>(items, count);
                break;
            case 4:
                result = new ImmutableListSequence<int>(items, count);
                break;
            case 5:
                result = new MutableDequeSequence<int>(items, count);
                break;
            case 6:
                result = new ImmutableDequeSequence<int>(items, count);
                break;
            default:
                throw InvalidArgumentException("Неизвестный тип последовательности");
        }
    } catch (...) {
        delete[] items;
        throw;
    }

    delete[] items;
    return result;
}

void ReplaceSequenceIfNeeded(Sequence<int>*& current, Sequence<int>* result) {
    if (result == nullptr) {
        throw InvalidArgumentException("Операция вернула пустой указатель");
    }

    if (result != current) {
        delete current;
        current = result;
    }
}

Sequence<int>* PopFrontIfDeque(Sequence<int>* current) {
    if (MutableDequeSequence<int>* mutableDeque = dynamic_cast<MutableDequeSequence<int>*>(current)) {
        return mutableDeque->PopFront();
    }

    if (ImmutableDequeSequence<int>* immutableDeque = dynamic_cast<ImmutableDequeSequence<int>*>(current)) {
        return immutableDeque->PopFront();
    }

    throw InvalidArgumentException("Операция PopFront доступна только для deque");
}

Sequence<int>* PopBackIfDeque(Sequence<int>* current) {
    if (MutableDequeSequence<int>* mutableDeque = dynamic_cast<MutableDequeSequence<int>*>(current)) {
        return mutableDeque->PopBack();
    }

    if (ImmutableDequeSequence<int>* immutableDeque = dynamic_cast<ImmutableDequeSequence<int>*>(current)) {
        return immutableDeque->PopBack();
    }

    throw InvalidArgumentException("Операция PopBack доступна только для deque");
}

void PrintSequenceTypeOptions() {
    std::cout << "1 - MutableArraySequence, 2 - ImmutableArraySequence, "
              << "3 - MutableListSequence, 4 - ImmutableListSequence, "
              << "5 - MutableDequeSequence, 6 - ImmutableDequeSequence\n";
}

void PrintCreationMenu() {
    std::cout << "\n===== ВЫБОР ТИПА ПОСЛЕДОВАТЕЛЬНОСТИ =====\n";
    std::cout << "1. Создать MutableArraySequence\n";
    std::cout << "2. Создать ImmutableArraySequence\n";
    std::cout << "3. Создать MutableListSequence\n";
    std::cout << "4. Создать ImmutableListSequence\n";
    std::cout << "5. Создать MutableDequeSequence\n";
    std::cout << "6. Создать ImmutableDequeSequence\n";
    std::cout << "7. Запустить все тесты\n";
    std::cout << "0. Выход\n";
}

void PrintOperationsMenu() {
    std::cout << "\n===== ОПЕРАЦИИ С ТЕКУЩЕЙ ПОСЛЕДОВАТЕЛЬНОСТЬЮ =====\n";
    std::cout << "1. Показать элементы\n";
    std::cout << "2. Append элемент\n";
    std::cout << "3. Prepend элемент\n";
    std::cout << "4. InsertAt\n";
    std::cout << "5. operator[] / получить элемент\n";
    std::cout << "6. Получить подпоследовательность\n";
    std::cout << "7. Конкатенировать последовательности\n";
    std::cout << "8. Сравнить последовательности (==, !=)\n";
    std::cout << "9. Map\n";
    std::cout << "10. Where\n";
    std::cout << "11. Reduce\n";
    std::cout << "12. Find\n";
    std::cout << "13. Split\n";
    std::cout << "14. Slice(index, count)\n";
    std::cout << "15. Slice(index, count, replacement)\n";
    std::cout << "16. Zip с другой последовательностью\n";
    std::cout << "17. Unzip последнего zip\n";
    std::cout << "18. Сменить тип последовательности\n";
    std::cout << "19. PopFront (только для deque)\n";
    std::cout << "20. PopBack (только для deque)\n";
    std::cout << "21. Запустить все тесты\n";
    std::cout << "0. Выход\n";
}

int main() {
    Sequence<int>* current = nullptr;
    Sequence<Pair<int, int>>* zippedPairs = nullptr;
    bool running = true;

    while (running) {
        try {
            if (current == nullptr) {
                PrintCreationMenu();
                int createCommand = ReadInt("Выберите пункт: ");
                switch (createCommand) {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6: {
                        current = CreateSequenceByType(createCommand);
                        std::cout << "Последовательность создана.\n";
                        break;
                    }
                    case 7: {
                        bool ok = RunAllTests();
                        if (ok) {
                            std::cout << "Все тесты пройдены.\n";
                        } else {
                            std::cout << "Есть упавшие тесты.\n";
                        }
                        break;
                    }
                    case 0: {
                        running = false;
                        break;
                    }
                    default:
                        std::cout << "Неизвестный пункт меню.\n";
                        break;
                }
                continue;
            }

            PrintOperationsMenu();
            int command = ReadInt("Выберите пункт: ");
            switch (command) {
                case 1: {
                    PrintSequence(current);
                    break;
                }
                case 2: {
                    int value = ReadInt("Введите элемент для Append: ");
                    Sequence<int>* result = current->Append(value);
                    ReplaceSequenceIfNeeded(current, result);
                    std::cout << "Append выполнен.\n";
                    break;
                }
                case 3: {
                    int value = ReadInt("Введите элемент для Prepend: ");
                    Sequence<int>* result = current->Prepend(value);
                    ReplaceSequenceIfNeeded(current, result);
                    std::cout << "Prepend выполнен.\n";
                    break;
                }
                case 4: {
                    int value = ReadInt("Введите элемент: ");
                    int index = ReadInt("Введите индекс: ");
                    Sequence<int>* result = current->InsertAt(value, index);
                    ReplaceSequenceIfNeeded(current, result);
                    std::cout << "InsertAt выполнен.\n";
                    break;
                }
                case 5: {
                    int index = ReadInt("Введите индекс: ");
                    std::cout << "Элемент (operator[]): " << (*current)[index] << "\n";
                    break;
                }
                case 6: {
                    int startIndex = ReadInt("Введите startIndex: ");
                    int endIndex = ReadInt("Введите endIndex: ");
                    Sequence<int>* sub = current->GetSubsequence(startIndex, endIndex);
                    std::cout << "Подпоследовательность: " << *sub << "\n";
                    delete sub;
                    break;
                }
                case 7: {
                    std::cout << "Создайте вторую последовательность для конкатенации:\n";
                    PrintSequenceTypeOptions();
                    int type = ReadInt("Введите тип: ");
                    Sequence<int>* other = CreateSequenceByType(type);
                    try {
                        Sequence<int>* result = *current + *other;
                        delete other;
                        delete current;
                        current = result;
                    } catch (...) {
                        delete other;
                        throw;
                    }
                    std::cout << "Concat (operator+) выполнен.\n";
                    break;
                }
                case 8: {
                    std::cout << "Создайте вторую последовательность для сравнения:\n";
                    PrintSequenceTypeOptions();
                    int type = ReadInt("Введите тип: ");
                    Sequence<int>* other = CreateSequenceByType(type);
                    bool eq = (*current == *other);
                    bool neq = (*current != *other);
                    std::cout << "current == other: " << (eq ? "true" : "false") << "\n";
                    std::cout << "current != other: " << (neq ? "true" : "false") << "\n";
                    delete other;
                    break;
                }
                case 9: {
                    std::cout << "Map: 1) x*2  2) x*x  3) x+10\n";
                    int mapType = ReadInt("Выберите тип map: ");
                    Sequence<int>* mapped = nullptr;
                    if (mapType == 1) {
                        mapped = current->Map([](int value) { return value * 2; });
                    } else if (mapType == 2) {
                        mapped = current->Map([](int value) { return value * value; });
                    } else if (mapType == 3) {
                        mapped = current->Map([](int value) { return value + 10; });
                    } else {
                        throw InvalidArgumentException("Неизвестный тип map");
                    }
                    ReplaceSequenceIfNeeded(current, mapped);
                    std::cout << "Map выполнен.\n";
                    break;
                }
                case 10: {
                    std::cout << "Where: 1) четные  2) больше заданного\n";
                    int whereType = ReadInt("Выберите тип where: ");
                    Sequence<int>* filtered = nullptr;
                    if (whereType == 1) {
                        filtered = current->Where([](int value) { return value % 2 == 0; });
                    } else if (whereType == 2) {
                        int threshold = ReadInt("Введите порог: ");
                        filtered = current->Where([threshold](int value) { return value > threshold; });
                    } else {
                        throw InvalidArgumentException("Неизвестный тип where");
                    }
                    ReplaceSequenceIfNeeded(current, filtered);
                    std::cout << "Where выполнен.\n";
                    break;
                }
                case 11: {
                    std::cout << "Reduce: 1) сумма  2) максимум\n";
                    int reduceType = ReadInt("Выберите тип reduce: ");
                    int reduced = 0;
                    if (reduceType == 1) {
                        reduced = current->Reduce([](int left, int right) { return left + right; });
                    } else if (reduceType == 2) {
                        reduced = current->Reduce([](int left, int right) {
                            return (left > right) ? left : right;
                        });
                    } else {
                        throw InvalidArgumentException("Неизвестный тип reduce");
                    }
                    std::cout << "Результат reduce: " << reduced << "\n";
                    break;
                }
                case 12: {
                    int target = ReadInt("Введите искомый элемент: ");
                    int index = current->Find([target](int value) { return value == target; });
                    if (index == -1) {
                        std::cout << "Элемент не найден.\n";
                    } else {
                        std::cout << "Элемент найден по индексу: " << index << "\n";
                    }
                    break;
                }
                case 13: {
                    int delimiter = ReadInt("Введите значение-разделитель: ");
                    int keep = ReadInt("Сохранять пустые части? (1 - да, 0 - нет): ");
                    bool keepEmpty = (keep == 1);
                    DynamicArray<Sequence<int>*> parts =
                        current->Split([delimiter](int value) { return value == delimiter; }, keepEmpty);

                    std::cout << "Количество частей: " << parts.GetSize() << "\n";
                    for (int i = 0; i < parts.GetSize(); ++i) {
                        std::cout << "Часть " << i << ": " << *parts.Get(i) << "\n";
                        delete parts.Get(i);
                    }
                    break;
                }
                case 14: {
                    int index = ReadInt("Введите index: ");
                    int count = ReadInt("Введите count: ");
                    Sequence<int>* result = current->Slice(index, count);
                    ReplaceSequenceIfNeeded(current, result);
                    std::cout << "Slice(index, count) выполнен.\n";
                    break;
                }
                case 15: {
                    int index = ReadInt("Введите index: ");
                    int count = ReadInt("Введите count: ");

                    int replacementCount = ReadInt("Введите количество элементов replacement: ");
                    if (replacementCount < 0) {
                        throw InvalidArgumentException("Количество replacement-элементов не может быть отрицательным");
                    }

                    MutableArraySequence<int> replacement;
                    for (int i = 0; i < replacementCount; ++i) {
                        int value = ReadInt("replacement[" + std::to_string(i) + "]: ");
                        replacement.Append(value);
                    }

                    Sequence<int>* result = current->Slice(index, count, &replacement);
                    ReplaceSequenceIfNeeded(current, result);
                    std::cout << "Slice(index, count, replacement) выполнен.\n";
                    break;
                }
                case 16: {
                    std::cout << "Создайте вторую последовательность для Zip:\n";
                    PrintSequenceTypeOptions();
                    int type = ReadInt("Введите тип: ");
                    Sequence<int>* other = CreateSequenceByType(type);

                    Sequence<Pair<int, int>>* newZippedPairs = nullptr;
                    try {
                        newZippedPairs = Zip(*current, *other);
                        delete other;
                    } catch (...) {
                        delete other;
                        delete newZippedPairs;
                        throw;
                    }

                    delete zippedPairs;
                    zippedPairs = newZippedPairs;

                    std::cout << "Zip выполнен:\n";
                    PrintPairSequence(zippedPairs);
                    break;
                }
                case 17: {
                    if (zippedPairs == nullptr) {
                        throw InvalidArgumentException("Сначала выполните Zip");
                    }

                    Pair<Sequence<int>*, Sequence<int>*> unzipped = Unzip(*zippedPairs);
                    std::cout << "Unzip результат (first): " << *unzipped.first << "\n";
                    std::cout << "Unzip результат (second): " << *unzipped.second << "\n";
                    delete unzipped.first;
                    delete unzipped.second;
                    break;
                }
                case 18: {
                    std::cout << "Выберите новый тип последовательности:\n";
                    PrintSequenceTypeOptions();
                    int type = ReadInt("Введите тип: ");
                    Sequence<int>* created = CreateSequenceByType(type);
                    delete current;
                    current = created;
                    std::cout << "Тип последовательности изменен.\n";
                    break;
                }
                case 19: {
                    Sequence<int>* result = PopFrontIfDeque(current);
                    ReplaceSequenceIfNeeded(current, result);
                    std::cout << "PopFront выполнен.\n";
                    break;
                }
                case 20: {
                    Sequence<int>* result = PopBackIfDeque(current);
                    ReplaceSequenceIfNeeded(current, result);
                    std::cout << "PopBack выполнен.\n";
                    break;
                }
                case 21: {
                    bool ok = RunAllTests();
                    if (ok) {
                        std::cout << "Все тесты пройдены.\n";
                    } else {
                        std::cout << "Есть упавшие тесты.\n";
                    }
                    break;
                }
                case 0: {
                    running = false;
                    break;
                }
                default:
                    std::cout << "Неизвестный пункт меню.\n";
                    break;
            }
        } catch (const LabException& e) {
            std::cout << "Ошибка: " << e.GetMessage() << "\n";
        } catch (...) {
            std::cout << "Неожиданная ошибка\n";
        }
    }

    delete current;
    delete zippedPairs;
    return 0;
}
