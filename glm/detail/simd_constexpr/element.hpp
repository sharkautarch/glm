
#include <iostream>
namespace glm::detail
{
	consteval bool NotEmpty(length_t I, length_t L) { return I <= L; }
	template <bool IsNotEmpty, typename T, length_t I, length_t L>
	struct Element;
	template <typename T, length_t I, length_t L> requires (!NotEmpty(I, L))
	struct Element<false, T, I, L>
	{
		constexpr operator auto() {
      return 0;
  	}
  	constexpr decltype(auto) operator=(auto thing) {
      return *this;
  	}
	  constexpr std::ostream& operator<<(std::ostream& os)
	  {
	    return os;
	  }
	  // prefix increment
	  constexpr decltype(auto) operator++() requires requires (T first) { first++; }
	  {
	    return *this; // return new value by reference
	  }
 
	  // postfix increment
	  constexpr decltype(auto) operator++(auto) requires requires (T first) { first++; }
	  {
			return *this;
	  }
 
	  // prefix decrement
	  constexpr decltype(auto) operator--() requires requires (T first) { first--; }
	  {
	    return *this; // return new value by reference
	  }
 
	  // postfix decrement
	  constexpr decltype(auto) operator--(auto) requires requires (T first) { first--; }
	  {
			return *this;
	  }
	  constexpr decltype(auto) operator+=(auto rhs) requires requires (T first, decltype(rhs) r) { first + r; }
	  {
	  	return *this;
	  }
	  constexpr decltype(auto) operator-=(auto rhs) requires requires (T first, decltype(rhs) r) { first - r; }
	  {
	  	return *this;
	  }
	  constexpr decltype(auto) operator*=(auto rhs) requires requires (T first, decltype(rhs) r) { first * r; }
	  {
	  	return *this;
	  }
	  
	  constexpr decltype(auto) operator/=(auto rhs) requires requires (T first, decltype(rhs) r) { first / r; }
	  {
	  	return *this;
	  }
	  
	  constexpr decltype(auto) operator%=(auto rhs) requires requires (T first, decltype(rhs) r) { first % r; }
	  {
	  	return *this;
	  }
	  
	  constexpr decltype(auto) operator&=(auto rhs) requires requires (T first, decltype(rhs) r) { first & r; }
	  {
	  	return *this;
	  }
	  
	  constexpr decltype(auto) operator|=(auto rhs) requires requires (T first, decltype(rhs) r) { first | r; }
	  {
	  	return *this;
	  }
	  
	  constexpr decltype(auto) operator^=(auto rhs) requires requires (T first, decltype(rhs) r) { first ^ r; }
	  {
	  	return *this;
	  }
	  
	  constexpr decltype(auto) operator<<=(auto rhs) requires requires (T first, decltype(rhs) r) { first << r; }
	  {
	  	return *this;
	  }
	  constexpr decltype(auto) operator>>=(auto rhs) requires requires (T first, decltype(rhs) r) { first >> r; }
	  {
	  	return *this;
	  }
	  
	  constexpr decltype(auto) operator+(auto rhs) requires requires (T first, decltype(rhs) r) { first + r; }
	  {
	  	return rhs;
	  }
	  constexpr decltype(auto) operator-(auto rhs) requires requires (T first, decltype(rhs) r) { first - r; }
	  {
	  	return -rhs;
	  }
	  constexpr decltype(auto) operator*(auto rhs) requires requires (T first, decltype(rhs) r) { first * r; }
	  {
	  	return 0;
	  }
	  constexpr decltype(auto) operator/(auto rhs) requires requires (T first, decltype(rhs) r) { first / r; }
	  {
	  	return 0;
	  }
	  constexpr decltype(auto) operator%(auto rhs) requires requires (T first, decltype(rhs) r) { first % r; }
	  {
	  	return 0;
	  }
	  constexpr decltype(auto) operator&(auto rhs) requires requires (T first, decltype(rhs) r) { first & r; }
	  {
	  	return 0;
	  }
	  constexpr decltype(auto) operator|(auto rhs) requires requires (T first, decltype(rhs) r) { first | r; }
	  {
	  	return rhs;
	  }
	  constexpr decltype(auto) operator^(auto rhs) requires requires (T first, decltype(rhs) r) { first ^ r; }
	  {
	  	return 0^rhs;
	  }
	  constexpr decltype(auto) operator<<(auto rhs) requires requires (T first, decltype(rhs) r) { first << r; }
	  {
	  	return 0;
	  }
	  constexpr decltype(auto) operator>>(auto rhs) requires requires (T first, decltype(rhs) r) { first >> r; }
	  {
	  	return 0;
	  }
	  constexpr decltype(auto) operator~() requires requires (T first) { ~first; }
	  {
	  	return 0;
	  }
	  
