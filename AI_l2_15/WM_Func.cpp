#include "WindMill.h"
#include<time.h>

unsigned int f_size = 24; // размер поля

vector<int> field = vector<int>();

bool is_game_over = false;
unsigned int PLAYER = 1; // ИГРОК
unsigned int COMPUTER = 2; // КОМПЬЮТЕР
unsigned int MAX_DEPTH = 6; // Глубина поиска хода

unsigned int turns = 0; // число всех ходов
vector<array<int, 3>> all_Mills = vector<array<int, 3>>();

// =================================== С КОГО НАЧИНАТЬ ИГРУ ===================================
unsigned int currentPlayer = PLAYER;
// ============================================================================================

// Начать игру
void play()
{
	create_field();
	Create_Mills();// инициализировать мельницы
	vector<array<int, 3>> PU_Mills = vector<array<int, 3>>();//использовано игроком
	vector<array<int, 3>> CU_Mills = vector<array<int, 3>>();//использовано компом
	print_field(field);
	int p_chip = 9;//фишки игрока в кармане
	int c_chip = 9;//фишки компа в кармане
	int p_f_chip = 0; //фишки игрока на поле
	int c_f_chip = 0;
	unsigned int winner = 0;
	//bool cap = false;

	while (!Have_winner(field,winner, p_chip, c_chip) )
	{
		if (currentPlayer == COMPUTER)
		{
			//Сделать ход
			array<int, 3> move = computer_move(PU_Mills, CU_Mills, p_chip, p_f_chip, c_chip, c_f_chip, false);
			make_move(field, move[1], move[2], COMPUTER, c_chip, c_f_chip);
			vector<array<int, 3>> CUC_Mills = copy(CU_Mills);
			bool mill = CheckMill(field, COMPUTER, CU_Mills);
			if (mill)
			{
				cout << "Я собрал мельницу, щас буду думать что убрать" << endl;
				move = computer_move(PU_Mills, CUC_Mills, p_chip, p_f_chip, c_chip, c_f_chip, true);
				field[move[1]] = 0;
				p_f_chip--;
			}
			cout << "Мои фишки в кармане: "<<c_chip<<" На поле: "<< c_f_chip << endl;
			currentPlayer = PLAYER;
		}
		else if (currentPlayer == PLAYER)
		{
			cout << "У вас осталось фишек в кармане: " << p_chip << " И на поле: " << p_f_chip << endl;
			make_user_move(field, p_chip, p_f_chip);
			if (CheckMill(field, PLAYER, PU_Mills))
			{
				UserEat(field);
				c_f_chip--;
			}
			currentPlayer = COMPUTER;
		}
		cout << endl;
		print_field(field);
	}
	if (winner == COMPUTER)
	{
		cout << "Вы проиграли!" << endl;
	}
	else
		cout << "Удивительно, но я прое...считался... Вы победили." << endl;
}

// Ход компьютера
array<int, 3> computer_move(vector<array<int, 3>>& puMill, vector<array<int, 3>>& cuMill, int p_chip, int f_p_chip, int c_chip, int f_c_chip, bool Mill_phase)
{
	cout << "Компьютер думает..." << endl;
	time_t s = clock();
	array<int, 3> res = ab_clipping(field, MAX_DEPTH, 0 - INT_MAX, INT_MAX, COMPUTER, puMill, cuMill, p_chip, f_p_chip, c_chip, f_c_chip, Mill_phase);
	time_t f = clock();
	int frm = res[1];
	if (res[1] != -1)
		frm = frm + 1;
	cout << "Мой ход: "<< frm <<" "<< res[2] + 1 << endl;
	//cout << "Мои фишки в кармане: "<<c_chip<<" На поле: "<< f_c_chip << endl;
	cout << "Время раздумий компьютера: " << (f - s) / 1000.0 << " секунды" << endl;
	return res;
}

