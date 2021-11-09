#include <iostream>
#include <exception>
#include <cstring>
#include <map>

namespace allocate{
    template<class T>
    class allocator{
        T* buffer;
        size_t offset;
        size_t reserved;

        static const size_t preserved = 100;

    public:
        using value_type = T;

        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        
        allocator(allocator<T>&& oth) :
            buffer{oth.buffer},
            offset{oth.offset},
            reserved{oth.reserved}
        {
            std::cout << "move allocator";
            oth.buffer = nullptr;
        }



        allocator(const allocator<T>&) = delete;

        allocator() : 
            reserved{preserved},
            offset{0}
        {
            std::cout << __PRETTY_FUNCTION__ << std::endl;  
            buffer = new T[preserved];
        }

        ~allocator(){
            std::cout << __PRETTY_FUNCTION__ << std::endl;;
            delete[] buffer;
        }

        T* allocate(size_t size){
            if (offset+size > reserved){
                throw std::bad_alloc();
                //std::cout << "offset+size" << offset+size << std::endl;
            }
            
            offset+=size;
            auto old = buffer;
            buffer+=size;
            return old;
        }

        template<typename U, typename ...Args>
        void construct(U *p, Args &&...args) {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
            new(p) U(std::forward<Args>(args)...);
        };

        void deallocate(T*, std::size_t size){
            std::cout << __PRETTY_FUNCTION__ << std::endl;
        }

        void destroy(T *p){
            std::cout << __PRETTY_FUNCTION__ << std::endl;
        }

    private:
        void new_buffer(){
            pointer old = buffer;
            reserved<<=1;
            buffer = new T[reserved];
            std::memcpy(buffer, old, sizeof(T));
        }
    };
}

int factorial(int num){
    int factorial = 1;
    for (int i = 1; i <= num; i++)
        factorial*=i;

    return factorial;
}

int main(int argc, char* args[]){
    using allocate::allocator;

    std::map<int, int> def_map;
    for (int i = 0; i < 10; i++)
        def_map[i] = factorial(i);
    
    std::map<int, int, std::less<int>,
             allocator<std::pair<const int, int>>> new_map;
    
    for (int i = 0; i < 5; i++)
        new_map[i] = factorial(i);         
    std::cout << "after\n";

    for (int i = 0; i < 10; i++)
        std::cout << new_map[i] << " ";

    return 0;
}
