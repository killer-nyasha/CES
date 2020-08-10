#pragma once

#include "Board.h"

//наследовать от него не надо, это скорее как пример
class AbstractModel
{

	void getVariants(std::vector<Turn>& variants) const;

	void apply(Turn& h);
};