// Алгоритм альфа бетта отсечения
array<int, 3> ab_clipping(vector<int>& b, unsigned int deep, int alf, int bet, unsigned int p, vector<array<int, 3>>& puMill, vector<array<int, 3>>& cuMill, int p_chip, int f_p_chip, int c_chip, int f_c_chip, bool Mill_phase)
{
	if (deep == 0)
		return array<int, 3>{combine_heuristic(b, COMPUTER, cuMill, puMill), -1, -1};
	if (p == COMPUTER)
	{
		array<int, 3> moving = { INT_MIN, -1,-1 };// moving;
		unsigned int winner = 0;
		//if (game_over(b, p_chip, c_chip))
		//return moving;
		if (Have_winner(b, winner, p_chip, c_chip))
		{
			if (winner == PLAYER)
				return moving;
		}
			
		//Жрём фишки
		if (Mill_phase)
		{
			vector<array<int, 3>> NewUsedMills = copy(cuMill);
			if (CheckMill(b, p, NewUsedMills))
			{
				vector<int> En_pos = vector<int>();
				for (int i = 0; i < 24; i++)
				{
					if (b[i] == PLAYER)
						En_pos.push_back(i);
				}
				
				for (int i = 0; i < En_pos.size(); i++)
				{
					vector<int> newBoard = copy(b);
					newBoard[En_pos[i]] = 0;
					
					int score = ab_clipping(newBoard, deep - 1, alf, bet, PLAYER, puMill, NewUsedMills, p_chip, f_p_chip -1, c_chip, f_c_chip, false)[0];
					if (score > moving[0])
						moving = { score, En_pos[i], -1 };
					alf = max(alf, moving[0]);
					if (alf >= bet)
						break;
				}
			}
			else //Нет мельниц
			{
				return ab_clipping(b, deep - 1, alf, bet, PLAYER, puMill, cuMill, p_chip, f_p_chip, c_chip, f_c_chip, false);
			}
		}
		else
		{
			if (c_chip > 0)
			{
				//Ставим фишки
				for (int i = 0; i < 24; i++)
				{
					if (b[i] == 0)
					{
						int cu_chip = c_chip;
						int f_cu_chip = f_c_chip;
						vector<int> newBoard = copy(b);
						make_move(newBoard, -1, i, COMPUTER, cu_chip, f_cu_chip);
						int score = ab_clipping(newBoard, deep, alf, bet, COMPUTER, puMill, cuMill, p_chip, f_p_chip, cu_chip, f_cu_chip, true)[0];
						if (score > moving[0])
							moving = { score, -1, i };
						alf = max(alf, moving[0]);
						if (alf >= bet)
							break;
					}
				}
			}
			else
			{
				//Двигаем фишки
				if (f_c_chip > 3)
				{
					//from
					for (int i = 0; i < f_size; i++)
					{
						//to
						for (int j = 0; j < f_size; j++)
						{
							if (Can_Move(b, i, j, COMPUTER, c_chip, f_c_chip))
							{
								vector<int> newBoard = copy(b);
								make_move(newBoard, i, j, COMPUTER, c_chip, f_c_chip);
								int score = ab_clipping(newBoard, deep, alf, bet, COMPUTER, puMill, cuMill, p_chip, f_p_chip, c_chip, f_c_chip, true)[0];
								if (score > moving[0])
									moving = { score, i, j };
								alf = max(alf, moving[0]);
								if (alf >= bet)
									break;
							}
						}
					}
				}
				else
				{
					vector<int> My_Chips = vector<int>();
					for (int i = 0; i < f_size; i++)
					{
						if (b[i] == COMPUTER)
							My_Chips.push_back(i);
					}

					for (int i = 0; i < My_Chips.size(); i++)
					{
						for (int j = 0; j < f_size; j++)
						{
							if (Can_Move(b, My_Chips[i], j, COMPUTER, c_chip, f_c_chip))
							{
								vector<int> newBoard = copy(b);
								make_move(newBoard, My_Chips[i], j, COMPUTER, c_chip, f_c_chip);
								int score = ab_clipping(newBoard, deep, alf, bet, COMPUTER, puMill, cuMill, p_chip, f_p_chip, c_chip, f_c_chip, true)[0];
								if (score > moving[0])
									moving = { score, My_Chips[i], j };
								alf = max(alf, moving[0]);
								if (alf >= bet)
									break;
							}
						}
					}
				}
				
			}
		}
		
		
		return moving;
	}
	else
	{
		array<int, 3> moving = { INT_MAX, -1, -1 };
		unsigned int winner = 0;
		if (Have_winner(b, winner, p_chip, c_chip))
		{
			if (winner == COMPUTER)
				return moving;
		}
		
		//Жрём фишки
		if (Mill_phase)
		{
			vector<array<int, 3>> NewUsedMills = copy(puMill);
			if (CheckMill(b, p, NewUsedMills))
			{
				vector<int> En_pos = vector<int>();
				for (int i = 0; i < 24; i++)
				{
					if (b[i] == COMPUTER)
						En_pos.push_back(i);
				}

				for (int i = 0; i < En_pos.size(); i++)
				{
					vector<int> newBoard = copy(b);
					newBoard[En_pos[i]] = 0;

					int score = ab_clipping(newBoard, deep - 1, alf, bet, COMPUTER, NewUsedMills, cuMill, p_chip, f_p_chip, c_chip, f_c_chip - 1, false)[0];
					if (score < moving[0])
						moving = { score, En_pos[i], -1 };
					bet = min(bet, moving[0]);
					if (alf >= bet)
						break;
				}
			}
			else //Нет мельниц
			{
				return ab_clipping(b, deep - 1, alf, bet, COMPUTER, puMill, cuMill, p_chip, f_p_chip, c_chip, f_c_chip, false);
			}
		}
		else
		{
			if (p_chip > 0)
			{
				//Ставим фишки
				for (int i = 0; i < 24; i++)
				{
					if (b[i] == 0)
					{
						int pu_chip = p_chip;
						int f_pu_chip = f_p_chip;
						vector<int> newBoard = copy(b);
						make_move(newBoard, -1, i, PLAYER, pu_chip, f_pu_chip);
						int score = ab_clipping(newBoard, deep, alf, bet, PLAYER, puMill, cuMill, pu_chip, f_pu_chip, c_chip, f_c_chip, true)[0];
						if (score < moving[0])
							moving = { score, -1, i };
						bet = min(bet, moving[0]);
						if (alf >= bet)
							break;
					}
				}
			}
			else
			{
				//Двигаем фишки
				if (f_p_chip > 3) //На поле больше 3 фишек и нечего ставить
				{
					//from
					for (int i = 0; i < f_size; i++)
					{
						//to
						for (int j = 0; j < f_size; j++)
						{
							if (Can_Move(b, i, j, PLAYER, p_chip, f_p_chip))
							{
								vector<int> newBoard = copy(b);
								make_move(newBoard, i, j, PLAYER, p_chip, f_p_chip);
								int score = ab_clipping(newBoard, deep, alf, bet, PLAYER, puMill, cuMill, p_chip, f_p_chip, c_chip, f_c_chip, true)[0];
								if (score < moving[0])
									moving = { score, i, j };
								bet = min(bet, moving[0]);
								if (alf >= bet)
									break;
							}
						}
					}
				}
				else //на поле меньше 3 фишек и нечего ставить
				{
					vector<int> My_Chips = vector<int>();
					for (int i = 0; i < f_size; i++)
					{
						if (b[i] == PLAYER)
							My_Chips.push_back(i);
					}

					for (int i = 0; i < My_Chips.size(); i++)
					{
						for (int j = 0; j < f_size; j++)
						{
							if (Can_Move(b, My_Chips[i], j, PLAYER, p_chip, f_p_chip))
							{
								vector<int> newBoard = copy(b);
								make_move(newBoard, My_Chips[i], j, PLAYER, p_chip, f_p_chip);
								int score = ab_clipping(newBoard, deep, alf, bet, PLAYER, puMill, cuMill, p_chip, f_p_chip, c_chip, f_c_chip, true)[0];
								if (score < moving[0])
									moving = { score, My_Chips[i], j };
								bet = min(bet, moving[0]);
								if (alf >= bet)
									break;
							}
						}
					}
				}
				
			}
		}

		return moving;
	}
}

