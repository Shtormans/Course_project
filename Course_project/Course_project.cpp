﻿#include <iostream>
#include <string>
#include <conio.h>
#include <windows.h>
#include <iomanip>
#include <fstream>
#include <Vector>
#include <filesystem>
#include <locale>

struct Item {
	char place[20];
	char type[19];
	float weight;
	char quality[8];
	char result[22];
	char date[11];
};

enum task_by
{
	number,
	place,
	type,
	weight,
	quality,
	result,
	date,
	not_include
};

enum sort_by
{
	ascending,
	descending
};

FILE* f;
errno_t err;

HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
COORD coord = { 0, 0 };

const std::string file_name = "info";

void program_menu();
void show_items();
void create_item();
void delete_item();
void make_request();
void edit_item();
void delete_by_number(task_by show_by2 = task_by::not_include, std::string value2 = "");
void sort_items();
void delete_by_type();
std::string get_type();
std::string get_quality();
std::string get_result(std::string, std::string);
std::string check_place(std::string, char, std::string&);
std::string check_weight(std::string, char, bool&, std::string&);
std::string get_date(std::string, char, std::string&);
bool check_date(std::string, std::string&);

void gotoxy(short x, short y)
{
	coord = { x, y };
	SetConsoleCursorPosition(hStdOut, coord);
}

void textcolor(WORD color)
{
	SetConsoleTextAttribute(hStdOut, color);
}

void print_error_message(std::string error_message)
{
	textcolor(4);
	std::cout << error_message << std::endl;
	textcolor(7);
}

void clear_line(short y)
{
	std::cout << std::setfill(' ')
		<< std::setw(100)
		<< " ";
	gotoxy(coord.X, y);
}

int file_length()
{
	Item item;
	int length = 0;

	err = fopen_s(&f, file_name.c_str(), "rb");
	while (fread(&item, sizeof(Item), 1, f) == 1)
	{
		length++;
	}

	fclose(f);
	return length;
}

char* create_arrow_keys(int length)
{
	char* arrows = new char[length];
	for (int i = 0; i < length; i++)
	{
		arrows[i] = ' ';
	}
	arrows[0] = '>';
	return arrows;
}

int get_arrows_index(char* arrows, int length)
{
	for (int i = 0; i < length; i++)
	{
		if (arrows[i] == '>')
		{
			return i;
		}
	}
	return -1;
}

void move_arrow(char* arrows, int length, int increment)
{
	int index = get_arrows_index(arrows, length);
	if (index + increment < length && index + increment >= 0)
	{
		arrows[index] = ' ';
		arrows[index + increment] = '>';
	}
}

int wait_key_input(char* arrows, int length)
{
	char ch = _getch();
	if (ch == -32)
	{
		switch (_getch())
		{
		case 72:
			move_arrow(arrows, length, -1);
			break;

		case 80:
			move_arrow(arrows, length, 1);
			break;
		}
	}
	else if (ch == 13)
	{
		return 1;
	}
	return 0;
}

int arrow_keys(std::string menu[], char* arrows, int length)
{
	for (int i = 0; i < length; i++)
	{
		std::cout << " " << arrows[i] << " " << menu[i] << std::endl;
	}

	if (wait_key_input(arrows, length))
	{
		return get_arrows_index(arrows, length);
	}

	return -1;
}

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	program_menu();

	system("pause");
}

int first_menu()
{
	std::string menu[2] = { "Створити дані", "Вийти з програми" };
	char* arrows = create_arrow_keys(2);

	while (true)
	{
		system("cls");
		std::cout << "              ГОЛОВНЕ МЕНЮ              " << std::endl << std::endl;

		int task = arrow_keys(menu, arrows, 2);

		if (task != -1)
		{
			switch (task)
			{
			case 0:
				create_item();
				delete[] arrows;
				return 0;
			case 1:
				return 1;
			}
		}
	}
}

void program_menu()
{
	Item item;
start:
	err = fopen_s(&f, file_name.c_str(), "rb");
	if (err != 0 || fread(&item, sizeof(Item), 1, f) == 0)
	{
		if (err != 0)
		{
			err = fopen_s(&f, file_name.c_str(), "ab+");
		}
		fclose(f);

		if (first_menu())
		{
			return;
		}
	}
	else
	{
		fclose(f);
	}

	std::string menu[7] = { "Переглянути дані",  "Додати дані",  "Створити запит", "Видалити з файлу", "Сортувати дані", "Редагувати дані", "Вийти з програми" };
	char* arrows = create_arrow_keys(7);

	while (true)
	{
		system("cls");
		std::cout << "              ГОЛОВНЕ МЕНЮ              " << std::endl << std::endl;

		int task = arrow_keys(menu, arrows, 7);

		if (task != -1)
		{
			switch (task)
			{
			case 0:
				system("cls");
				show_items();
				system("pause>0");
				break;
			case 1:
				create_item();
				break;
			case 2:
				make_request();
				break;
			case 3:
				delete_item();
				goto start;
			case 4:
				sort_items();
				break;
			case 5:
				edit_item();
				break;
			case 6:
				return;
			default:
				break;
			}
		}
	}

	delete[] arrows;
}

void vertical()
{
	SetConsoleCP(866);
	SetConsoleOutputCP(866);
	std::cout << "\xBA";
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
}

void table_start()
{
	SetConsoleCP(866);
	SetConsoleOutputCP(866);
	std::cout << std::endl << std::left << "\xC9\xCD\xCD\xCD\xCD\xCD" <<
		std::setfill('\xCD') << std::setw(24) << "\xCB" <<
		std::setfill('\xCD') << std::setw(21) << "\xCB" <<
		std::setfill('\xCD') << std::setw(18) << "\xCB" <<
		std::setfill('\xCD') << std::setw(10) << "\xCB" <<
		std::setfill('\xCD') << std::setw(26) << "\xCB" <<
		std::setfill('\xCD') << std::setw(14) << "\xCB" << "\xBB" << std::endl;
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	vertical();
	std::cout
		<< std::setfill(' ')
		<< std::left
		<< std::setw(5)
		<< "№";
	vertical();
	std::cout << std::left
		<< std::setw(23)
		<< "Місце прибуття";
	vertical();
	std::cout << std::left
		<< std::setw(20)
		<< "Тип";
	vertical();
	std::cout << std::left
		<< std::setw(17)
		<< "Тоннаж";
	vertical();
	std::cout << std::left
		<< std::setw(9)
		<< "Якість";
	vertical();
	std::cout << std::left
		<< std::setw(25)
		<< "Кінцева ціль";
	vertical();
	std::cout << std::left
		<< std::setw(13)
		<< "Дата прибуття";
	vertical();
	std::cout << std::endl;
}

