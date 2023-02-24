#pragma once

#include <iostream>
#include <memory>
#include <functional>

class Object
{
  struct ObjectConcept
  {
    virtual ~ObjectConcept() {}
    virtual void run() = 0;
  };

  template <typename Func, typename... Args>
  class ObjectModel : public ObjectConcept
  {
    Func func;
    std::tuple<Args...> args;

  public:
    ObjectModel(Func func, std::tuple<Args...> &&tup)
        : func(func), args(std::move(tup)) {}
    using return_type = std::result_of_t<Func(Args...)>;
    void run() override
    {
      if constexpr (std::is_same_v<return_type, void>)
        std::apply(this->func, this->args);
      else
      {
        return_type res = std::apply(this->func, this->args);
        std::cout << "result : " << res << '\n';
      }
    }
  };

  std::unique_ptr<ObjectConcept> object;

public:
  template <typename Func, typename... Args>
  Object(Func func, std::tuple<Args...> tup)
      : object(new ObjectModel(func, std::move(tup))) {}

  void run() { object->run(); }
};

/*
 * @author bigdj2002@naver.com
 * INFO: -
 * FIXME: Not implemented yet with followoing example
 */

// int foo(int a, int b) { return a + b; }
// std::string bar(std::string msg) { return msg + " world"; }
// void zoo(double a) { std::cout << a * 3.14 << '\n'; }

// int main()
// {
//     std::vector<Object> vec;

//     vec.emplace_back(foo, std::tuple(100, 200));
//     vec.emplace_back(bar, std::tuple<std::string>("hello"));
//     vec.emplace_back(zoo, std::tuple(1.23));
    
//     for (auto& e : vec)
//         e.run();
// }