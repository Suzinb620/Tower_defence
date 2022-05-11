export module MonkeyStandard;
import <SFML/System.hpp>;
import <SFML/Graphics.hpp>;
import <iostream>;
import ResourceManager;
import Monkey;

export class MonkeyStandard : public Monkey
{
private:
	std::size_t attackCounter;

public:
	//Constructors/destructors:
	MonkeyStandard(const sf::Vector2f& position = sf::Vector2f(0, 0), const std::size_t& sValue = 0) : Monkey(150, 40, 10, 2.2), attackCounter(sValue)
	{ 
		this->initMonkey(); 
		this->setPosition(position);
	}

	//Methods:	
	void specialAbility() override 
	{ 
		if (this->attackCounter == 3)
		{
			this->attackCounter = 0;
			this->setDamage(60);
		}
		else if (this->damage == 50)
			this->setDamage(40);
		else 
			this->attackCounter++;
	}

	//Accessors:
	std::string getType() const override { return "MonkeyStandard"; }
	std::size_t getSpecialAbilityValue() const override { return this->attackCounter; }
};