//Создаёт 16 возможных мельниц
void Create_Mills()
{
	array<int, 3> cuMill = array<int, 3>{0,1,2};
	all_Mills.push_back(cuMill);
	cuMill = array<int, 3>{2, 3, 4};
	all_Mills.push_back(cuMill);
	cuMill = array<int, 3>{4, 5, 6};
	all_Mills.push_back(cuMill);
	cuMill = array<int, 3>{6, 7, 0};
	all_Mills.push_back(cuMill);

	cuMill = array<int, 3>{8, 9, 10};
	all_Mills.push_back(cuMill);
	cuMill = array<int, 3>{10, 11, 12};
	all_Mills.push_back(cuMill);
	cuMill = array<int, 3>{12, 13, 14};
	all_Mills.push_back(cuMill);
	cuMill = array<int, 3>{14, 15, 8};
	all_Mills.push_back(cuMill);

	cuMill = array<int, 3>{16, 17, 18};
	all_Mills.push_back(cuMill);
	cuMill = array<int, 3>{18, 19, 20};
	all_Mills.push_back(cuMill);
	cuMill = array<int, 3>{20, 21, 22};
	all_Mills.push_back(cuMill);
	cuMill = array<int, 3>{22, 23, 16};
	all_Mills.push_back(cuMill);

	cuMill = array<int, 3>{1, 9, 17};
	all_Mills.push_back(cuMill);
	cuMill = array<int, 3>{7, 15, 23};
	all_Mills.push_back(cuMill);
	cuMill = array<int, 3>{5, 13, 21};
	all_Mills.push_back(cuMill);
	cuMill = array<int, 3>{3, 11, 19};
	all_Mills.push_back(cuMill);
}