void table_end()
{
	SetConsoleCP(866);
	SetConsoleOutputCP(866);
	std::cout << std::left << "\xC8\xCD\xCD\xCD\xCD\xCD" <<
		std::setfill('\xCD') << std::setw(24) << "\xCA" <<
		std::setfill('\xCD') << std::setw(21) << "\xCA" <<
		std::setfill('\xCD') << std::setw(18) << "\xCA" <<
		std::setfill('\xCD') << std::setw(10) << "\xCA" <<
		std::setfill('\xCD') << std::setw(26) << "\xCA" <<
		std::setfill('\xCD') << std::setw(14) << "\xCA" << "\xBC" << std::endl;
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
}

void show_item(Item item, int i)
{
	SetConsoleCP(866);
	SetConsoleOutputCP(866);
	std::cout << std::left << "\xCC\xCD\xCD\xCD\xCD\xCD" <<
		std::setfill('\xCD') << std::setw(24) << "\xCE" <<
		std::setfill('\xCD') << std::setw(21) << "\xCE" <<
		std::setfill('\xCD') << std::setw(18) << "\xCE" <<
		std::setfill('\xCD') << std::setw(10) << "\xCE" <<
		std::setfill('\xCD') << std::setw(26) << "\xCE" <<
		std::setfill('\xCD') << std::setw(14) << "\xCE" << "\xB9" << std::endl;
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	vertical();
	std::cout
		<< std::setfill(' ')
		<< std::left
		<< std::setw(5)
		<< i;
	vertical();
	std::cout << std::left
		<< std::setw(23)
		<< item.place;
	vertical();
	std::cout << std::left
		<< std::setw(20)
		<< item.type;
	vertical();
	std::cout << std::left
		<< std::setw(17)
		<< item.weight;
	vertical();
	std::cout << std::left
		<< std::setw(9)
		<< item.quality;
	vertical();
	std::cout << std::left
		<< std::setw(25)
		<< item.result;
	vertical();
	std::cout << std::left
		<< std::setw(13)
		<< item.date;
	vertical();
	std::cout << std::endl;
}

void show_items()
{
	Item item;
	err = fopen_s(&f, file_name.c_str(), "rb");
	int i = 1;

	table_start();
	while (fread(&item, sizeof(Item), 1, f) == 1)
	{
		show_item(item, i);
		i++;
	}
	table_end();

	fclose(f);
}

bool isCorrect(Item item, int num, task_by check_by, std::string value)
{
	switch (check_by)
	{
	case number:
		if (value != "" && num != stoi(value))
			return false;
		break;
	case place:
		if (std::string(item.place).find(value) == std::string::npos)
			return false;
		break;
	case type:
		if (std::string(item.type).find(value) == std::string::npos)
			return false;
		break;
	case weight:
		if (item.weight != stoi(value))
			return false;
		break;
	case quality:
		if (std::string(item.quality).find(value) == std::string::npos)
			return false;
		break;
	case result:
		if (std::string(item.result).find(value) == std::string::npos)
			return false;
		break;
	case date:
		if (std::string(item.date).find(value) == std::string::npos)
			return false;
		break;
	}
	return true;
}

bool show_items_by(task_by show_by, std::string value, task_by show_by2 = task_by::not_include, std::string value2 = "")
{
	Item item;
	err = fopen_s(&f, file_name.c_str(), "rb");
	int i = 0;
	int j = 0;

	bool item_exist = false;

	table_start();
	while (fread(&item, sizeof(Item), 1, f) == 1)
	{
		i++;
		j++;
		if (show_by2 != task_by::not_include)
		{
			if (!isCorrect(item, j, show_by, value) || !isCorrect(item, i, show_by2, value2))
			{
				if (isCorrect(item, i, show_by2, value2))
				{
					j++;
				}
				j--;
				continue;
			}
		}
		else
		{
			if (!isCorrect(item, i, show_by, value))
			{
				if (show_by != task_by::number)
				{
					j--;
				}
				continue;
			}
		}

		item_exist = true;
		show_item(item, j);
	}
	table_end();

	fclose(f);

	return item_exist;
}

void print_example_date(std::string date, std::string example_date)
{
	textcolor(8);
	if (example_date.length() > date.length())
	{
		std::cout << example_date.substr(date.length());
	}
	textcolor(7);
}

void print_input_table(std::string place, std::string type, std::string weight, std::string quality, std::string date, std::string example_date)
{
	std::cout << "Місце прибуття: " << place << std::endl;
	std::cout << "Тип сировини: " << type << std::endl;
	std::cout << "Тонаж: " << weight << std::endl;
	std::cout << "Якість сировини: " << quality << std::endl;
	std::cout << "Результат: " << std::endl;
	std::cout << "Дата: " << date;
	textcolor(8);
	std::cout << example_date.substr(date.length()) << std::endl;
	textcolor(7);
}

void print_input_error_message(std::string& error_message)
{
	gotoxy(0, 8);
	clear_line(8);
	print_error_message(error_message);
	error_message = "";
}

void place_output(std::string place)
{
	gotoxy(16, 0);
	clear_line(0);
	std::cout << place;
}

void weight_output(std::string weight)
{
	gotoxy(7, 2);
	clear_line(2);
	std::cout << weight;
}

void date_output(std::string date, std::string example_date)
{
	gotoxy(6, 5);
	clear_line(5);
	std::cout << date;
	print_example_date(date, example_date);
	gotoxy(6 + date.length(), 5);
}

std::string input_place(std::string place = "")
{
	std::string error_message = "";
	while (true)
	{
		place_output(place);
		char symbol = _getch();
		if ((int)symbol == 13 && place.length() > 0)
		{
			if (place[place.length() - 1] == 32 || place[place.length() - 1] == 45)
			{
				place = place.substr(0, place.length() - 1);
			}
			place += "\0";
			break;
		}
		place = check_place(place, symbol, error_message);

		print_input_error_message(error_message);
	}

	return place;
}

std::string input_weight(std::string weight = "")
{
	std::string error_message = "";
	bool has_dot = false;
	while (true)
	{
		weight_output(weight);
		char symbol = _getch();
		if ((int)symbol == 13 && weight.length() > 0)
		{
			break;
		}
		weight = check_weight(weight, symbol, has_dot, error_message);

		print_input_error_message(error_message);
	}

	return weight;
}

