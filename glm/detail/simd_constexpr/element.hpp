#include <iostream>
namespace glm::detail
{
	consteval bool NotEmpty(length_t I, length_t L) { return I <= L; }

	template <typename DataWrapper, typename T, length_t L>
	struct ElementCollection;
	template <typename DataWrapper, typename T>
	struct ElementCollection<DataWrapper, T, 4> {
		using data_t = DataWrapper::data_t;
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
	template <typename DataWrapper, typename T>
	struct ElementCollection<DataWrapper, T, 3> {
		using data_t = DataWrapper::data_t;
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
	template <typename DataWrapper, typename T>
	struct ElementCollection<DataWrapper, T, 2> {
		using data_t = DataWrapper::data_t;
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
	template <typename DataWrapper, typename T>
	struct ElementCollection<DataWrapper, T, 1> {
		using data_t = DataWrapper::data_t;
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