void make_user_move(vector<int>& b, int& chip, int& f_chip)
{
	cout << endl;
	cout << "Ваш ход! Вводите откуда и куда переставить фишку. Если из кармана, то откуда = -1. Пропуск хода -9. " << endl;
	//cout << "У вас осталось фишек в кармане: "<<chip<<" И на поле: "<<f_chip << endl;
	int from = 0;
	int to = 0;
	bool correct = false;
	bool first = true;
	while (!correct)
	{
		cin >> from;
		if (from == -9)
			return;
		cin >> to;
		if (first)
		{
			if (from == -1)
			{
				if (chip > 0)
				{
					from++;
				}
				else
				{
					cout << "Нет фишек в кармане! делайте другой ход..." << endl;
				}

			}
			correct = make_move(b, from-1, to-1, PLAYER, chip, f_chip);
			if (!correct)
			{
				cout << "Невозможно так походить! Сделайте другой ход" << endl;
			}
			first = false;
		}
		else
		{
			cout << "Некорректный ход. Попробуйте ещё раз" << endl;
			correct = make_move(b, from-1, to-1, PLAYER, chip, f_chip);
		}
	}
}

// Сделать ход (b - поле, откуда, куда, фишки в кармане, p - игрок, фишки на поле)
bool make_move(vector<int>& b, int from, int to, unsigned int p, int& chip, int& f_chip)
{
	//Первая стадия игры. Игрок ставит фишки из кармана
	if (from == -1)
	{
		if (chip == 0)
			return false;
		else
		{
			if (b[to] != 0)
				return false;
			else
			{
				b[to] = p;
				chip--;
				f_chip++;
				return true;
			}
		}
	}
	else
	{
		//Завершающая стадия игры. У игрока 3 фишки
		if (f_chip == 3)
		{
			if (b[from] != p)
				return false;
			if (b[to] != 0)
				return false;
			else
			{
				b[from] = 0;
				b[to] = p;
				return true;
			}
		}
		else
		{
			//Основная часть игры
			//Если назначение занято, то хода нет
			if (b[to] != 0)
				return false;
			//Делать ход из пустой или вражеской клетки нельзя
			if (b[from] != p)
				return false;
			if (from == 0)
			{
				if (to == 1 || to == 7)
				{
					b[from] = 0;
					b[to] = p;
					return true;
				}
				else
					return false;
			}

			if (from == 1)
			{
				if (to == 0 || to == 2 || to == 9)
				{
					b[from] = 0;
					b[to] = p;
					return true;
				}
				else
					return false;
			}

			if (from == 2)
			{
				if (to == 1 || to == 3)
				{
					b[from] = 0;
					b[to] = p;
					return true;
				}
				else
					return false;
			}

			if (from == 3)
			{
				if (to == 2 || to == 4 || to == 11)
				{
					b[from] = 0;
					b[to] = p;
					return true;
				}
				else
					return false;
			}

			if (from == 4)
			{
				if (to == 3 || to == 5)
				{
					b[from] = 0;
					b[to] = p;
					return true;
				}
				else
					return false;
			}

			if (from == 5)
			{
				if (to == 4 || to == 6 || to == 13)
				{
					b[from] = 0;
					b[to] = p;
					return true;
				}
				else
					return false;
			}
			if (from == 6)
			{
				if (to == 5 || to == 7)
				{
					b[from] = 0;
					b[to] = p;
					return true;
				}
				else
					return false;
			}
			if (from == 7)
			{
				if (to == 0 || to == 6 || to == 15)
				{
					b[from] = 0;
					b[to] = p;
					return true;
				}
				else
					return false;
			}
			if (from == 8)
			{
				if (to == 15 || to == 9)
				{
					b[from] = 0;
					b[to] = p;
					return true;
				}
				else
					return false;
			}
			if (from == 9)
			{
				if (to == 1 || to == 8 || to == 10 || to == 17)
				{
					b[from] = 0;
					b[to] = p;
					return true;
				}
				else
					return false;
			}
			if (from == 10)
			{
				if (to == 11 || to == 9)
				{
					b[from] = 0;
					b[to] = p;
					return true;
				}
				else
					return false;
			}
			if (from == 11)
			{
				if (to == 3 || to == 19 || to == 10 || to == 12)
				{
					b[from] = 0;
					b[to] = p;
					return true;
				}
				else
					return false;
			}
			if (from == 12)
			{
				if (to == 11 || to == 13)
				{
					b[from] = 0;
					b[to] = p;
					return true;
				}
				else
					return false;
			}
			if (from == 13)
			{
				if (to == 12 || to == 14 || to == 5 || to == 21)
				{
					b[from] = 0;
					b[to] = p;
					return true;
				}
				else
					return false;
			}
			if (from == 14)
			{
				if (to == 13 || to == 15)
				{
					b[from] = 0;
					b[to] = p;
					return true;
				}
				else
					return false;
			}
			if (from == 15)
			{
				if (to == 14 || to == 8 || to == 7 || to ==23)
				{
					b[from] = 0;
					b[to] = p;
					return true;
				}
				else
					return false;
			}
			if (from == 16)
			{
				if (to == 23 || to == 17)
				{
					b[from] = 0;
					b[to] = p;
					return true;
				}
				else
					return false;
			}
			if (from == 17)
			{
				if (to == 16 || to == 18 || to ==9)
				{
					b[from] = 0;
					b[to] = p;
					return true;
				}
				else
					return false;
			}
			if (from == 18)
			{
				if (to == 19 || to == 17)
				{
					b[from] = 0;
					b[to] = p;
					return true;
				}
				else
					return false;
			}
			if (from == 19)
			{
				if (to == 18 || to == 11 || to ==20)
				{
					b[from] = 0;
					b[to] = p;
					return true;
				}
				else
					return false;
			}
			if (from == 20)
			{
				if (to == 19 || to == 21)
				{
					b[from] = 0;
					b[to] = p;
					return true;
				}
				else
					return false;
			}
			if (from == 21)
			{
				if (to == 13 || to == 20 || to ==22)
				{
					b[from] = 0;
					b[to] = p;
					return true;
				}
				else
					return false;
			}
			if (from == 22)
			{
				if (to == 21 || to == 23)
				{
					b[from] = 0;
					b[to] = p;
					return true;
				}
				else
					return false;
			}
			if (from == 23)
			{
				if (to == 16 || to == 22 || to ==15)
				{
					b[from] = 0;
					b[to] = p;
					return true;
				}
				else
					return false;
			}

			return false;
		}
	}

	return true;
}