std::string input_date(std::string example_date, std::string date = "")
{
	std::string error_message = "";

	while (true)
	{
		date_output(date, example_date);

		char symbol = _getch();

		date = get_date(date, symbol, error_message);
		if ((int)symbol == 13 && check_date(date, error_message))
		{
			break;
		}

		print_input_error_message(error_message);
	}

	return date;
}

Item input_item()
{
	Item item = { "", "", 0.0f, "", "", "" };
	int action = 0;
	std::string place = "";
	std::string type = "";
	std::string weight = "";
	std::string quality = "";
	std::string result = "";
	std::string date = "";
	std::string error_message = "";
	const std::string example_date = "01.01.2020";

	system("cls");

	print_input_table(place, type, weight, quality, date, example_date);

	place = input_place();

	type = type = get_type();
	system("cls");
	print_input_table(place, type, weight, quality, date, example_date);

	weight = input_weight();

	quality = get_quality();
	system("cls");
	print_input_table(place, type, weight, quality, date, example_date);

	gotoxy(11, 4);
	result = get_result(type, quality);
	std::cout << result;

	date = input_date(example_date);

	strcpy_s(item.place, place.c_str());
	strcpy_s(item.type, type.c_str());
	item.weight = stof(weight);
	strcpy_s(item.quality, quality.c_str());
	strcpy_s(item.result, result.c_str());
	strcpy_s(item.date, date.c_str());

	return item;
}

std::string check_place(std::string place, char symbol, std::string& error_message)
{
	if ((int)symbol == 8)
	{
		place = place.substr(0, place.length() - 1);
	}
	else
	{
		if (place.length() >= 20)
		{
			error_message = "Помилка: Кількість символів не повинна перевищувати 20 символів";
		}
		else if (place.length() == 0 && !(symbol >= -86 && symbol <= 1))
		{
			error_message = "Помилка: Назва міста повинна починатися з букви-кирилиці";
		}
		else if (!((symbol >= -86 && symbol <= 1) || symbol == 39 || symbol == 32 || symbol == 45))
		{
			error_message = "Помилка: Назва міста повинна бути написана кирилицею";
		}
		else if (place.length() > 0 && (symbol == 32 && (place[place.length() - 1] == 32 || place[place.length() - 1] == 45)) || (symbol == 45 && (place[place.length() - 1] == 32 || place[place.length() - 1] == 45)))
		{
			error_message = "Помилка: Назва міста повинна починатися з букви-кирилиці";
		}
		else
		{
			place += symbol;
			std::locale loc("Ukrainian");
			if (place.length() == 1 || place[place.length() - 2] == 32 || place[place.length() - 2] == 45)
			{
				if (std::islower(place[place.length() - 1], loc))
					place[place.length() - 1] = toupper(place[place.length() - 1], loc);
			}
			else if (!std::islower(place[place.length() - 1], loc))
				place[place.length() - 1] = tolower(place[place.length() - 1], loc);
		}
	}

	return place;
}

std::string check_weight(std::string weight, char symbol, bool& has_dot, std::string& error_message)
{
	if ((int)symbol == 8)
	{
		if (weight.length() > 0 && weight[weight.length() - 1] == '.')
		{
			has_dot = false;
		}
		weight = weight.substr(0, weight.length() - 1);
	}
	else
	{
		if (!((symbol >= 48 && symbol <= 57) || (symbol == '.' && weight.length() > 0 && !has_dot)))
		{
			error_message = "Помилка: Тонаж має бути числом";
			return weight;
		}

		if (symbol == '.' && !has_dot)
		{
			has_dot = true;
		}
		weight += symbol;

		if (weight.length() > 0 && !has_dot && stoi(weight) >= 10000)
		{
			weight = weight.substr(0, weight.length() - 1);
			error_message = "Помилка: Тоннаж не повинен бути більшим за 9999";
		}
		else if (weight.length() > 0 && has_dot && weight.length() - std::to_string(stoi(weight)).length() - 1 > 5)
		{
			weight = weight.substr(0, weight.length() - 1);
			error_message = "Помилка: Кількість символів після крапки не повинна перевищувати 5 символів";
		}
	}

	return weight;
}

std::string get_date(std::string date, char symbol, std::string& error_message)
{
	if ((int)symbol == 8)
	{
		int delete_length = 1;
		if (date.length() == 3 || date.length() == 6)
		{
			delete_length = 2;
		}
		date = date.substr(0, date.length() - delete_length);
	}
	else
	{
		if (!(symbol >= 48 && symbol <= 57))
		{
			error_message = "Помилка: Дата має бути записана тільки числами";
		}
		else if (date.length() > 9)
		{
			error_message = "Помилка: Кількість символів не повинна перевищувати 10 символів";
		}
		else
		{
			date += symbol;
			if (date.length() == 2 || date.length() == 5)
			{
				date += ".";
			}
		}
	}

	return date;
}

bool check_date(std::string date, std::string& error_message)
{
	if (date.length() != 10)
	{
		error_message = "Помилка: Такої дати не існує";
		return false;
	}

	int day = stoi(date.substr(0, 2));
	int month = stoi(date.substr(3, 2));
	int year = stoi(date.substr(6));

	if (year < 2000 || year > 2100)
	{
		error_message = "Помилка: Рік повинен бути меншим за 2101 та більшим за 1999";
		return false;
	}

	error_message = "Помилка: Такої дати не існує";

	if (month < 1 || month > 12)
		return false;
	if (day < 1 || day > 31)
		return false;

	if (month == 2)
	{
		if (((year % 4 == 0) &&
			(year % 100 != 0)) ||
			(year % 400 == 0))
			return (day <= 29);
		else
			return (day <= 28);
	}

	if (month == 4 || month == 6 ||
		month == 9 || month == 11)
		return (day <= 30);

	error_message = "";

	return true;
}

void create_item()
{
	Item item = input_item();

	err = fopen_s(&f, file_name.c_str(), "ab+");
	fwrite(&item, sizeof(Item), 1, f);

	fclose(f);
}

std::string get_type()
{
	std::string menu[7] = { "Метали та бляшанки\0",  "Картон та папір\0",  "Пластик\0", "Скло\0", "Олії\0", "Електроніка\0", "Аккумулятори\0" };
	char* arrows = create_arrow_keys(7);

	while (true)
	{
		system("cls");
		std::cout << "Оберіть тип сировини" << std::endl << std::endl;

		int task = arrow_keys(menu, arrows, 7);

		if (task != -1)
		{
			delete[] arrows;
			return menu[task];
		}
	}
}

