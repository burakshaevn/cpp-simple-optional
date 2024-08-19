#include <stdexcept>
#include <utility>

// ���������� ����� ���� ������ ������������� ��� ��������� � ������� optional
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

    // ���������� ��� ��������� ��������: ������������ ������� � ��������� Optional. 
    Optional& operator=(const T& value) {
        if (is_initialized_) {
            *ptr_ = value;  // ���� ������ ���������������, �������� ��������
        }
        else {
            ptr_ = new (&data_[0]) T(value);  // ���� ������ �����������������, ������� ����� ��������
            is_initialized_ = true;
        }
        return *this;
    }
    Optional& operator=(T&& rhs) {
        if (is_initialized_) {
            *ptr_ = std::move(rhs);  // ���� ������ ���������������, ���������� ��������
        }
        else {
            ptr_ = new (&data_[0]) T(std::move(rhs));  // ���� ������ �����������������, ������� ����� ��������
            is_initialized_ = true;
        }
        return *this;
    }
    // ������ ��������� ��������: ����� ����� � ������ ��������� �������� ������������ ����� ���� ��� ������, ��� � �������� Optional.
    Optional& operator=(const Optional& rhs) {
        if (this == &rhs) {
            return *this;
        }

        if (is_initialized_) {
            if (rhs.is_initialized_) {
                // ��� ������� ����������������, �������� ����������
                *ptr_ = *rhs.ptr_;
            }
            else {
                // ����� ������ ���������������, ������ - ���, ���������� ����� ������
                Reset();
            }
        }
        else {
            if (rhs.is_initialized_) {
                // ����� ������ �����������������, ������ - ���������������, ������� ����� ������
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
                    // ��� ������� ����������������, ���������� ����������
                    *ptr_ = std::move(*rhs.ptr_);
                }
                else {
                    // ����� ������ ���������������, ������ - ���, ���������� ����� ������
                    Reset();
                }
            }
            else {
                if (rhs.is_initialized_) {
                    // ����� ������ �����������������, ������ - ���������������, ������� ����� ������
                    ptr_ = new (&data_[0]) T(std::move(*rhs.ptr_));
                    is_initialized_ = true;
                }
            }
            //rhs.Reset();  // ������ rhs �� �����!
        }
        return *this;
    }

    ~Optional() {
        Reset();
    }

    bool HasValue() const {
        return is_initialized_;
    }

    // ��������� * � -> �� ������ ������ ������� �������� �� ������� Optional.
    // ��� �������� �������� �� ������� ������������
    T& operator*() {
        return *ptr_;
    }
    const T& operator*() const {
        return *ptr_;
    }
    T* operator->() {
        return this->ptr_;
    }
    const T* operator->() const {
        return ptr_;
    }

    // ����� Value() ���������� ���������� BadOptionalAccess, ���� Optional ����
    T& Value() {
        if (is_initialized_) {
            return *ptr_;
        }
        throw BadOptionalAccess();
    }
    const T& Value() const {
        if (is_initialized_) {
            return *ptr_;
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

private:
    // alignas ����� ��� ����������� ������������ ����� ������
    alignas(T) char data_[sizeof(T)];
    bool is_initialized_ = false;

    T* ptr_ = nullptr;
};