bool Can_Move(vector<int>& b, int from, int to, unsigned int p, int& chip, int& f_chip)
{
	//Первая стадия игры. Игрок ставит фишки из кармана
	if (from == -1)
	{
		if (chip == 0)
			return false;
		else
		{
			if (b[to] != 0)
				return false;
			else
			{
				
				return true;
			}
		}
	}
	else
	{
		//Завершающая стадия игры. У игрока 3 фишки
		if (chip == 3)
		{
			if (b[from] != p)
				return false;
			if (b[to] != 0)
				return false;
			else
			{
				
				return true;
			}
		}
		else
		{
			//Если назначение занято, то хода нет
			if (b[to] != 0)
				return false;
			//Делать ход из пустой или вражеской клетки нельзя
			if (b[from] != p)
				return false;
			if (from == 0)
			{
				if (to == 1 || to == 7)
				{
					return true;
				}
				else
					return false;
			}

			if (from == 1)
			{
				if (to == 0 || to == 2 || to == 9)
				{
					return true;
				}
				else
					return false;
			}

			if (from == 2)
			{
				if (to == 1 || to == 3)
				{
					return true;
				}
				else
					return false;
			}

			if (from == 3)
			{
				if (to == 2 || to == 4 || to == 11)
				{
					return true;
				}
				else
					return false;
			}

			if (from == 4)
			{
				if (to == 3 || to == 5)
				{
					return true;
				}
				else
					return false;
			}

			if (from == 5)
			{
				if (to == 4 || to == 6 || to == 13)
				{
					return true;
				}
				else
					return false;
			}
			if (from == 6)
			{
				if (to == 5 || to == 7)
				{
					return true;
				}
				else
					return false;
			}
			if (from == 7)
			{
				if (to == 0 || to == 6 || to == 15)
				{
					return true;
				}
				else
					return false;
			}
			if (from == 8)
			{
				if (to == 15 || to == 9)
				{
					return true;
				}
				else
					return false;
			}
			if (from == 9)
			{
				if (to == 1 || to == 8 || to == 10 || to == 17)
				{
					return true;
				}
				else
					return false;
			}
			if (from == 10)
			{
				if (to == 11 || to == 9)
				{
					return true;
				}
				else
					return false;
			}
			if (from == 11)
			{
				if (to == 3 || to == 19 || to == 10 || to == 12)
				{
					return true;
				}
				else
					return false;
			}
			if (from == 12)
			{
				if (to == 11 || to == 13)
				{
					return true;
				}
				else
					return false;
			}
			if (from == 13)
			{
				if (to == 12 || to == 14 || to == 5 || to == 21)
				{
					return true;
				}
				else
					return false;
			}
			if (from == 14)
			{
				if (to == 13 || to == 15)
				{
					return true;
				}
				else
					return false;
			}
			if (from == 15)
			{
				if (to == 14 || to == 8 || to == 7 || to == 23)
				{
					return true;
				}
				else
					return false;
			}
			if (from == 16)
			{
				if (to == 23 || to == 17)
				{
					return true;
				}
				else
					return false;
			}
			if (from == 17)
			{
				if (to == 16 || to == 18 || to == 9)
				{
					return true;
				}
				else
					return false;
			}
			if (from == 18)
			{
				if (to == 19 || to == 17)
				{
					return true;
				}
				else
					return false;
			}
			if (from == 19)
			{
				if (to == 18 || to == 11 || to == 20)
				{
					return true;
				}
				else
					return false;
			}
			if (from == 20)
			{
				if (to == 19 || to == 21)
				{
					return true;
				}
				else
					return false;
			}
			if (from == 21)
			{
				if (to == 13 || to == 20 || to == 22)
				{
					return true;
				}
				else
					return false;
			}
			if (from == 22)
			{
				if (to == 21 || to == 23)
				{
					return true;
				}
				else
					return false;
			}
			if (from == 23)
			{
				if (to == 16 || to == 22 || to == 15)
				{
					return true;
				}
				else
					return false;
			}
		}
	}
	return true;
}