	  constexpr decltype(auto) operator||(auto rhs) requires requires (T first, decltype(rhs) r) { first + r; }
	  {
	  	return false || rhs;
	  }
	  
	  constexpr decltype(auto) operator&&(auto rhs) requires requires (T first, decltype(rhs) r) { first + r; }
	  {
	  	return false;
	  }
	  
	  constexpr decltype(auto) operator!() requires requires (T first) { !first; }
	  {
	  	return false;
	  }
	  
	  constexpr operator bool() requires requires (T first) { !!first; }
	  {
	  	return false;
	  }
	  
	  constexpr decltype(auto) operator +() requires requires (T first) { +first; }
	  {
	  	return 0;
	  }
	  
	  constexpr decltype(auto) operator -() requires requires (T first) { -first; }
	  {
	  	return 0;
	  }
	  
	  constexpr decltype(auto) operator&() requires requires (T first) { &first; }
	  {
	  	return nullptr;
	  }
	  
	  constexpr decltype(auto) operator<=>(auto rhs) requires requires (T first, decltype(rhs) r) { first <=> r; }
	  {
	  	return (T)false <=> rhs;
	  }
	};
	
	
	template <typename T, length_t I, length_t L> requires (NotEmpty(I, L))
	struct Element<true, T, I, L> 
	{
		T t;
		constexpr operator auto() 
		{
      return t;
  	}
  	
  	constexpr operator auto() const
		{
      return t;
  	}
  	constexpr decltype(auto) operator=(auto thing) 
  	{
      t=(T)thing;
      return *this;
  	}
	  constexpr std::ostream& operator<<(std::ostream& os)
	  {
	      return os << t;
	  }
	  // prefix increment
	  constexpr decltype(auto) operator++() requires requires (T first) { first++; }
	  {
	      t++;
	      return *this; // return new value by reference
	  }
 
	  // postfix increment
	  constexpr decltype(auto) operator++(auto) requires requires (T first) { first++; }
	  {
	      auto old = *this; // copy old value
	      operator++();  // prefix increment
	      return old;    // return old value
	  }
 
	  // prefix decrement
	  constexpr decltype(auto) operator--() requires requires (T first) { first--; }
	  {
	      t--;
	      return *this; // return new value by reference
	  }
 
	  // postfix decrement
	  constexpr decltype(auto) operator--(auto) requires requires (T first) { first--; }
	  {
	      auto old = *this; // copy old value
	      operator--();  // prefix decrement
	      return old;    // return old value
	  }
	  constexpr decltype(auto) operator+=(auto rhs) requires requires (T first, decltype(rhs) r) { first + r; }
	  {
	  	t+=rhs;
	  	return *this;
	  }
	  constexpr decltype(auto) operator-=(auto rhs) requires requires (T first, decltype(rhs) r) { first - r; }
	  {
	  	t-=rhs;
	  	return *this;
	  }
	  constexpr decltype(auto) operator*=(auto rhs) requires requires (T first, decltype(rhs) r) { first * r; }
	  {
	  	t*=rhs;
	  	return *this;
	  }
	  
	  constexpr decltype(auto) operator/=(auto rhs) requires requires (T first, decltype(rhs) r) { first / r; }
	  {
	  	t/=rhs;
	  	return *this;
	  }
	  
	  constexpr decltype(auto) operator%=(auto rhs) requires requires (T first, decltype(rhs) r) { first % r; }
	  {
	  	t%=rhs;
	  	return *this;
	  }
	  
	  constexpr decltype(auto) operator&=(auto rhs) requires requires (T first, decltype(rhs) r) { first & r; }
	  {
	  	t&=rhs;
	  	return *this;
	  }
	  
	  constexpr decltype(auto) operator|=(auto rhs) requires requires (T first, decltype(rhs) r) { first | r; }
	  {
	  	t|=rhs;
	  	return *this;
	  }
	  
	  constexpr decltype(auto) operator^=(auto rhs) requires requires (T first, decltype(rhs) r) { first ^ r; }
	  {
	  	t^=rhs;
	  	return *this;
	  }
	  
