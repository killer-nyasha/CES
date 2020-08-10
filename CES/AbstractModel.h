#pragma once

//наследовать от него не надо, это скорее как пример
class AbstractModel
{
	class Hod
	{

	};

	void getVariants(std::vector<Hod>& variants) const;

	void apply(Hod& h);
};