std::string get_quality()
{
	std::string menu[3] = { "Висока\0",  "Середня\0",  "Низька\0" };
	char* arrows = create_arrow_keys(3);

	while (true)
	{
		system("cls");
		std::cout << "Оберіть якість сировини" << std::endl << std::endl;

		int task = arrow_keys(menu, arrows, 3);

		if (task != -1)
		{
			delete[] arrows;
			return menu[task];
		}
	}
}

std::string get_result(std::string type, std::string quality)
{
	if (type == "Метали та бляшанки")
	{
		if (quality == "Висока")
		{
			return "Метали\0";
		}
		else if (quality == "Середня")
		{
			return "Метали(0, 75)\0";
		}
		else if (quality == "Низька")
		{
			return "Утилізація\0";
		}
	}
	else if (type == "Картон та папір")
	{
		if (quality == "Висока")
		{
			return "Картон та папір(0,75)\0";
		}
		else if (quality == "Середня")
		{
			return "Картон та папір(0,50)\0";
		}
		else if (quality == "Низька")
		{
			return "Картон та папір(0,25)\0";
		}
	}
	else if (type == "Пластик")
	{
		if (quality == "Висока")
		{
			return "Пластикові гранули\0";
		}
		else if (quality == "Середня")
		{
			return "Пластикові гранули\0";
		}
		else if (quality == "Низька")
		{
			return "Пластикові гранули\0";
		}
	}
	else if (type == "Скло")
	{
		if (quality == "Висока")
		{
			return "Очищена тара\0";
		}
		else if (quality == "Середня")
		{
			return "Скляний лом\0";
		}
		else if (quality == "Низька")
		{
			return "Утилізація\0";
		}
	}
	else if (type == "Олії")
	{
		if (quality == "Висока")
		{
			return "Інші олії\0";
		}
		else if (quality == "Середня")
		{
			return "Технічні замазки\0";
		}
		else if (quality == "Низька")
		{
			return "Утилізація\0";
		}
	}
	else if (type == "Електроніка")
	{
		if (quality == "Висока")
		{
			return "Дорогоцінні метали\0";
		}
		else if (quality == "Середня")
		{
			return "Чорні метали\0";
		}
		else if (quality == "Низька")
		{
			return "Пластик\0";
		}
	}
	else if (type == "Аккумулятори")
	{
		if (quality == "Висока")
		{
			return "Хром та Нікель\0";
		}
		else if (quality == "Середня")
		{
			return "Кадмій\0";
		}
		else if (quality == "Низька")
		{
			return "Літій та свинець\0";
		}
	}
}

void delete_item()
{
	std::string menu[3] = { "Видалити за номером",  "Видалити за типом",  "Повернутися в меню" };
	char* arrows = create_arrow_keys(3);

	while (true)
	{
		system("cls");
		std::cout << "              Видалення даних             " << std::endl << std::endl;

		int task = arrow_keys(menu, arrows, 3);

		if (task != -1)
		{
			switch (task)
			{
			case 0:
				delete_by_number();
				break;
			case 1:
				delete_by_type();
				break;
			case 2:
				return;
			default:
				break;
			}
		}
	}

	delete[] arrows;
}

std::string input_number(char symbol, std::string number, int length, std::string& error_message)
{
	if ((int)symbol == 8 && number.length() > 0)
	{
		error_message = "";
		number = number.substr(0, number.length() - 1);
	}
	else if (!(symbol >= 48 && symbol <= 57))
	{
		error_message = "Помилка: Номер має бути числом";
	}
	else if (stoi(number + symbol) > length)
	{
		error_message = "Помилка: число має бути меншим за " + std::to_string(length + 1);
	}
	else
	{
		number += symbol;
	}

	return number;
}

void delete_by(task_by show_by, std::string value, task_by show_by2 = task_by::not_include, std::string value2 = "")
{
	Item item;
	FILE* f2;
	err = fopen_s(&f, file_name.c_str(), "rb");
	err = fopen_s(&f2, (file_name + "_copy").c_str(), "ab+");

	int i = 0;
	int j = 0;

	while (fread(&item, sizeof(Item), 1, f) == 1)
	{
		i++;
		j++;

		if (show_by2 != task_by::not_include)
		{
			if (isCorrect(item, j, show_by, value) && isCorrect(item, i, show_by2, value2))
			{
				continue;
			}
			if (isCorrect(item, i, show_by2, value2))
			{
				j++;
			}
			j--;
		}
		else
		{
			if (isCorrect(item, i, show_by, value))
			{
				continue;
			}
		}


		fwrite(&item, sizeof(Item), 1, f2);
	}

	fclose(f);
	fclose(f2);
	remove(file_name.c_str());
	rename((file_name + "_copy").c_str(), file_name.c_str());
}

bool ask_delete()
{
	std::string menu[2] = { "Так",  "Ні" };
	char* arrows = create_arrow_keys(2);

	while (true)
	{
		gotoxy(0, 0);
		clear_line(0);
		clear_line(1);
		clear_line(2);
		gotoxy(0, 0);
		std::cout << "Ви точно хочете видалити цей запис?" << std::endl << std::endl;

		int task = arrow_keys(menu, arrows, 2);

		if (task != -1)
		{
			switch (task)
			{
			case 0:
				return true;
			case 1:
				return false;
			default:
				break;
			}
		}
	}

	delete[] arrows;
}

void delete_by_number(task_by show_by2, std::string value2)
{
	std::string number = "";
	std::string error_message = "";

	int length = file_length();

	while (true)
	{
		system("cls");
		std::cout << "Уведіть номер, за яким хочете видалити: " << number << std::endl << std::endl;

		print_error_message(error_message);

		std::cout << std::endl;
		show_items_by(task_by::number, number, show_by2, value2);

		gotoxy(40 + number.length(), 0);

		char symbol = _getch();
		if (symbol == 13)
		{
			system("cls");
			if (number.length() > 0 && ask_delete())
			{
				delete_by(task_by::number, number, show_by2, value2);
			}
			break;
		}
		number = input_number(symbol, number, length, error_message);
	}
}

std::string get_type_to_delete()
{
	std::string menu[8] = { "Метали та бляшанки\0",  "Картон та папір\0",  "Пластик\0", "Скло\0", "Олії\0", "Електроніка\0", "Аккумулятори\0", "Вийти\0" };
	char* arrows = create_arrow_keys(8);

	while (true)
	{
		system("cls");
		std::cout << "Оберіть тип сировини" << std::endl << std::endl;

		int task = arrow_keys(menu, arrows, 8);

		if (task != -1)
		{
			delete[] arrows;
			return menu[task];
		}
	}
}

