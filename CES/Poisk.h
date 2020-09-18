#pragma once
#include <cstdlib>

#include <vector>
#include <thread>
#include <climits>

//#define AI_PLAYER 1
//#define HUMAN_PLAYER 0
#define GAME_GOES_ON -1

template <typename M, typename T>
class DeepPoisk
{
public:

	//==========================================================================

	//точка входа
	//выбирает ход с максимальным first/second
	//использует алгоритм poisk3
	Turn selectTurn_1_3(M& model)
	{
		setPlayerFromModel(model);

		auto p_turnsW = model.getVariants();
		auto turnsW = *p_turnsW;

		std::vector<float> results;

		for (size_t i = 0; i < turnsW.size(); i++)
		{
			M modelCopy = model;
			modelCopy.doTurn(turnsW[i]);

			auto pair = poisk3(modelCopy);
			results.push_back((float)pair.first / (float)pair.second);
		}

		size_t maxI = 0;
		for (size_t i = 1; i < results.size(); i++)
			if (results[i] > results[maxI])
				maxI = i;
		return turnsW[maxI];
	}

	//точка входа
	//выбирает ход с максимальным first/second
	//использует алгоритм poisk3
	Turn selectTurn(M& model)
	{
		setPlayerFromModel(model);

		auto p_turnsW = model.getVariants();
		auto turnsW = *p_turnsW;

		std::vector<float> results;

		for (size_t i = 0; i < turnsW.size(); i++)
		{
			M modelCopy = model;
			modelCopy.doTurn(turnsW[i]);

			float r = poisk4(modelCopy);
			results.push_back(r);
		}

		size_t maxI = 0;
		for (size_t i = 1; i < results.size(); i++)
			if (results[i] > results[maxI])
				maxI = i;
		return turnsW[maxI];
	}


	//точка входа
	//выбирает случайный ход
	Turn selectTurn_random(M& model)
	{
		setPlayerFromModel(model);

		auto p_turnsW = model.getVariants();
		auto turnsW = *p_turnsW;

		return turnsW[rand() % turnsW.size()];
	}

	//точка входа
	//выбирает ход с наилучшей позицией после него
	Turn selectTurn_simple(M& model)
	{
		setPlayerFromModel(model);

		auto p_turnsW = model.getVariants();
		auto turnsW = *p_turnsW;

		std::vector<float> results;

		for (size_t i = 0; i < turnsW.size(); i++)
		{
			M modelCopy = model;
			modelCopy.doTurn(turnsW[i]);
			float rating = analyze(modelCopy);
			results.push_back(rating);
		}

		size_t maxI = 0;
		for (size_t i = 1; i < results.size(); i++)
			if (results[i] > results[maxI])
				maxI = i;
		return turnsW[maxI];
	}


protected:

	//==========================================================================

	size_t player;
	size_t otherPlayer;
	void setPlayer(size_t p)
	{
		player = p;
		otherPlayer = 1 - p;
		if (player > 1)
			throw std::exception();
	}

	void setPlayerFromModel(M& model)
	{
		if (model.whoseTurn() == White)
		{
			auto p_turnsB = model.getVariants();
			if (model.whoseTurn() == Black)
				setPlayer(Black);
			else setPlayer(White);
		}
		else 
		{
			auto p_turnsB = model.getVariants();
			if (model.whoseTurn() == White)
				setPlayer(White);
			else setPlayer(Black);
		}
	}

	//==========================================================================

	float analyze(M& model)
	{
		float rating = 0;

		if (model.won() != GAME_GOES_ON)
		{ 
			if (model.won() == player)
				rating += 1;
		}
		else
		{
			rating += ((player == 1 ? -1 : 1) * model.boardAnalize() + 1) / 2.0f;//0...2 / 2 = 0...1
		}

		return rating;
	}

	//==========================================================================

#ifdef _DEBUG
#define DEFAULT_DEPTH 5
#else
#define DEFAULT_DEPTH 9
#endif

