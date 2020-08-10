#pragma once
#include <vector>

template <typename M>
class Poisk
{
	std::vector<M::Hod> variants;

	void poisk(M& model)
	{
		model.getVariants(variants);

		throw "а хер знает чё делать";
	}
};