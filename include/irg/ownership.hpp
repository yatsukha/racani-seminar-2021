#pragma once

#include <memory>
#include <functional>
#include <type_traits>

namespace irg {

  template<typename T>
  using shared_ownership = ::std::shared_ptr<T>;

  template<typename T>
  using deleter = ::std::function<void(T*)>;

  template<typename T, typename C>
  shared_ownership<T> deffer_ownership(T* ptr, C const& c) {
    // code bloat brought to you by type deduction
    return {ptr, [&c](T* p){ 
      if (auto cc = static_cast<deleter<T> const&>(c); cc)
        cc(p);
      delete p; 
    }};
  }


}