void delete_by_type()
{
	std::string error_message = "";
	std::string type_list[7] = { "Метали та бляшанки\0",  "Картон та папір\0",  "Пластик\0", "Скло\0", "Олії\0", "Електроніка\0", "Аккумулятори\0" };
	std::string type = type_list[0];

	int index = 0;

	bool item_exist = false;

	while (true)
	{
		error_message = "";
		system("cls");
		std::cout << "Тип товару, за яким хочете видалити: " << type << std::endl << std::endl;

		gotoxy(0, 2);
		clear_line(2);
		print_error_message(error_message);

		std::cout << std::endl;
		item_exist = show_items_by(task_by::type, type);
		if (!item_exist)
		{
			error_message = "Помилка: Даних з типом " + type + "не існує";
			gotoxy(0, 2);
			clear_line(2);
			print_error_message(error_message);
		}

		char symbol = _getch();
		if (symbol == -32)
		{
			switch (_getch())
			{
			case 72:
				index--;
				if (index < 0)
				{
					index = 6;
				}
				type = type_list[index];
				break;

			case 80:
				index++;
				if (index > 6)
				{
					index = 0;
				}
				type = type_list[index];
				break;
			}
		}
		if (symbol == 13)
		{
			if (item_exist)
			{
				delete_by_number(task_by::type, type);
			}
			break;
		}
	}
}

int sort_compare(task_by check_by, sort_by order, Item item, Item min_item)
{
	int res = 0;
	switch (check_by)
	{
	case place:
		res = strcmp(item.place, min_item.place);
		break;
	case type:
		res = strcmp(item.type, min_item.type);
		break;
	case weight:
		res = item.weight > min_item.weight;
		res = res == 0 ? -1 : 1;
		break;
	case quality:
		res = strcmp(item.quality, min_item.quality);
		break;
	case result:
		res = strcmp(item.result, min_item.result);
		break;
	}
	return order == sort_by::ascending ? res : res * -1;
}

void sort_items_by(task_by show_by, sort_by order)
{
	Item item;
	FILE* f3;
	int length = file_length();
	err = fopen_s(&f3, (file_name + "_sort").c_str(), "ab+");

	Item min_item;


	for (int j = 0; j < length; j++)
	{
		int i = 0;
		int min_i = 1;
		err = fopen_s(&f, file_name.c_str(), "rb");
		while (fread(&item, sizeof(Item), 1, f) == 1)
		{
			i++;
			if (i == 1)
			{
				min_item = item;
				continue;
			}

			int compare = sort_compare(show_by, order, item, min_item);
			if (compare < 0)
			{
				min_item = item;
				min_i = i;
			}
			else if (compare == 0 && sort_compare(task_by::weight, order, item, min_item) < 0)
			{
				min_item = item;
				min_i = i;
			}
		}

		fwrite(&min_item, sizeof(Item), 1, f3);
		fclose(f);

		delete_by(task_by::number, std::to_string(min_i));
	}

	fclose(f3);
	remove(file_name.c_str());
	rename((file_name + "_sort").c_str(), file_name.c_str());
}

void sort_items()
{
	system("cls");
	std::string menu[3] = { "Сортувати за місцем",  "Сортувати за збільшенням",  "Повернутися в меню" };
	char* arrows = create_arrow_keys(3);
	task_by sort = task_by::place;
	sort_by order = sort_by::ascending;
	sort_items_by(sort, order);
	gotoxy(0, 7);
	show_items();

	while (true)
	{
		gotoxy(0, 0);
		clear_line(0);
		clear_line(1);
		clear_line(2);
		clear_line(3);
		gotoxy(0, 0);
		std::cout << "              Сортування даних             " << std::endl << std::endl;

		int task = arrow_keys(menu, arrows, 3);

		if (task != -1)
		{
			switch (task)
			{
			case 0:
				sort = menu[0] == "Сортувати за місцем" ? task_by::quality : task_by::place;
				menu[0] = menu[0] == "Сортувати за місцем" ? "Сортувати за якістю" : "Сортувати за місцем";
				sort_items_by(sort, order);
				system("cls");
				gotoxy(0, 7);
				show_items();
				break;
			case 1:
				order = menu[1] == "Сортувати за збільшенням" ? sort_by::descending : sort_by::ascending;
				menu[1] = menu[1] == "Сортувати за збільшенням" ? "Сортувати за зменшенням" : "Сортувати за збільшенням";
				sort_items_by(sort, order);
				system("cls");
				gotoxy(0, 7);
				show_items();
				break;
			case 2:
				return;
			default:
				break;
			}
		}
	}

	delete[] arrows;
}

void request_1_table_start()
{
	SetConsoleCP(866);
	SetConsoleOutputCP(866);
	std::cout << std::endl << std::left << "\xC9\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD" <<
		std::setfill('\xCD') << std::setw(26) << "\xCB" << "\xBB" << std::endl;
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	vertical();
	std::cout
		<< std::setfill(' ')
		<< std::left
		<< std::setw(9)
		<< "Якість";
	vertical();
	std::cout << std::left
		<< std::setw(25)
		<< "Відсоткове співвідношення";
	vertical();
	std::cout << std::endl;
}

void request_1_table_end()
{
	SetConsoleCP(866);
	SetConsoleOutputCP(866);
	std::cout << std::left << "\xC8\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD" <<
		std::setfill('\xCD') << std::setw(26) << "\xCA" << "\xBC" << std::endl;
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
}

void request_1_item_output(std::string quality_name, float quality_value)
{
	SetConsoleCP(866);
	SetConsoleOutputCP(866);
	std::cout << std::left << "\xCC\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD" <<
		std::setfill('\xCD') << std::setw(26) << "\xCE" << "\xB9" << std::endl;
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	vertical();
	std::cout
		<< std::setfill(' ')
		<< std::left
		<< std::setw(9)
		<< quality_name;
	vertical();
	std::cout << std::left
		<< std::setw(25)
		<< std::to_string(quality_value).substr(0, 4) + "%";
	vertical();
	std::cout << std::endl;
}

void request_1_output(float quality1, float quality2, float quality3)
{
	request_1_table_start();

	request_1_item_output("Низька", quality1);
	request_1_item_output("Середня", quality2);
	request_1_item_output("Висока", quality3);

	request_1_table_end();
}

