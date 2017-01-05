#include <array>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>


// Tags for supported containers
struct t_tag_array {};
struct t_tag_vector {};

// Tag dispatching
template<class T, size_t S>
struct t_container_tag {};

template<class T, size_t S>
struct t_container_tag<std::vector<T>, S> {
        using type = t_tag_vector;
};

template<class T, size_t S>
struct t_container_tag<std::array<T, S>, S> {
        using type = t_tag_array;
};

// Helper to fetch the size of an std::array
template<typename>
struct array_size;

template<typename T, size_t S>
struct array_size<std::array<T, S>> {
    static const auto value = S;
};

template <typename C, size_t S = array_size<C>::value>
struct foo 
{
        using value_type = typename C::value_type;

        // Constructor
        template<typename... Args>
        foo(Args&&... args) : foo(typename t_container_tag<C, S>::type{}, std::forward<Args>(args)...) {}

        // Specialized constructor for vectors
        template<typename... Args>
        foo(t_tag_vector &&, Args&&... args) : m(std::forward<Args>(args)...), container(S, value_type{}) {}
        
        // Specialized constructor for arrays
        template<typename... Args>
        foo(t_tag_array &&, Args&&... args) : m(std::forward<Args>(args)...), container{} {}

        value_type m;
        C container;
};

int main()
{
    const std::string str_data = "Hello, World!";

    foo<std::vector<int>, 7> vec_foo(42);
    foo<std::array<std::string, 6>> arr_foo(std::begin(str_data), std::end(str_data));
        
    // The size of the containers
    std::cout << vec_foo.container.size() << ' ' 
        << arr_foo.container.size() << std::endl;

    // The m members contain the assigned values
    std::cout << vec_foo.m << " \'" << arr_foo.m << '\'' << std::endl;

    // The containers are zero or default initialized
    std::cout << vec_foo.container.front() << " \'" << 
                arr_foo.container.front() << '\'' << std::endl;
        
        return 0;
}
