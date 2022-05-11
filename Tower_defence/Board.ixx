export module Board;
import <boost/nondet_random.hpp>;
import <SFML/Graphics.hpp>;
import <boost/random.hpp>;
import <SFML/System.hpp>;
import <filesystem>;
import <functional>;
import <execution>;
import <semaphore>;
import <iostream>;
import <vector>;
import <thread>;
import <mutex>;
import ResourceManager;
import TileMap;
import Balloon;
import Monkey;

export class Board
{
private:
	//Levles:  (-1 = start, -2 = end)
	const std::vector<int> level =
	{
		0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0,
		2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 3, 0, 0, 3, 0, 2,
		2, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 0, 3, 0, 0, 3, 0, 0,
		0, 0, 0, 0, 3, 0, 0, 3, 0, 0, 0, 0, 3, 0, 0, 3, 0, 0,
		2, 0, 0, 0, 3, 0, 0, 3, 0, 0, 2, 0, 3, 0, 0, 3, 0, 0,
		2, 0, 0, 0, 3, 2, 0, 3, 0, 0, 0, 0, 3, 0, 0, 3, 0, 0,
		0, 0, 0, 0, 3, 0, 0, 3, 0, 0, 0, 0, 3, 0, 0, 3, 0, 0,
		2, 0, 0, 0, 3, 0, 0, 3, 0, 0, 0, 0, 3, 0, 0, 3, 3, -2,
		0, 0, 0, 2, 3, 0, 0, 3, 3, 3, 3, 0, 3, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 3, 0, 0, 2, 0, 0,
		0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 3, 0, 0, 0, 0, 0,
		-1, 3, 3, 3, 3, 0, 0, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 2, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2,
	};

	//Variables:
	TileMap* map;
	std::vector<Balloon*> balloons;
	std::vector<Monkey*> monkeys;
	std::vector<Bullet*> bullets;
	std::size_t balloonsToRespawnLeft;
	std::size_t balloonsLeft;
	sf::Clock balloonsSpawnTimer;
	std::counting_semaphore<300> semaphore;
	std::mutex mutexUpdate;
	std::mutex mutexRespawn;

public:
	//Constructors/destructors:
	Board(sf::Texture* tileSet = ResourceManager::getTexture("TileSet.png")) : balloonsToRespawnLeft(0), semaphore(0), monkeys(0), balloons(0), bullets(0), balloonsLeft(0) { initMap(tileSet); }
	Board(const std::vector<Monkey*>& monkeys, sf::Texture* tileSet = ResourceManager::getTexture("TileSet.png")) : monkeys(monkeys), balloonsToRespawnLeft(0), semaphore(0), balloons(0), bullets(0), balloonsLeft(0) { initMap(tileSet); }

	~Board()
	{
		for (auto& bullet : this->bullets)
			delete bullet;
		for (auto& balloon : this->balloons)
			delete balloon;
		for (auto& monkey : this->monkeys)
			delete monkey;
	}

	//Initialization:
	void initMap(sf::Texture* tileSet) { map = new TileMap(*tileSet, sf::Vector2i(64, 64), level, 18, 15, 3); }

	//Update:
	void update(const float& deltaTime, const sf::Vector2f& mousePosition, sf::RenderWindow* window, sf::Event& ev)
	{
		std::jthread j1(&Board::updateBalloonsReleaser, this);
		std::jthread j2(&Board::updateBalloonsSpawner, this);
		this->updateBalloons(deltaTime);
		this->updateBullets(deltaTime);
		this->updateMonkeys(mousePosition, deltaTime);
	}

private:
	void updateMonkeys(const sf::Vector2f& mousePosition, const float& deltaTime)
	{
		std::for_each(std::execution::par_unseq, this->monkeys.begin(), this->monkeys.end(), [&](const auto& monkey)
			{ 
				const std::lock_guard<std::mutex> lock(this->mutexUpdate);
				monkey->update(balloons, bullets, mousePosition, deltaTime); 
			});
	}
	 
