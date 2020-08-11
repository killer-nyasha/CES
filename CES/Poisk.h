#pragma once
#include <vector>

template <typename M, typename T>
class Poisk
{
public:

	Turn selectTurn(M& model)
	{
		auto turnsW = model.getVariants();

		std::vector<std::pair<int, int>> results;

		for (size_t i = 0; i < turnsW.size(); i++)
		{
			M modelCopy = model;
			modelCopy.doTurn(turnsW[i]);

			results.push_back(poisk(modelCopy));

			std::cout << "yeah!";
		}
	}

	//поиск в глубину
	std::pair<int, int> poisk(M& model, int depth = 0)
	{
		auto variants = model.getVariants();

		int w = 0;
		int l = 0;

		for (auto& v : *variants)
		{
			M modelCopy = model;
			modelCopy.doTurn(v);

			if (modelCopy.won() != -1)
			{
				//std::cout << "end at " << depth << "\n";

				//игра закончена
				if (modelCopy.won() == 0)
					w++;
				else l++;
			}
			else
			{
				auto r = poisk(modelCopy, depth + 1);
				w += r.first;
				l += r.second;
			}
		}

		return std::make_pair(w, l);

		//throw "а хер знает чё делать";
	}
};