	  constexpr decltype(auto) operator<<=(auto rhs) requires requires (T first, decltype(rhs) r) { first << r; }
	  {
	  	t<<=rhs;
	  	return *this;
	  }
	  constexpr decltype(auto) operator>>=(auto rhs) requires requires (T first, decltype(rhs) r) { first >> r; }
	  {
	  	t>>=rhs;
	  	return *this;
	  }
	  
	  constexpr decltype(auto) operator+(auto rhs) requires requires (T first, decltype(rhs) r) { first + r; }
	  {
	  	auto lhs = *this;
	  	lhs+=rhs;
	  	return lhs.t;
	  }
	  constexpr decltype(auto) operator-(auto rhs) requires requires (T first, decltype(rhs) r) { first - r; }
	  {
	  	auto lhs = *this;
	  	lhs-=rhs;
	  	return lhs.t;
	  }
	  constexpr decltype(auto) operator*(auto rhs) requires requires (T first, decltype(rhs) r) { first * r; }
	  {
	  	auto lhs = *this;
	  	lhs*=rhs;
	  	return lhs.t;
	  }
	  constexpr decltype(auto) operator/(auto rhs) requires requires (T first, decltype(rhs) r) { first / r; }
	  {
	  	auto lhs = *this;
	  	lhs/=rhs;
	  	return lhs.t;
	  }
	  constexpr decltype(auto) operator%(auto rhs) requires requires (T first, decltype(rhs) r) { first % r; }
	  {
	  	auto lhs = *this;
	  	lhs%=rhs;
	  	return lhs.t;
	  }
	  constexpr decltype(auto) operator&(auto rhs) requires requires (T first, decltype(rhs) r) { first & r; }
	  {
	  	auto lhs = *this;
	  	lhs&=rhs;
	  	return lhs.t;
	  }
	  constexpr decltype(auto) operator|(auto rhs) requires requires (T first, decltype(rhs) r) { first | r; }
	  {
	  	auto lhs = *this;
	  	lhs|=rhs;
	  	return lhs.t;
	  }
	  constexpr decltype(auto) operator^(auto rhs) requires requires (T first, decltype(rhs) r) { first ^ r; }
	  {
	  	auto lhs = *this;
	  	lhs^=rhs;
	  	return lhs.t;
	  }
	  constexpr decltype(auto) operator<<(auto rhs) requires requires (T first, decltype(rhs) r) { first << r; }
	  {
	  	auto lhs = *this;
	  	lhs<<=rhs;
	  	return lhs.t;
	  }
	  constexpr decltype(auto) operator>>(auto rhs) requires requires (T first, decltype(rhs) r) { first >> r; }
	  {
	  	auto lhs = *this;
	  	lhs>>=rhs;
	  	return lhs.t;
	  }
	  constexpr decltype(auto) operator~() requires requires (T first) { ~first; }
	  {
	  	auto lhs = *this;
	  	lhs.t = ~(lhs.t);
	  	return lhs.t;
	  }
	  
	  constexpr decltype(auto) operator||(auto rhs) requires requires (T first, decltype(rhs) r) { first || r; }
	  {
	  	auto lhs = *this;
	  	lhs.t = lhs.t || rhs.t;
	  	return lhs.t;
	  }
	  
	  constexpr decltype(auto) operator&&(auto rhs) requires requires (T first, decltype(rhs) r) { first && r; }
	  {
	  	auto lhs = *this;
	  	lhs.t = lhs.t && rhs.t;
	  	return lhs.t;
	  }
	  
	  constexpr decltype(auto) operator!() requires requires (T first) { !first; }
	  {
	  	auto lhs = *this;
	  	return !lhs.t;
	  }
	  
	  constexpr operator bool() requires requires (T first) { !!first; }
	  {
	  	auto lhs = *this;
	  	return !!lhs.t;
	  }
	  
	  constexpr decltype(auto) operator +() requires requires (T first) { +first; }
	  {
	  	auto lhs = *this;
	  	return +lhs.t;
	  }
	  
	  constexpr decltype(auto) operator -() requires requires (T first) { -first; }
	  {
	  	auto lhs = *this;
	  	return -lhs.t;
	  }
	  
	  constexpr decltype(auto) operator&() requires requires (T first) { &first; }
	  {
	  	return &(t);
	  }
	  
	  constexpr decltype(auto) operator<=>(auto rhs) requires requires (T first, decltype(rhs) r) { first <=> r; }
	  {
	  	return t <=> rhs;
	  }
	};
}