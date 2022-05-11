export module MonkeyNinja;
import <SFML/System.hpp>;
import <SFML/Graphics.hpp>;
import <iostream>;
import ResourceManager;
import Monkey;

export class MonkeyNinja : public Monkey
{
private:
	sf::Clock boostTimer;

public:
	//Constructors/destructors:
	MonkeyNinja(const sf::Vector2f& position = sf::Vector2f(0, 0), const std::size_t& sValue = 0) : Monkey(300, 10, 20, 2.2)
	{ 
		this->initMonkey(); 
		this->setPosition(position);
	}

	//Methods:
	void specialAbility() override
	{
		if (this->boostTimer.getElapsedTime() >= sf::seconds(10) && this->attackSpeed != 25)
		{
			this->setAttackSpeed(25);
			return;
		}

		if (this->boostTimer.getElapsedTime() >= sf::seconds(12) && this->attackSpeed == 25)
		{
			this->setAttackSpeed(20);
			this->boostTimer.restart();
			return;
		}
	}

	//Accessors:
	std::string getType() const override { return "MonkeyNinja"; }
	std::size_t getSpecialAbilityValue() const override { return 0.; }
};