	//считает только победы и поражения, не оценивая позицию. возвращает (w, l)
	//баг: неверно определяет, кто ходит
	std::pair<int, int> poisk(M& model, int depth = 0)
	{
		auto variants = model.getVariants();

		int w = 0;
		int l = 0;

		for (auto& v : *variants)
		{
			M modelCopy = model;
			modelCopy.doTurn(v);

			//кто-то выиграл
			if (modelCopy.won() != GAME_GOES_ON)
			{
				if (modelCopy.won() == player)
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
	}

	//учитывает позицию. возвращает (rating, maxRating)
	//баг: неверно определяет, кто ходит
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
			if (modelCopy.won() != GAME_GOES_ON)
			{
				rating += analyze(modelCopy);
				maxRating += 1;
			}
			else if (depth == maxDepth) //заканчиваем ветку
			{
				rating += analyze(modelCopy);
				maxRating += 1;
			}
			else
			{
				auto r = poisk2(modelCopy, maxDepth, depth + 1);
				rating += r.first;
				maxRating += r.second;

				rating /= maxRating;
				maxRating /= maxRating;
			}
		}

		return std::make_pair(rating, maxRating);
	}

	//учитывает позицию. возвращает (rating, maxRating)
	std::pair<float, float> poisk3(M& model, int maxDepth = 5, int depth = 0)
	{
		float rating = 0;
		float maxRating = 0;

		if (model.won() == GAME_GOES_ON && depth < maxDepth)
		{
			//if (model.whoseTurn() == player)
			//{
				auto variants = model.getVariants();
				for (auto& v : *variants)
				{
					M modelCopy = model;
					modelCopy.doTurn(v);
					auto r = poisk3(modelCopy, maxDepth, depth + 1);

					//std::cout << r.first << " " << r.second << "\n";

					rating += r.first;
					maxRating += r.second;

					//rating += analyze(model);
					//maxRating += 1;
				}
			//}
			//else if (model.whoseTurn() == otherPlayer)
			//{

			//}
		}

		//заканчиваем
		if (model.won() != GAME_GOES_ON || depth == maxDepth)
		{
			rating += analyze(model);
			maxRating++;

			rating /= maxRating;
			maxRating /= maxRating;

			return std::make_pair(rating, maxRating);
		}



		//for (auto& v : *variants)
		//{
		//	M modelCopy = model;
		//	modelCopy.doTurn(v);

		//	//кто-то выиграл
		//	if (modelCopy.won() != -1)
		//	{
		//		if (modelCopy.won() == AI_PLAYER)
		//			rating += 1;
		//		maxRating += 1;
		//	}
		//	else if (depth == maxDepth) //заканчиваем ветку
		//	{
		//		rating += ((AI_PLAYER == 1 ? -1 : 1) * modelCopy.boardAnalize() + 1) / 5.0f;
		//		maxRating += 1;
		//	}
		//	else
		//	{
		//		auto r = poisk2(modelCopy, maxDepth, depth + 1);
		//		rating += r.first;
		//		maxRating += r.second;

		//		rating /= maxRating;
		//		maxRating /= maxRating;
		//	}
		//}

		return std::make_pair(rating, maxRating);
	}