void request_1()
{
	system("cls");
	Item item;

	float sum = 0, sum1 = 0, sum2 = 0, sum3 = 0;


	err = fopen_s(&f, file_name.c_str(), "rb");
	while (fread(&item, sizeof(Item), 1, f) == 1)
	{
		if (strcmp(item.quality, "Низька") == 0)
		{
			sum1 += item.weight;
		}
		else if (strcmp(item.quality, "Середня") == 0)
		{
			sum2 += item.weight;
		}
		else if (strcmp(item.quality, "Висока") == 0)
		{
			sum3 += item.weight;
		}
		sum += item.weight;
	}

	fclose(f);

	float quality1 = sum1 == 0 ? 0 : sum1 / sum * 100;
	float quality2 = sum2 == 0 ? 0 : sum2 / sum * 100;
	float quality3 = sum3 == 0 ? 0 : sum3 / sum * 100;

	request_1_output(quality1, quality2, quality3);
	system("pause>0");
}

std::string request_2_get_date()
{
	std::string date = "";
	std::string error_message = "";

	while (true)
	{
		std::cout << "Уведіть дату: " << place << std::endl << std::endl;

		print_error_message(error_message);
		std::cout << std::endl << std::endl;

		//bool exist = request_3_show_place_variants(date);

		gotoxy(21 + date.length(), 0);

		char symbol = _getch();


		if ((int)symbol == 13)
		{
			break;
		}

		system("cls");

		error_message = "";
	}

	return date;
}

void request_2()
{
	system("cls");

	std::string date = request_2_get_date();

	//request_3_show(place);

	system("pause>0");
}

void show_max_weight(int max_weight)
{
	SetConsoleCP(866);
	SetConsoleOutputCP(866);
	std::cout << std::left << "\xCC\xCD\xCD\xCD\xCD\xCD" <<
		std::setfill('\xCD') << std::setw(24) << "\xCA" <<
		std::setfill('\xCD') << std::setw(21) << "\xCA" <<
		std::setfill('\xCD') << std::setw(18) << "\xCA" <<
		std::setfill('\xCD') << std::setw(10) << "\xCA" <<
		std::setfill('\xCD') << std::setw(26) << "\xCA" <<
		std::setfill('\xCD') << std::setw(14) << "\xCA" << "\xB9" << std::endl;
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	vertical();
	std::cout
		<< "Загальна вага: "
		<< std::setfill(' ')
		<< std::left
		<< std::setw(103)
		<< max_weight;
	vertical();
	std::cout << std::endl;
}

void table_end_after_weight()
{
	SetConsoleCP(866);
	SetConsoleOutputCP(866);
	std::cout << std::left << "\xC8\xCD\xCD\xCD\xCD\xCD" <<
		std::setfill('\xCD') << std::setw(24) << "\xCD" <<
		std::setfill('\xCD') << std::setw(21) << "\xCD" <<
		std::setfill('\xCD') << std::setw(18) << "\xCD" <<
		std::setfill('\xCD') << std::setw(10) << "\xCD" <<
		std::setfill('\xCD') << std::setw(26) << "\xCD" <<
		std::setfill('\xCD') << std::setw(14) << "\xCD" << "\xBC" << std::endl;
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
}

bool check_item_place(std::string place, const std::string temp_file_name)
{
	FILE* f2;
	Item item;
	err = fopen_s(&f2, temp_file_name.c_str(), "rb");

	while (fread(&item, sizeof(Item), 1, f2) == 1)
	{
		if (strcmp(item.place, place.c_str()) == 0)
		{
			fclose(f2);
			return true;
		}
	}
	fclose(f2);
	return false;
}

std::string request_3_complete_place(std::string place)
{
	const std::string temp_file_name = file_name + "_temp";

	FILE* f2;
	Item item;
	err = fopen_s(&f2, temp_file_name.c_str(), "rb");

	while (fread(&item, sizeof(Item), 1, f2) == 1)
	{
		if (std::string(item.place).find(place) != std::string::npos)
		{
			fclose(f2);
			return std::string(item.place);
		}
	}
}

bool request_3_show_place_variants(std::string place)
{
	const std::string temp_file_name = file_name + "_temp";

	Item item;
	FILE* f2;

	err = fopen_s(&f, file_name.c_str(), "rb");
	err = fopen_s(&f2, temp_file_name.c_str(), "ab+");
	fclose(f2);

	while (fread(&item, sizeof(Item), 1, f) == 1)
	{
		if (check_item_place(std::string(item.place), temp_file_name))
		{
			continue;
		}

		err = fopen_s(&f2, temp_file_name.c_str(), "ab+");
		fwrite(&item, sizeof(Item), 1, f2);
		fclose(f2);
	}

	fclose(f);
	err = fopen_s(&f2, temp_file_name.c_str(), "rb");
	bool exist = false;

	while (fread(&item, sizeof(Item), 1, f2) == 1)
	{
		if (std::string(item.place).find(place) != std::string::npos)
		{
			exist = true;
			std::cout << item.place << std::endl;
		}
	}

	fclose(f2);

	return exist;
}

std::string request_3_get_place()
{
	std::string place = "";
	std::string error_message;

	while (true)
	{
		std::cout << "Уведіть місце збору: " << place << std::endl << std::endl;

		print_error_message(error_message);
		std::cout << std::endl << std::endl;

		bool exist = request_3_show_place_variants(place);

		gotoxy(21 + place.length(), 0);
		char symbol = _getch();

		system("cls");

		error_message = "";

		if ((int)symbol == 13 && place.length() > 0)
		{
			if (!exist)
			{
				error_message = "Помилка: Такого запису не існує";
				continue;
			}
			if (place[place.length() - 1] == 32 || place[place.length() - 1] == 45)
			{
				place = place.substr(0, place.length() - 1);
			}
			place += "\0";
			break;
		}
		place = check_place(place, symbol, error_message);
	}

	return request_3_complete_place(place);
}

void request_3_show(std::string place)
{
	Item item;
	int weight = 0;
	int i = 0;

	err = fopen_s(&f, file_name.c_str(), "rb");

	table_start();

	while (fread(&item, sizeof(Item), 1, f) == 1)
	{
		if (strcmp(item.place, place.c_str()) == 0)
		{
			weight += item.weight;
			show_item(item, ++i);
		}
	}

	fclose(f);
	show_max_weight(weight);
	table_end_after_weight();
}

void request_3()
{
	system("cls");

	std::string place = request_3_get_place();

	request_3_show(place);

	system("pause>0");
}

void request_4_table_start()
{
	SetConsoleCP(866);
	SetConsoleOutputCP(866);
	std::cout << std::endl << std::left << "\xC9\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD" <<
		std::setfill('\xCD') << std::setw(26) << "\xCB" << "\xBB" << std::endl;
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	vertical();
	std::cout
		<< std::setfill(' ')
		<< std::left
		<< std::setw(20)
		<< "Якість";
	vertical();
	std::cout << std::left
		<< std::setw(25)
		<< "Відсоткове співвідношення";
	vertical();
	std::cout << std::endl;
}

