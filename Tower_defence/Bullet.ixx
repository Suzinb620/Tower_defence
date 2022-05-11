export module Bullet;
import <SFML/Graphics.hpp>;
import <SFML/System.hpp>;
import <iostream>;
import <cmath>;
import Balloon;

export class Bullet
{
private:
	sf::RectangleShape shape;
	int damage;
	Balloon* target;
	sf::Vector2f direction;
	bool hit;

public:
	//Constructors/destructors:
	Bullet(int damage, sf::Vector2f position, Balloon* target) : damage(damage), target(target), hit(false), direction(position), shape(sf::Vector2f(8, 8))
	{ 
		shape.setPosition(position);
		shape.setFillColor(sf::Color::Black);
	}

	//Update:
	void update(const float& deltaTime)
	{
		if (this->target != nullptr)
		{
			updateStatus();
			updateDirection();
			this->shape.move(this->direction * deltaTime * 1500.0f);
		}
		else setHit();
	}

private:
	void updateDirection() { this->direction = normalize(this->target->getPosition() - this->shape.getPosition()); }

	void updateStatus()
	{
		if (this->target->getGlobalBounds().intersects(this->shape.getGlobalBounds()))
		{
			target->dealDamage(this->damage);
			setHit();
		}
	}


public:
	//Render:
	void render(sf::RenderTarget* renderTarget) const { renderTarget->draw(shape); }

	//Methods:
	void setHit(bool value = true) { this->hit = value; }
	float vectorLength(const sf::Vector2f& vector) { return std::sqrt(std::pow(vector.x, 2) + std::pow(vector.y, 2)); }
	sf::Vector2f normalize(const sf::Vector2f& vector) { return sf::Vector2f(vector / vectorLength(vector)); }

	//Accessors:
	int getDamage() const { return this->damage; }
	bool isHit() const { return this->hit; }
};