	//учитывает позицию. возвращает rating. минимаксный критерий
	float poisk4(M& model, int depth = DEFAULT_DEPTH)
	{
		short minRating = INT16_MAX;//3.40282e+038f;
        short maxRating = INT16_MIN;
		//float maxRating = 0;

		if (model.won() == GAME_GOES_ON && depth > 0)
		{
			//if (model.whoseTurn() == player)
			//{
			auto variants = model.getVariants();
			for (auto& v : *variants)
			{
				M modelCopy = model;
				modelCopy.doTurn(v);
				float r = poisk4(modelCopy, depth - 1);

                if (depth % 2 == 0)
                {
                    if (r > maxRating)
                        maxRating = r;
                }

                else
                {
                    if (r < minRating)
                        minRating = r;
                }
			}
            if (depth % 2 == 0)
                minRating = maxRating;
		}

		//заканчиваем
		else
		//if (!(model.won() == GAME_GOES_ON && depth > 0))
		{
			minRating = analyze(model);

			//return minRating;
		}



		//for (auto& v : *variants)
		//{
		//	M modelCopy = model;
		//	modelCopy.doTurn(v);

		//	//кто-то выиграл
		//	if (modelCopy.won() != -1)
		//	{
		//		if (modelCopy.won() == AI_PLAYER)
		//			rating += 1;
		//		maxRating += 1;
		//	}
		//	else if (depth == maxDepth) //заканчиваем ветку
		//	{
		//		rating += ((AI_PLAYER == 1 ? -1 : 1) * modelCopy.boardAnalize() + 1) / 5.0f;
		//		maxRating += 1;
		//	}
		//	else
		//	{
		//		auto r = poisk2(modelCopy, maxDepth, depth + 1);
		//		rating += r.first;
		//		maxRating += r.second;

		//		rating /= maxRating;
		//		maxRating /= maxRating;
		//	}
		//}

		return minRating;
	}

};

template <typename M, typename T>
class DeepPoiskMultiThread : public DeepPoisk<M, T>
{
public:
	static std::pair<Turn, M*>* static_args[];
	//для многопоточного поиска
	static float static_results[];

	std::vector<std::pair<Turn, M*>> args_val;

protected:

	//auto p_turnsW;

	//для многопоточного поиска кладём результаты одного из ходов.
	//используем poisk2
	void getTurnResult(size_t i, Turn t, M* model)
	{
		M modelCopy = *model;
		modelCopy.doTurn(t);
		auto f = poisk4(modelCopy);
		static_results[i] = f;
	}

	void thread_main(size_t i)
	{
		while (true)
		{
			if (static_args[i] != nullptr)
			{
				getTurnResult(i, static_args[i]->first, static_args[i]->second);
				static_args[i] = nullptr;
			}
			_sleep(40);
		}
	}

public:

	std::vector<std::thread> threads;
	DeepPoiskMultiThread()
	{
		for (size_t i = 1; i < 12; i++)
			threads.emplace_back(std::thread(&DeepPoiskMultiThread<M, T>::thread_main, this, i));
	}

	//точка входа
	//пока нет мьютекса у аллокаций пула, не работает
	Turn selectTurnMultiThread(M& model/*, int threadCount = 0*/)
	{
		setPlayerFromModel(model);

		//if (threadCount == 0)
		//{
			//threadCount = std::thread::hardware_concurrency();
		//}
		//else if (threadCount == 1)
		//	return selectTurn(model);

		auto p_turnsW = model.getVariants();
		auto turnsW = *p_turnsW;

		//std::vector<std::thread> threads;

		for (size_t i = 1; i < turnsW.size(); i++)
			static_results[i] = 32000;

		args_val.clear();
		for (size_t i = 0; i < turnsW.size(); i++)
		{
			args_val.push_back(std::make_pair(turnsW[i], &model));
			//threads.emplace_back(std::thread(&DeepPoisk<M, T>::getTurnResult, i, turnsW[i], &model));
		}
		for (size_t i = 1; i < turnsW.size(); i++)
			static_args[i] = &args_val[i];

		getTurnResult(0, turnsW[0], &model);

		for (size_t i = 1; i < turnsW.size(); i++)
		{
			if (static_results[i] == 32000)
			{
				_sleep(25);
				i = 0;
				continue;
			}
		}

		//for (size_t i = 1; i < turnsW.size(); i++)
		//	threads[i].join();

		size_t maxI = 0;
		for (size_t i = 1; i < turnsW.size(); i++)
			if (static_results[i] > static_results[maxI])
				maxI = i;
		return turnsW[maxI];
	}

};