void request_4_table_end()
{
	SetConsoleCP(866);
	SetConsoleOutputCP(866);
	std::cout << std::left << "\xC8\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD" <<
		std::setfill('\xCD') << std::setw(26) << "\xCA" << "\xBC" << std::endl;
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
}

void request_4_item_output(std::string quality_name, float quality_value)
{
	SetConsoleCP(866);
	SetConsoleOutputCP(866);
	std::cout << std::left << "\xCC\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD\xCD" <<
		std::setfill('\xCD') << std::setw(26) << "\xCE" << "\xB9" << std::endl;
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	vertical();
	std::cout
		<< std::setfill(' ')
		<< std::left
		<< std::setw(20)
		<< quality_name;
	vertical();
	std::cout << std::left
		<< std::setw(25)
		<< std::to_string(quality_value).substr(0, 4) + "%";
	vertical();
	std::cout << std::endl;
}

void request_4_output(float quality1, float quality2, float quality3, float quality4, float quality5, float quality6, float quality7)
{
	request_4_table_start();

	request_4_item_output("Метали та бляшанки", quality1);
	request_4_item_output("Картон та папір", quality2);
	request_4_item_output("Пластик", quality3);
	request_4_item_output("Скло", quality4);
	request_4_item_output("Електроніка", quality5);
	request_4_item_output("Олії", quality6);
	request_4_item_output("Аккумулятори", quality7);

	request_4_table_end();
}

void request_4()
{
	system("cls");
	Item item;

	float sum = 0, sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0, sum5 = 0, sum6 = 0, sum7 = 0;

	err = fopen_s(&f, file_name.c_str(), "rb");
	while (fread(&item, sizeof(Item), 1, f) == 1)
	{
		if (strcmp(item.type, "Метали та бляшанки") == 0)
		{
			sum1 += item.weight;
		}
		else if (strcmp(item.type, "Картон та папір") == 0)
		{
			sum2 += item.weight;
		}
		else if (strcmp(item.type, "Пластик") == 0)
		{
			sum3 += item.weight;
		}
		else if (strcmp(item.type, "Скло") == 0)
		{
			sum4 += item.weight;
		}
		else if (strcmp(item.type, "Олії") == 0)
		{
			sum5 += item.weight;
		}
		else if (strcmp(item.type, "Електроніка") == 0)
		{
			sum6 += item.weight;
		}
		else if (strcmp(item.type, "Аккумулятори") == 0)
		{
			sum7 += item.weight;
		}
		sum += item.weight;
	}

	fclose(f);

	float quality1 = sum1 == 0 ? 0 : sum1 / sum * 100;
	float quality2 = sum2 == 0 ? 0 : sum2 / sum * 100;
	float quality3 = sum3 == 0 ? 0 : sum3 / sum * 100;
	float quality4 = sum4 == 0 ? 0 : sum4 / sum * 100;
	float quality5 = sum5 == 0 ? 0 : sum5 / sum * 100;
	float quality6 = sum6 == 0 ? 0 : sum6 / sum * 100;
	float quality7 = sum7 == 0 ? 0 : sum7 / sum * 100;

	request_4_output(quality1, quality2, quality3, quality4, quality5, quality6, quality7);
	system("pause>0");
}

void request_5_show_item_start()
{
	SetConsoleCP(866);
	SetConsoleOutputCP(866);
	std::cout << std::left << "\xCC\xCD\xCD\xCD\xCD\xCD" <<
		std::setfill('\xCD') << std::setw(24) << "\xCE" <<
		std::setfill('\xCD') << std::setw(21) << "\xCE" <<
		std::setfill('\xCD') << std::setw(18) << "\xCE" <<
		std::setfill('\xCD') << std::setw(10) << "\xCE" <<
		std::setfill('\xCD') << std::setw(26) << "\xCE" <<
		std::setfill('\xCD') << std::setw(14) << "\xCE" << "\xB9" << std::endl;
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
}

void request_5_show_item(Item item, int i)
{
	vertical();
	std::cout
		<< std::setfill(' ')
		<< std::left
		<< std::setw(5)
		<< i;
	vertical();
	std::cout << std::left
		<< std::setw(23)
		<< item.place;
	vertical();
	std::cout << std::left
		<< std::setw(20)
		<< item.type;
	vertical();
	std::cout << std::left
		<< std::setw(17)
		<< item.weight;
	vertical();
	std::cout << std::left
		<< std::setw(9)
		<< item.quality;
	vertical();
	std::cout << std::left
		<< std::setw(25)
		<< item.result;
	vertical();
	std::cout << std::left
		<< std::setw(13)
		<< item.date;
	vertical();
	std::cout << std::endl;
}

void request_5_show_items(std::string place)
{
	Item item;
	err = fopen_s(&f, file_name.c_str(), "rb");

	int i = 0;

	while (fread(&item, sizeof(Item), 1, f) == 1)
	{
		if (strcmp(item.place, place.c_str()) == 0)
		{
			i++;
			if (i != 1)
				request_5_show_item_start();

			request_5_show_item(item, i);
		}
	}
	fclose(f);
}

void request_5_table_start()
{
	SetConsoleCP(866);
	SetConsoleOutputCP(866);
	std::cout << std::left << "\xCC\xCD\xCD\xCD\xCD\xCD" <<
		std::setfill('\xCD') << std::setw(24) << "\xCB" <<
		std::setfill('\xCD') << std::setw(21) << "\xCB" <<
		std::setfill('\xCD') << std::setw(18) << "\xCB" <<
		std::setfill('\xCD') << std::setw(10) << "\xCB" <<
		std::setfill('\xCD') << std::setw(26) << "\xCB" <<
		std::setfill('\xCD') << std::setw(14) << "\xCB" << "\xB9" << std::endl;
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
}

void request_5_show(int max_weight)
{
	const std::string temp_file_name = file_name + "_temp";

	Item item;
	FILE* f2;

	err = fopen_s(&f2, temp_file_name.c_str(), "rb");

	table_start();

	bool start = true;
	bool start_block = true;

	while (fread(&item, sizeof(Item), 1, f2) == 1)
	{
		if (item.weight == max_weight)
		{
			if (start)
			{
				request_5_show_item_start();
				start = false;
			}
			else if (start_block == true)
			{
				request_5_table_start();
				start_block = false;
			}

			request_5_show_items(std::string(item.place));
			show_max_weight(max_weight);
		}
	}

	table_end_after_weight();
	fclose(f2);
}

