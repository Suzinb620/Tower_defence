export module Monkey;
import <SFML/Graphics.hpp>;
import <SFML/System.hpp>;
import <filesystem>;
import <execution>;
import <iostream>;
import ResourceManager;
import Balloon;
import Bullet;

export class Monkey
{
protected:
	//Variables:
	int price;
	int damage;
	float attackSpeed;
	sf::CircleShape attackRange;
	bool attackRangeVisible;
	sf::Image* image;
	sf::Sprite sprite;
	sf::Clock shootingTimer;

public:
	//Constructors/destructors:
	Monkey(const int& price, const int& damage, const int& attackSpeed, const float& attackRange) : price(price), damage(damage), attackSpeed(attackSpeed), attackRange(64 * attackRange, 32), attackRangeVisible(false) { this->initAttackRange(); }

protected:
	//Initialization:
	void initMonkey()
	{
		if (ResourceManager::getTexture(this->getType() + ".png") != nullptr)
		{
			this->sprite.setTexture(*ResourceManager::getTexture(this->getType() + ".png"), true);
			image = ResourceManager::getImage(this->getType() + ".png");
		}
		else throw std::exception("Brak tekstur\n");
	}

	void initAttackRange()
	{
		this->attackRange.setFillColor(sf::Color::Transparent);
		this->attackRange.setOutlineColor(sf::Color::White);
		this->attackRange.setOutlineThickness(2.0f);
		this->setAttackRange();
	}

public:
	//Update:
	void update(const std::vector<Balloon*>& balloons, std::vector<Bullet*>& bullets, const sf::Vector2f& mousePosition, const float& deltaTime)
	{
		updateShooting(balloons, bullets, deltaTime);
		updateAttackRangeVisibility(mousePosition);
	}

	void updateShooting(const std::vector<Balloon*>& balloons, std::vector<Bullet*>& bullets, const float& deltaTime)
	{
		if (shootingTimer.getElapsedTime() >= (sf::seconds(5) / this->attackSpeed))
		{
			shotToBalloonInRange(balloons, bullets);
			shootingTimer.restart();
		}
	}

	void updateAttackRangeVisibility(const sf::Vector2f& mousePosition)
	{
		if (this->sprite.getGlobalBounds().contains(mousePosition))
			this->attackRangeVisible = true;
		else 
			this->attackRangeVisible = false;
	}

	//Render:
	void render(sf::RenderTarget* renderTarget) const 
	{
		renderTarget->draw(sprite);
		if(attackRangeVisible)
			renderTarget->draw(attackRange);
	}

	//Methods:
	void shotToBalloonInRange(const std::vector<Balloon*>& balloons, std::vector<Bullet*>& bullets)
	{
		for (const auto& balloon : balloons)
		{
			if (attackRange.getGlobalBounds().contains(balloon->getPosition()))
			{
				specialAbility();
				bullets.push_back(new Bullet(this->damage, this->sprite.getPosition() + sf::Vector2f(32, 32), balloon));
				return;
			}
		}
	}

	virtual void specialAbility() = 0;
	void setDamage(int value) { this->damage = value; }
	void setAttackSpeed(float value) { this->attackSpeed = value; }
	void setAttackRange(float value) { this->attackRange.setRadius(64 * value); }

	void setPosition(sf::Vector2f position)
	{
		this->sprite.setPosition(position - sf::Vector2f(32, 32));
		this->setAttackRange();
	}

	void setAttackRange()
	{
		float radius = this->attackRange.getRadius();
		this->attackRange.setPosition(this->getPosition() - sf::Vector2f(radius, radius));
	}

	//Accessors:
	virtual std::string getType() const = 0;
	virtual std::size_t getSpecialAbilityValue() const = 0;
	sf::FloatRect getGlobalBounds() const { return sprite.getGlobalBounds(); }
	sf::Image* getImage() const { return image; }
	sf::Vector2f getPosition() const { return sprite.getPosition() + sf::Vector2f(32, 32); }
	int getPrice() const { return price; }
	float getAttackRange() const { return attackRange.getRadius(); }
};