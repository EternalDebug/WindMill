#pragma once
#ifndef WINDMILL_H
#define WINDMILL_H

#include <stdio.h>
#include <iostream>
#include <vector>
#include <limits.h>
#include <array>
#include <sstream>

// функции min и max
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

using namespace std;

// Создать поле
void create_field();

// Вывести поле
void print_field(vector<int>& b);

//Начать игру
void play();

array<int, 3> computer_move(vector<array<int, 3>>& puMill, vector<array<int, 3>>& cuMill, int p_chip, int f_p_chip, int c_chip, int f_c_chip, bool Mill_phase);

array<int, 3> ab_clipping(vector<int>& b, unsigned int deep, int alf, int bet, unsigned int p, vector<array<int, 3>>& puMill, vector<array<int, 3>>& cuMill, int p_chip, int f_p_chip, int c_chip, int f_c_chip, bool Mill_phase);

void Create_Mills();

void make_user_move(vector<int>& b, int& chip, int& f_chip);

bool make_move(vector<int>& b, int from, int to, unsigned int p, int& chip, int& f_chip);

bool Can_Move(vector<int>& b, int from, int to, unsigned int p, int& chip, int& f_chip);

bool CheckMill(vector<int>& b, unsigned int p, vector<array<int, 3>>& puMill);

void UserEat(vector<int>& b);

void EatEnemy(vector<int>& b, unsigned int p, vector<array<int, 3>>& puMill, vector<array<int, 3>>& euMill);

int combine_heuristic(vector<int>& b, unsigned int p, vector<array<int, 3>>& puMill, vector<array<int, 3>>& euMill);

int impr_heuristic(vector<int>& b, unsigned int p, vector<array<int, 3>>& puMill, vector<array<int, 3>>& euMill);

int Simple_Heuristic(vector<int>& b, unsigned int p);

bool game_over(vector<int>& b, int p_chip, int c_chip);

vector<int> copy(vector<int> b);

vector<array<int, 3>> copy(vector<array<int, 3>> b);

bool Have_winner(vector<int>& b, unsigned int& winner, int& p_chip, int& c_chip);

bool MillsContains(vector<array<int, 3>>& puMill, array<int, 3> mill);


#endif // !WINDMILL_H