	void updateBalloons(const float& deltaTime)
	{
		balloons.erase(std::remove_if(this->balloons.begin(), this->balloons.end(), [&](auto& balloon) {
			if (balloon->isDead())
			{
				balloon = nullptr;
				this->balloonsLeft--;
				return true;
			}
			return false;
			}), balloons.end());

		std::for_each(std::execution::par_unseq, this->balloons.begin(), this->balloons.end(), [&](const auto& balloon) 
			{ 
				const std::lock_guard<std::mutex> lock(this->mutexUpdate);
				balloon->update(deltaTime); 
			});
	} 

	void updateBullets(const float& deltaTime)
	{
		bullets.erase(std::remove_if(this->bullets.begin(), this->bullets.end(), [&](auto& bullet) {
			if (bullet->isHit())
			{
				bullet = nullptr;
				return true;
			}
			return false;
		}), bullets.end());

		std::for_each(std::execution::par_unseq, this->bullets.begin(), this->bullets.end(), [&](const auto& bullet) 
			{ 
				const std::lock_guard<std::mutex> lock(this->mutexUpdate);
				bullet->update(deltaTime); 
			});
	}
	void updateBalloonsSpawner()
	{
		const std::lock_guard<std::mutex> lock(this->mutexRespawn);
		if (this->balloonsSpawnTimer.getElapsedTime() >= sf::milliseconds(200) && this->semaphore.try_acquire())
		{
			this->addBalloon(new Balloon(this->map->getPath()));
			this->balloonsSpawnTimer.restart();
		}
	}

	void updateBalloonsReleaser()
	{
		const std::lock_guard<std::mutex> lock(this->mutexRespawn);
		if (this->balloonsSpawnTimer.getElapsedTime() >= sf::milliseconds(600) && !this->semaphore.try_acquire() && this->balloonsToRespawnLeft != 0)
		{
			int randNum = randomNumberGenerator(0, this->balloonsToRespawnLeft);
			this->semaphore.release(randNum);
			this->balloonsToRespawnLeft -= randNum;
			this->balloonsSpawnTimer.restart();
		}
	}

public:
	//Render:
	void render(sf::RenderTarget* renderTarget)
	{
		renderTarget->draw(*this->map);

		for (const auto& monkey : this->monkeys)
			monkey->render(renderTarget);
		for (const auto& bullet : this->bullets)
			bullet->render(renderTarget);
		for (const auto& balloon : this->balloons)
			balloon->render(renderTarget);
	}

	//Methods:
	bool check(const sf::Vector2f& mousePosition)
	{
		if (this->map->checkPosition(mousePosition, 0) && !this->MonkeyCointainsCursor(mousePosition))
			return true;
		return false;
	}

	bool MonkeyCointainsCursor(const sf::Vector2f& mousePosition)
	{
		for (const auto& monkey : this->monkeys)
		{
			if (monkey->getGlobalBounds().contains(mousePosition))
				return true;
		}
		return false;
	}

	int randomNumberGenerator(int min, int max)
	{
		boost::random::random_device rand;
		boost::random::mt19937 gen(rand());
		boost::random::uniform_int_distribution<> dist(min, max);
		return dist(gen);
	}

	void setBalloonsToRespawn(const std::size_t& value) 
	{ 
		this->balloonsToRespawnLeft = value; 
		this->balloonsLeft = value;
	}

	void addMonkey(Monkey* monkey) { this->monkeys.push_back(monkey); }
	void addBalloon(Balloon* balloon) { this->balloons.push_back(balloon); }

	//Accessors:
	const std::vector<Monkey*>* getMonkeysOnBoard() const { return &this->monkeys; }
	const std::vector<Balloon*>* getBalloonsOnBoard() const { return &this->balloons; }
	bool isBalloonsExists() const { return !this->balloons.empty() || this->balloonsToRespawnLeft != 0; }
	std::size_t getBalloonsLeft() const { return this->balloonsLeft; }
};