//ищет собранные И НЕИСПОЛЬЗОВАННЫЕ мельницы. Принимает доску, игрока и его собранные мельницы
bool CheckMill(vector<int>& b, unsigned int p, vector<array<int, 3>>& puMill)
{
	for (int i = 0; i < 16; i++)
	{
		if (puMill.size() == 0)
		{
			if (b[all_Mills[i][0]] == p && b[all_Mills[i][1]] == p && b[all_Mills[i][2]] == p)
			{
				puMill.push_back(all_Mills[i]);
				return true;
			}
		}
		else
		if (!MillsContains(puMill,all_Mills[i]))
			if (b[all_Mills[i][0]] == p && b[all_Mills[i][1]] == p && b[all_Mills[i][2]] == p)
			{
				puMill.push_back(all_Mills[i]);
				return true;
			}
	}
	return false;
}

//Принимает использованные мельницы и текущую. Если текущая в использованных, то истина. Иначе - ложь
bool MillsContains(vector<array<int, 3>>& puMill, array<int, 3> mill)
{
	bool res = false;
	for (int i = 0; i < puMill.size(); i++)
	{
		if (mill == puMill[i])
			return true;
	}

	return res;
}

void UserEat(vector<int>& b)
{
	cout << "У вас мельница. Что кушать изволите? (номер позиции)" << endl;
	int target = 0;
	cin >> target;
	target--;
	while (target < 0 || target > 23)
	{
		cout << "Некорректный ввод! Попробуйте ещё раз" << endl;
		cin >> target;
		target--;
	}

	while (b[target] != COMPUTER)
	{
		cout << "На данной позиции нет противника! Попробуйте ещё раз" << endl;
		cin >> target;
		target--;
	}

	b[target] = 0;
}

