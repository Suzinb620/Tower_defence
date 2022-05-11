export module Balloon;
import <SFML/Graphics.hpp>;
import <SFML/System.hpp>;
import <iostream>;
import <atomic>;
import ResourceManager;
import TileMap;

export class Balloon
{
private:
	//Variables:
	int health;
	float movementSpeed;
	int damage;
	sf::Sprite sprite;
	bool dead;
	bool finished;
	const std::vector<sf::Vector2f>* path;
	int targetNumber;
	sf::Vector2f target;
	sf::Vector2f direction;

public:
	//Constructors/destructors:
	Balloon(const std::vector<sf::Vector2f>* path) : health(100), movementSpeed(3), damage(2), dead(false), finished(false), path(path), targetNumber(0), direction(sf::Vector2f(0, 0)), target((*path)[0])
	{
		this->sprite.setPosition((*path)[0]);
		this->sprite.setTexture(*ResourceManager::getTexture("Balloon.png"), true);
	}

	//Update:
	void update(const float& deltaTime)
	{
		this->updateStatus();
		this->updateMovement(deltaTime);
	}

	void updateMovement(const float& deltaTime)
	{
		this->sprite.move(this->direction * this->movementSpeed * deltaTime);

		sf::Vector2f targetDirection = this->target - this->sprite.getPosition();
		if (this->dotProduct(this->direction, targetDirection) <= 0)
		{
			if (this->targetNumber + 1 != this->path->size())
				this->setNextTarget();
			else this->finish();
		}
	}

	void updateStatus()
	{
		if (this->health <= 0)
			kill();
	}

	//Render:
	void render(sf::RenderTarget* renderTarget) const { renderTarget->draw(this->sprite); }

	//Methods:
	void setNextTarget()
	{
		this->sprite.setPosition(this->target);
		this->targetNumber++;
		this->target = (*this->path)[this->targetNumber];
		this->direction = this->target - this->sprite.getPosition();
	}

	void kill() { this->dead = true; }
	void finish() { this->finished = true; }
	void dealDamage(int damage) { this->health -= damage; }
	float dotProduct(const sf::Vector2f& rhs, const sf::Vector2f& lhs) { return rhs.x * lhs.x + rhs.y * lhs.y; }

	//Accessors:
	bool isDead() const { return this->dead; }
	bool isFinished() const { return this->finished; }
	int getDamage() const { return this->damage; }
	sf::Vector2f getPosition() const { return this->sprite.getPosition(); }
	sf::FloatRect getGlobalBounds() const { return this->sprite.getGlobalBounds(); }
};