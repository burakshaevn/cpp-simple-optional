# Упрощённый аналог класса `std::optional`

## Описание
Реализация упрощённого класса Optional, который имитирует поведение `std::optional` из стандартной библиотеки C++. Класс позволяет безопасно работать с объектами, которые могут содержать или не содержать значение, без необходимости явной проверки на наличие значения.

Основные возможности:
- Хранение и доступ к значению: Позволяет безопасно хранить, получать и изменять объект.
- Поддержка семантики перемещения и копирования: Полная поддержка операций перемещения и копирования объектов.
- Обработка ошибок: Генерирует исключение `BadOptionalAccess`, если производится попытка доступа к неинициализированному значению.
- Методы для управления объектом:
  - Emplace: Создание нового значения напрямую с передачей параметров конструктору.
  - Reset: Сброс текущего значения с разрушением объекта.
  - Операторы доступа `*` и `->`, а также метод `Value()` для получения значения.

## Ключевые методы
- `HasValue()`: Возвращает true, если объект содержит значение, и false, если пуст.
- `Value()`: Возвращает ссылку на объект, если он инициализирован, иначе бросает исключение BadOptionalAccess.
- Операторы * и ->: Доступ к объекту без явной проверки, если объект не пуст.
- `Emplace()`: Конструирует объект внутри Optional с передачей аргументов конструктору.
- `Reset()`: Очищает объект, освобождая память.

## Пример
```cpp
Optional<int> opt;
opt = 10;

if (opt.HasValue()) {
    std::cout << "Значение: " << *opt << std::endl;
} else {
    std::cout << "Optional пуст" << std::endl;
}

// Использование метода Value(), который бросает исключение, если Optional пуст
try {
    int value = opt.Value();
    std::cout << "Значение: " << value << std::endl;
} catch (const BadOptionalAccess& e) {
    std::cout << "Ошибка: " << e.what() << std::endl;
}
```
