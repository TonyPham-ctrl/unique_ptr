#include <iostream>

template <typename T>

class UniquePtr{
private:
    T& ptr_ = nullptr;
public:
    UniquePtr() = default;
    UniquePtr(std::nullptr_t);
    explicit UniquePtr(T* ptr);

    UniquePtr(const UniquePtr& ptr) = delete;
    UniquePtr operator=(const UniquePtr&) = delete;
    

    ~UniquePtr();


    T* get() noexcept;
    T* release() noexcept;
    void reset(T ptr) noxecept;
    void swap(UniquePtr& other) noexcept;
};