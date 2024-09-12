#include <iostream>
namespace glm::detail
{
	consteval bool NotEmpty(length_t I, length_t L) { return I <= L; }

	template <typename data_t, typename T, length_t L>
	struct ElementCollection;
	template <typename data_t, typename T>
	struct ElementCollection<data_t, T, 4> {
		union
		{
				struct {
					union { T x, r, s; }; 
					union { T y, g, t; };
					union { T z, b, p; };
					union { T w, a, q; };
				};
				data_t data;
		};
	};
	
	struct Empty {};
	#define G [[no_unique_address]]
	template <typename data_t, typename T>
	struct ElementCollection<data_t, T, 3> {
		union
		{
				struct {
					union { T x, r, s; }; 
					union { T y, g, t; };
					union { T z, b, p; };
					union { G Empty w, a, q; };
				};
				data_t data;
		};
	};
	template <typename data_t, typename T>
	struct ElementCollection<data_t, T, 2> {
		union
		{
				struct {
					union { T x, r, s; }; 
					union { T y, g, t; };
					union { G Empty z, b, p; };
					union { G Empty w, a, q; };
				};
				data_t data;
		};
	};
	template <typename data_t, typename T>
	struct ElementCollection<data_t, T, 1> {
		union
		{
				struct {
					union { T x, r, s; }; 
					union { G Empty y, g, t; };
					union { G Empty z, b, p; };
					union { G Empty w, a, q; };
				};
				data_t data;
		};
		#undef G
	};
}