int request_5_find_max_weight(int length)
{
	const std::string temp_file_name = file_name + "_temp";

	Item item;
	Item max_item;
	FILE* f2;

	err = fopen_s(&f2, temp_file_name.c_str(), "ab+");
	fclose(f2);

	for (int i = 0; i < length; i++)
	{
		err = fopen_s(&f, file_name.c_str(), "rb");

		int j = 0;
		bool write = true;

		while (fread(&item, sizeof(Item), 1, f) == 1)
		{
			if (i == j++)
			{
				if (check_item_place(std::string(item.place), temp_file_name))
				{
					write = false;
					break;
				}

				max_item = item;
				write = true;
				continue;
			}
			if (strcmp(item.place, max_item.place) == 0)
			{
				max_item.weight += item.weight;
			}
		}
		fclose(f);
		if (write)
		{
			err = fopen_s(&f2, temp_file_name.c_str(), "ab+");
			fwrite(&max_item, sizeof(Item), 1, f2);
			fclose(f2);
		}
	}

	err = fopen_s(&f2, temp_file_name.c_str(), "rb");
	int max_weight = -1;

	while (fread(&item, sizeof(Item), 1, f2) == 1)
	{
		if (item.weight > max_weight)
		{
			max_weight = item.weight;
		}
	}
	fclose(f2);

	return max_weight;
}

void request_5()
{
	system("cls");

	Item item;
	const std::string temp_file_name = file_name + "_temp";
	int length = file_length();

	int max_weight = request_5_find_max_weight(length);
	request_5_show(max_weight);

	remove(temp_file_name.c_str());

	system("pause>0");
}

void make_request()
{
	std::string menu[6] = { "відсоткове співвідношення сировини за якістю",  "загальна кількість сировини, що надійшла за період часу",  "загальний тоннаж сировини", "відсоткове співвідношення сировини за типом", "місця з яких надійшла максимальна кількість сировини", "Повернутися в меню" };
	char* arrows = create_arrow_keys(6);

	while (true)
	{
		system("cls");
		std::cout << "              МЕНЮ ЗАПИТІВ              " << std::endl << std::endl;

		int task = arrow_keys(menu, arrows, 6);

		if (task != -1)
		{
			switch (task)
			{
			case 0:
				request_1();
				break;
			case 1:
				request_2();
				break;
			case 2:
				request_3();
				break;
			case 3:
				request_4();
				break;
			case 4:
				request_5();
				break;
			case 5:
				return;
			default:
				break;
			}
		}
	}

	delete[] arrows;
}

int array_find(std::string item, std::string* arr, int length)
{
	for (int i = 0; i < length; i++)
	{
		if (strcmp(arr[i].c_str(), item.c_str()))
		{
			return i;
		}
	}

	return -1;
}

std::string get_type_edit(std::string type)
{
	std::string type_list[7] = { "Метали та бляшанки\0",  "Картон та папір\0",  "Пластик\0", "Скло\0", "Олії\0", "Електроніка\0", "Аккумулятори\0" };

	int index = array_find(type, type_list, 7);

	while (true)
	{
		gotoxy(14, 1);
		clear_line(1);
		std::cout << type;

		char symbol = _getch();
		if (symbol == -32)
		{
			switch (_getch())
			{
			case 72:
				index--;
				if (index < 0)
				{
					index = 6;
				}
				type = type_list[index];
				break;

			case 80:
				index++;
				if (index > 6)
				{
					index = 0;
				}
				type = type_list[index];
				break;
			}
		}
		if (symbol == 13)
		{
			return type;
		}
	}
}

std::string get_quality_edit(std::string quality)
{
	std::string quality_list[3] = { "Висока\0",  "Середня\0",  "Низька\0" };

	int index = array_find(quality, quality_list, 3);

	while (true)
	{
		gotoxy(17, 3);
		clear_line(3);
		std::cout << quality;

		char symbol = _getch();
		if (symbol == -32)
		{
			switch (_getch())
			{
			case 72:
				index--;
				if (index < 0)
				{
					index = 2;
				}
				quality = quality_list[index];
				break;

			case 80:
				index++;
				if (index > 2)
				{
					index = 0;
				}
				quality = quality_list[index];
				break;
			}
		}
		if (symbol == 13)
		{
			return quality;
		}
	}
}

Item edit(Item item)
{
	std::string place = std::string(item.place);
	std::string type = std::string(item.type);
	std::string weight = std::to_string(item.weight);
	std::string quality = std::string(item.quality);
	std::string result;
	std::string date = std::string(item.date);
	const std::string example_date = "01.01.2020";

	print_input_table(place, type, weight, quality, date, example_date);

	place = input_place(place);

	type = get_type_edit(type);
	system("cls");
	print_input_table(place, type, weight, quality, date, example_date);

	weight = input_weight(weight);

	quality = get_quality_edit(quality);
	system("cls");
	print_input_table(place, type, weight, quality, date, example_date);

	gotoxy(11, 4);
	result = get_result(type, quality);
	std::cout << result;

	date = input_date(example_date, date);

	strcpy_s(item.place, place.c_str());
	strcpy_s(item.type, type.c_str());
	item.weight = stof(weight);
	strcpy_s(item.quality, quality.c_str());
	strcpy_s(item.result, result.c_str());
	strcpy_s(item.date, date.c_str());

	return item;
}

void find_edit_item(int number)
{
	const std::string temp_file_name = file_name + "_temp";

	FILE* f2;
	Item item;
	int i = 0;

	err = fopen_s(&f, file_name.c_str(), "rb");
	err = fopen_s(&f2, temp_file_name.c_str(), "ab+");

	while (fread(&item, sizeof(Item), 1, f) == 1)
	{
		if (++i == number)
		{
			item = edit(item);
		}

		fwrite(&item, sizeof(Item), 1, f2);
	}

	fclose(f);
	fclose(f2);

	remove(file_name.c_str());
	rename(temp_file_name.c_str(), file_name.c_str());
}

void edit_item()
{
	system("cls");

	std::string number = "";
	std::string error_message = "";

	int length = file_length();

	while (true)
	{
		system("cls");
		std::cout << "Уведіть номер, за яким хочете редагувати: " << number << std::endl << std::endl;

		print_error_message(error_message);

		std::cout << std::endl;
		show_items_by(task_by::number, number);

		gotoxy(42 + number.length(), 0);

		char symbol = _getch();
		if (symbol == 13)
		{
			system("cls");
			if (number.length() > 0)
			{
				system("cls");
				find_edit_item(stoi(number));
			}
			break;
		}
		number = input_number(symbol, number, length, error_message);
	}
}
