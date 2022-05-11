export module SaveManager;
import <SFML/System.hpp>;
import <filesystem>;
import <iostream>;
import <fstream>;
import <vector>;
import Monkey;
import MonkeyStandard;
import MonkeyNinja;
import MonkeySniper;

namespace fs = std::filesystem;

export struct Save
{
	//Variables:
	int round;
	int saldo;
	int health;
	std::vector<Monkey*> monkeys;

	//Constructors/destructors:
	Save() = default;

	Save(int round, int saldo, int health, std::vector<Monkey*> monkeys) : round(round), saldo(saldo), health(health), monkeys(monkeys) {}
};

export class SaveManager
{
private:
	//Variables:
	inline static fs::path path = fs::current_path() / "save.txt";

	//Constructors:
	SaveManager() = delete;
	~SaveManager() = delete;

public:
	//Methods:
	inline static bool isSaveExist() { return fs::exists(path) ? true : false; }

	inline static void save(Save save)
	{
		std::ofstream file(path);
		if (file)
		{
			file << save.round << "\n" << save.saldo << "\n" << save.health << "\n";

			for (const auto& monkey : save.monkeys)
				file << monkey->getType() << "\n" << monkey->getPosition().x << "\n" << monkey->getPosition().y << "\n" << monkey->getSpecialAbilityValue() << "\n";
		}
		else throw std::exception("Blad podczas zapisu gry\n");

		file.close();
	}

	inline static Save load()
	{
		Save save;
		std::ifstream file(path);
		if (file)
		{
			file >> save.round >> save.saldo >> save.health;
			
			std::string type, x, y, sValue;
			while (file >> type >> x >> y >> sValue)
				save.monkeys.push_back(matchMonkey(type, sf::Vector2f(std::stof(x), std::stof(y)), std::stoi(sValue)));
		}
		else throw std::exception("Nie mozna otworzyc pliku z zapisem gry\n");

		file.close();
		return save;
	}
	 
private:
	inline static Monkey* matchMonkey(const std::string& type, const sf::Vector2f& position, const float& sValue)
	{
		if (type == "MonkeyStandard")
			return new MonkeyStandard(position, sValue);
		else if (type == "MonkeySniper")
			return new MonkeySniper(position, sValue);
		else if (type == "MonkeyNinja")
			return new MonkeyNinja(position, sValue);
		else throw std::exception("Problem podczas wczytywania typu malpy\n");
	}
};