//ест фишку врага оптимальным образом
//Стратегия - сначала искать где 2 фишки врага и одна пустая. Потом где 2 наши и одна вражья, потом убирать любую вражескую. Потом посчитать сколько у врага есть. Менее 3 - победа
void EatEnemy(vector<int>& b, unsigned int p, vector<array<int, 3>>& puMill, vector<array<int, 3>>& euMill)
{
	unsigned int en = (COMPUTER == p) ? 1 : 2;

	//Убрать выгодные расклады врага
	for (int i = 0; i < 16; i++)
	{
		bool Used = false;
		for (int j = 0; j < euMill.size(); j++)
		{
			Used = euMill[j] == all_Mills[i];
			if (Used)
				goto skip2;
		}
		if (b[all_Mills[i][0]] == en && b[all_Mills[i][1]] == en && b[all_Mills[i][2]] == 0 || b[all_Mills[i][0]] == 0 && b[all_Mills[i][1]] == en && b[all_Mills[i][2]] == en)
		{
			b[all_Mills[i][1]] = 0;
			return;
		}
		if (b[all_Mills[i][0]] == en && b[all_Mills[i][1]] == 0 && b[all_Mills[i][2]] == en)
		{
			b[all_Mills[i][0]] = 0;
			return;
		}
	skip2:;
	}

	//Почистить себе место
	for (int i = 0; i < 16; i++)
	{
		bool Used = false;
		for (int j = 0; j < puMill.size(); j++)
		{
			Used = puMill[j] == all_Mills[i];
			if (Used)
				goto skip;
		}
		if (b[all_Mills[i][0]] == en && b[all_Mills[i][1]] == p && b[all_Mills[i][2]] == p)
		{
			b[all_Mills[i][0]] = 0;
			return;
		}
		if (b[all_Mills[i][0]] == p && b[all_Mills[i][1]] == en && b[all_Mills[i][2]] == p)
		{
			b[all_Mills[i][1]] = 0;
			return;
		}
		if (b[all_Mills[i][0]] == p && b[all_Mills[i][1]] == p && b[all_Mills[i][2]] == en)
		{
			b[all_Mills[i][2]] = 0;
			return;
		}
	skip:;
	}

	//убрать любую
	for (int i = 0; i < 24; i++)
	{
		if (b[i] == en)
		{
			b[i] = 0;
			return;
		}
	}
}

//Комбинированная эвристика
int combine_heuristic(vector<int>& b, unsigned int p, vector<array<int, 3>>& puMill, vector<array<int, 3>>& euMill)
{
	//int impr = impr_heuristic(b, p, puMill, euMill);
	int simple = Simple_Heuristic(b, p);

	//return impr * 2 + simple;
	return simple;
}

//Усложнённая на шаблонах и мельницах
int impr_heuristic(vector<int>& b, unsigned int p, vector<array<int, 3>>& puMill, vector<array<int, 3>>& euMill)
{
	unsigned int en = (COMPUTER == p) ? 1 : 2;

	int res_p = 0;
	int res_en = 0;

	//Подсчитать потенциально полезные фишки игрока
	for (int i = 0; i < all_Mills.size(); i++)
	{
		bool used = false;
		int mip = 0;
		int eip = 0;
		int zip = 0;
		for (int j = 0; j < puMill.size(); j++)
		{
			used = puMill[j] == all_Mills[i];
			if (used)
				goto cont;
		}

		for (int j = 0; j < 3; j++)
		{
			if (b[all_Mills[i][j]] == p)
				res_p++;
			/*if (b[all_Mills[i][j]] == p)
				mip++;
			if (b[all_Mills[i][j]] == en)
				eip++;
			if (b[all_Mills[i][j]] == 0)
				zip++;*/
		}

		/*if (mip == 3)
			res_p += 30;
		if (mip == 2 && zip == 1)
			res_p += 25;
		if (mip == 2 && eip == 1)
			res_p += 10;
		if (mip == 1 && zip == 2)
			res_p += 15;
		if (mip == 1 && zip == 1 && eip == 1)
			res_p += 7;
		if (mip == 1 && eip == 2)
			res_p += 16;
		//if (zip == 1 && eip == 2)
		//	res_p -=*/

	cont:;
	}

	//Подсчёт потенциально полезных фишек врага
	for (int i = 0; i < all_Mills.size(); i++)
	{
		bool used = false;
		for (int j = 0; j < euMill.size(); j++)
		{
			used = euMill[j] == all_Mills[i];
			if (used)
				goto cont2;
		}

		for (int j = 0; j < 3; j++)
		{
			if (b[all_Mills[i][j]] == en)
				res_en++;
		}

	cont2:;
	}

	return res_p - res_en;
}

//Простейшая эвристика
int Simple_Heuristic(vector<int>& b, unsigned int p)
{
	unsigned int en = (COMPUTER == p) ? 1 : 2;
	int res_p = 0;
	int res_en = 0;
	for (int i = 0; i < f_size; i++)
	{
		if (b[i] == en)
		{
			res_en++;
		}
		else
		{
			if (b[i] == p)
				res_p++;
		}
	}

	if (res_p < 3)
		return -999;

	return res_p - res_en;
}

