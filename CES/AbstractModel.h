#pragma once

#include "Board.h"

//����������� �� ���� �� ����, ��� ������ ��� ������
class AbstractModel
{

	void getVariants(std::vector<Turn>& variants) const;

	void apply(Turn& h);
};