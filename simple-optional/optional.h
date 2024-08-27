#include <stdexcept>
#include <utility>

// Исключение этого типа должно генерироватся при обращении к пустому optional
class BadOptionalAccess : public std::exception {
public:
    using exception::exception;

    virtual const char* what() const noexcept override {
        return "Bad optional access";
    }
};

template <typename T>
class Optional {
public:
    Optional() = default;
    Optional(const T& value) {
        ptr_ = new (&data_[0]) T(value);
        is_initialized_ = true;
    }
    Optional(T&& value) {
        ptr_ = new (&data_[0]) T(std::move(value));
        is_initialized_ = true;
    }
    Optional(const Optional& other) {
        if (other.is_initialized_) {
            ptr_ = new (&data_[0]) T(*other.ptr_);
            is_initialized_ = true;
        }
    }
    Optional(Optional&& other) noexcept {
        if (other.is_initialized_) {
            ptr_ = new (&data_[0]) T(std::move(*other.ptr_));
            is_initialized_ = true;
            //other.Reset();  
        }
    }

    // обработать две возможные ситуации: присваивание пустому и непустому Optional. 
    Optional& operator=(const T& value) {
        if (is_initialized_) {
            *ptr_ = value;  // Если объект инициализирован, копируем значение
        }
        else {
            ptr_ = new (&data_[0]) T(value);  // Если объект неинициализирован, создаем новое значение
            is_initialized_ = true;
        }
        return *this;
    }
    Optional& operator=(T&& rhs) {
        if (is_initialized_) {
            *ptr_ = std::move(rhs);  // Если объект инициализирован, перемещаем значение
        }
        else {
            ptr_ = new (&data_[0]) T(std::move(rhs));  // Если объект неинициализирован, создаем новое значение
            is_initialized_ = true;
        }
        return *this;
    }
    // четыре возможные ситуации: когда левый и правый аргументы операции присваивания могут быть как пустым, так и непустым Optional.
    Optional& operator=(const Optional& rhs) {
        if (this == &rhs) {
            return *this;
        }

        if (is_initialized_) {
            if (rhs.is_initialized_) {
                // Оба объекта инициализированы, копируем содержимое
                *ptr_ = *rhs.ptr_;
            }
            else {
                // Левый объект инициализирован, правый - нет, уничтожаем левый объект
                Reset();
            }
        }
        else {
            if (rhs.is_initialized_) {
                // Левый объект неинициализирован, правый - инициализирован, создаем новый объект
                ptr_ = new (&data_[0]) T(rhs.Value());
                is_initialized_ = true;
            }
        }
        return *this;
    }
    Optional& operator=(Optional&& rhs) noexcept {
        if (this != &rhs) {
            if (is_initialized_) {
                if (rhs.is_initialized_) {
                    // Оба объекта инициализированы, перемещаем содержимое
                    *ptr_ = std::move(*rhs.ptr_);
                }
                else {
                    // Левый объект инициализирован, правый - нет, уничтожаем левый объект
                    Reset();
                }
            }
            else {
                if (rhs.is_initialized_) {
                    // Левый объект неинициализирован, правый - инициализирован, создаем новый объект
                    ptr_ = new (&data_[0]) T(std::move(*rhs.ptr_));
                    is_initialized_ = true;
                }
            }
            //rhs.Reset();  // ломать rhs не нужно!
        }
        return *this;
    }

    ~Optional() {
        Reset();
    }

    bool HasValue() const {
        return is_initialized_;
    }

    // Операторы * и -> не должны делать никаких проверок на пустоту Optional.
    // Эти проверки остаются на совести программиста
    //T& operator*() {
    //    return *ptr_;
    //}
    /*const T& operator*() const {
        return *ptr_;
    }*/

    // Перегрузка по rvalue-ссылке сделана для того чтобы при вызове у временных объектов они возвращали rvalue-ссылку на содержимое Optional.
    T& operator*()& {
        return *ptr_;
    }
    T&& operator*()&& {
        return std::move(*ptr_);
    }
    T* operator->() {
        return this->ptr_;
    }
    const T* operator->() const {
        return ptr_;
    }

    const T& operator*() const& {
        return *ptr_;
    }

    const T&& operator*() const&& {
        return std::move(*ptr_);
    }


    // Метод Value() генерирует исключение BadOptionalAccess, если Optional пуст
    //T& Value() {
    //    if (is_initialized_) {
    //        return *ptr_;
    //    }
    //    throw BadOptionalAccess();
    //}
    //const T& Value() const {
    //    if (is_initialized_) { 
    //        return *ptr_;
    //    }
    //    throw BadOptionalAccess();
    //}

    // Перегрузка по rvalue-ссылке сделана для того чтобы при вызове у временных объектов они возвращали rvalue-ссылку на содержимое Optional.
    T& Value()& {
        if (!is_initialized_) throw BadOptionalAccess();
        return *ptr_;
    }
    T&& Value()&& {
        if (!is_initialized_) throw BadOptionalAccess();
        return std::move(*ptr_);
    }

    const T& Value() const& {
        if (is_initialized_) {
            return *ptr_;
        }
        throw BadOptionalAccess();
    }

    const T&& Value() const&& {
        if (is_initialized_) {
            return std::move(*ptr_);
        }
        throw BadOptionalAccess();
    }

    void Reset() {
        if (is_initialized_) {
            ptr_->~T();
            ptr_ = nullptr;
        }
        is_initialized_ = false;
    }

    // Этот метод должен создавать значение внутри Optional путём прямой передачи параметров его конструктору.
    // Если в Optional содержалось значение, оно должно быть предварительно разрушено перед созданием нового значения.
    template <typename... Args>
    void Emplace(Args&&... args) {
        if (is_initialized_) {
            Reset();
        }
        ptr_ = new (&data_[0]) T(std::forward<Args>(args)...);
        is_initialized_ = true;
    }

private:
    // alignas нужен для правильного выравнивания блока памяти
    alignas(T) char data_[sizeof(T)];
    bool is_initialized_ = false;

    T* ptr_ = nullptr;
};