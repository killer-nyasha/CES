#pragma once
#include <vector>

template <typename M, typename T>
class DeepPoisk
{
public:

	Turn selectTurn(M& model)
	{
		auto p_turnsW = model.getVariants();
		auto turnsW = *p_turnsW;

		std::vector<float> results;

		for (size_t i = 0; i < turnsW.size(); i++)
		{
			M modelCopy = model;
			modelCopy.doTurn(turnsW[i]);

			auto pair = poisk2(modelCopy);
			results.push_back((float)pair.first / (float)pair.second);

			//std::cout << "yeah!";
		}

		size_t maxI = 0;
		for (size_t i = 1; i < results.size(); i++)
			if (results[i] > results[maxI])
				maxI = i;
		return turnsW[maxI];
	}

	//����� � �������
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

				//���� ���������
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

		//throw "� ��� ����� �� ������";
	}

	//����� � �������
	std::pair<float, float> poisk2(M& model, int maxDepth = 7, int depth = 0)
	{
		auto variants = model.getVariants();

		float rating = 0;
		float maxRating = 0;

		for (auto& v : *variants)
		{
			M modelCopy = model;
			modelCopy.doTurn(v);

			//кто-то выиграл
			if (modelCopy.won() != -1)
			{
				//std::cout << "end at " << depth << "\n";
				if (modelCopy.won() == 0)
					rating += 1;
				maxRating += 1;
			}
			else if (depth == maxDepth) //заканчиваем ветку
			{
				rating += (modelCopy.boardAnalize()+1)/3.0f;
				maxRating += 1;
			}
			else
			{
				auto r = poisk2(modelCopy, maxDepth, depth + 1);
				rating += r.first;
				maxRating += r.second;
			}
		}

		return std::make_pair(rating, maxRating);
	}

};