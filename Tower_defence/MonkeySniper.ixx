export module MonkeySniper;
import <SFML/System.hpp>;
import <SFML/Graphics.hpp>;
import <iostream>;
import ResourceManager;
import Monkey;

export class MonkeySniper : public Monkey
{
private:
	std::size_t attackCounter;

public:
	//Constructors/destructors:
	MonkeySniper(const sf::Vector2f& position = sf::Vector2f(0, 0), const std::size_t& sValue = 0) : Monkey(320, 80, 4, 4.2), attackCounter(sValue)
	{ 
		this->initMonkey();
		this->setPosition(position);
	}

	//Methods:
	void specialAbility() override
	{
		if (this->attackCounter == 80)
		{
			this->attackCounter = 0;
			this->setAttackRange(this->getAttackRange() + 1);
		}
		else 
			this->attackCounter++;
	}

	//Accessors:
	std::string getType() const override { return "MonkeySniper"; }
	std::size_t getSpecialAbilityValue() const override { return this->attackCounter; }
};