bool game_over(vector<int>& b, int p_chip, int c_chip)
{
	int res_p = p_chip;
	int res_c = c_chip;
	for (int i = 0; i < f_size; i++)
	{
		if (b[i] == COMPUTER)
		{
			res_c++;
		}
		else
		{
			if (b[i] == PLAYER)
				res_p++;
		}
	}

	return res_p < 3 || res_c < 3;
}

// Создать доску
void create_field() 
{
	for (unsigned int r = 0; r < f_size; r++)
		field.push_back(0);// [r] = 0;
}

// Создать копию доски
vector<int> copy(vector<int> b) 
{
	vector<int> newBoard = vector<int>();//(f_size,0);
	for (unsigned int r = 0; r < f_size; r++)
		newBoard.push_back(b[r]);
		//newBoard[r] = b[r];
	return newBoard;
}

//Копировать вектор использованных
vector<array<int, 3>> copy(vector<array<int, 3>> b)
{
	vector<array<int, 3>> New = vector<array<int, 3>>();
	for (int i = 0; i < b.size(); i++)
	{
		New.push_back (b[i]);
	}

	return New;
}

//Определить победителя
bool Have_winner(vector<int>& b, unsigned int& winner, int& p_chip, int& c_chip)
{
	int res_p = p_chip;
	int res_c = c_chip;
	for (int i = 0; i < f_size; i++)
	{
		if (b[i] == COMPUTER)
		{
			res_c++;
		}
		else
		{
			if (b[i] == PLAYER)
				res_p++;
		}
	}

	if (res_p < 3)
	{
		winner = COMPUTER;
		return true;
	}
	if (res_c < 3)
	{
		winner = PLAYER;
		return true;
	}

	return false;
}

// Печатает доску
void print_field(vector<int>& b) 
{
	cout << endl;
	cout << "                     " << endl;
	cout << "                     " << endl;
	cout << "  " <<"1   "<< b[0]<< "---------------" << "2   " <<b[1]<< "---------------" << "3   " << b[2]<<"  "<<endl;
	cout << "      " << "|" << "                   " << "|" << "       " << "            |" << "  " << endl;
	cout << "      " << "|" << "                   " << "|" << "       " << "            |" << "  " << endl;
	cout << "      " << "|" << "                   " << "|" << "       " << "            |" << "  " << endl;
	cout << "      " << "|" << "   " << "9   " <<b[8]<<"-------" << "10  " << b[9] << "-------" << "11  " << b[10] << "       " << "|" << "  " << endl;
	cout << "      " << "|" << "       |   " << "        |" << "           |   " << "    |" << "  " << endl;
	cout << "      " << "|" << "       |   " << "        |" << "           |   " << "    |" << "  " << endl;
	cout << "      " << "|" << "       | " << " 17 " << b[16] << "-" << "-18 " << b[17] << "-" << "-19 " << b[18] << "     |   " << "    |" << "  " << endl;
	cout << "      " << "|" << "       |     | " << "          |" << "     |     " << "  |" << "  " << endl;
	/**/cout << "  " << "8   " << b[7] << "   " << "16  " << b[15] << "-" << "-24 " <<b[23]<<"   " << "     20 " << b[19] << "-" << "-12 " << b[11] << "   " << "4   " << b[3] << "  " << endl;
	cout << "      " << "|" << "       |     | " << "          |" << "     |     " << "  |" << "  " << endl;
	cout << "      " << "|" << "       | " << "-23 " << b[22] << "-" << "-22 " << b[21] << "-" << "-21 " << b[20] << "     |   " << "    |" << "  " << endl;
	cout << "      " << "|" << "       |   " << "        |" << "           |   " << "    |" << "  " << endl;
	cout << "      " << "|" << "       |   " << "        |" << "           |   " << "    |" << "  " << endl;
	cout << "      " << "|" << "   " << "15  " << b[14] << "-------" << "14  " << b[13] << "-------" << "13  " << b[12] << "       " << "|" << "  " << endl;
	cout << "      " << "|" << "                   " << "|" << "       " << "            |" << "  " << endl;
	cout << "      " << "|" << "                   " << "|" << "       " << "            |" << "  " << endl;
	cout << "      " << "|" << "                   " << "|" << "       " << "            |" << "  " << endl;
	cout << "  " << "7   " << b[6] << "---------------" << "6   " << b[5] << "---------------" << "5   " << b[4] << "  " << endl;
	cout << "                     " << endl;
	cout << "                     " << endl;
	cout << endl;
}
