export module GameState;
import <SFML/Graphics.hpp>;
import <SFML/System.hpp>;
import <filesystem>;
import <iostream>;
import <map>;
import ResourceManager;
import MonkeyStandard;
import MonkeySniper;
import MonkeyNinja;
import SaveManager;
import Button;
import Board;
import State;

enum class CursorMode
{
	arrow, 
	allowed,
	notAllowed
};

export class GameState : public State
{
private:
	//Variables:
	int round;
	int saldo;
	int health;
	Board* board;
	std::map<std::string, sf::Text*> texts;
	std::map<std::string, sf::Sprite*> sprites;
	std::map<std::string, Button*> buttons;
	Monkey* monkeyToAdd;
	CursorMode cursor;
	std::size_t balloonsLeft;
	bool duringRound;

public:
	//Constructors/destructors:
	GameState(std::stack<State*>* states) : State(states), round(0), saldo(320), health(200), cursor(CursorMode::arrow), duringRound(false), monkeyToAdd(nullptr), balloonsLeft(0)
	{
		this->board = new Board();
		initTexts();
		initSprites();
		initButtons();
	}

	GameState(std::stack<State*>* states, const Save& save) : State(states), round(save.round), saldo(save.saldo), health(save.health), cursor(CursorMode::arrow), duringRound(false), monkeyToAdd(nullptr), balloonsLeft(0)
	{
		this->board = new Board(save.monkeys);
		initTexts();
		initSprites();
		initButtons();
	}

	~GameState()
	{
		for (auto& button : this->buttons)
			delete button.second;
		for (auto& text : this->texts)
			delete text.second;
		for (auto& sprite : this->sprites)
			delete sprite.second;
	}

private:
	//Initialization:
	void initTexts()
	{
		this->texts.insert({ "saldo", new sf::Text(std::to_string(saldo), *ResourceManager::getFont(), 20) });
		this->texts.insert({ "health", new sf::Text(std::to_string(health), *ResourceManager::getFont(), 20) });
		this->texts.insert({ "round", new sf::Text(std::to_string(round) + " / 20", *ResourceManager::getFont(), 20) });
		this->texts.insert({ "info", new sf::Text("", *ResourceManager::getFont(), 40) });
		this->texts["saldo"]->setFillColor(sf::Color::White);
		this->texts["saldo"]->setPosition(1222, 12);
		this->texts["health"]->setFillColor(sf::Color::White);
		this->texts["health"]->setPosition(1222, 71);
		this->texts["round"]->setFillColor(sf::Color::White);
		this->texts["round"]->setPosition(1187, 840);
		this->texts["info"]->setFillColor(sf::Color::White);
		this->texts["info"]->setPosition(571, 20);
	}

	void initSprites()
	{
		this->sprites.insert({ "coin", new sf::Sprite(*ResourceManager::getTexture("Coin.png")) });
		this->sprites.insert({ "heart", new sf::Sprite(*ResourceManager::getTexture("Heart.png")) });
		this->sprites["coin"]->setPosition(1177, 7);
		this->sprites["coin"]->scale(0.55, 0.55);
		this->sprites["heart"]->setPosition(1177, 66);
		this->sprites["heart"]->scale(0.55, 0.55);
	}

	void initButtons()
	{
		this->buttons.insert({ "monkeyStandard", new Button(1182, 150, 70, 70, "150", sf::Color(180, 180, 180), sf::Color(140, 140, 140), sf::Color(90, 90, 90), ResourceManager::getFont(), ResourceManager::getTexture("MonkeyStandard.png")) });
		this->buttons.insert({ "monkeyNinja", new Button(1182, 240, 70, 70, "300", sf::Color(180, 180, 180), sf::Color(140, 140, 140), sf::Color(90, 90, 90), ResourceManager::getFont(), ResourceManager::getTexture("MonkeyNinja.png")) });
		this->buttons.insert({ "monkeySniper", new Button(1182, 330, 70, 70, "320", sf::Color(180, 180, 180), sf::Color(140, 140, 140), sf::Color(90, 90, 90), ResourceManager::getFont(), ResourceManager::getTexture("MonkeySniper.png")) });
		this->buttons.insert({ "startRound", new Button(1182, 870, 70, 70, "start", sf::Color(180, 180, 180), sf::Color(140, 140, 140), sf::Color(90, 90, 90), ResourceManager::getFont(), ResourceManager::getTexture("Arrow.png")) });
	}

public:
	//Update:
	void update(const float& deltaTime, const sf::Vector2f& mousePosition, sf::RenderWindow* window, sf::Event& ev) override
	{
		updateGameStatus();
		updateButtonsStatus();
		updateButtons(mousePosition, ev);
		updateEvent(window, ev);		
		updateAdding(mousePosition, window);
		updateTexts();
		updateDamageTaken();
		updateSaldo();
		this->board->update(deltaTime, mousePosition, window, ev);
	}

private:
	void updateButtons(const sf::Vector2f& mousePosition, sf::Event& ev)
	{
		for (const auto& button : this->buttons)
			button.second->update(mousePosition, ev);
	}

	void updateTexts()
	{
		this->texts["saldo"]->setString(std::to_string(saldo));
		this->texts["health"]->setString(std::to_string(health));
		this->texts["round"]->setString(std::to_string(round) + " / 20");
	}

	void updateGameStatus()
	{
		if (this->health <= 0)
			this->end("Przegrales");

		if (!this->board->isBalloonsExists())
		{
			this->duringRound = false;
			if (this->round == 20)
				this->end("Wygrales");
		}
	}

	void updateAdding(const sf::Vector2f& mousePosition, sf::RenderWindow* window)
	{
		if (this->monkeyToAdd == nullptr)
			changeCursor(window, CursorMode::arrow);
		else if (this->board->check(mousePosition) && saldo - monkeyToAdd->getPrice() >= 0)
		{
			changeCursor(window, CursorMode::allowed);
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
				this->addToBoard(mousePosition);
		}
		else
			changeCursor(window, CursorMode::notAllowed);
	}

	void updateButtonsStatus()
	{
		if (this->duringRound)
			this->buttons["startRound"]->deactivate();
		else
			this->buttons["startRound"]->activate();
	}

	void updateDamageTaken()
	{
		for (const auto& balloon : *this->board->getBalloonsOnBoard())
			if (balloon->isFinished())
			{
				this->health -= balloon->getDamage();
				balloon->kill();
			}
	}

	void updateSaldo()
	{
		if (this->board->getBalloonsLeft() < this->balloonsLeft)
		{
			this->saldo += (this->balloonsLeft - this->board->getBalloonsLeft()) * 3;
			this->balloonsLeft = this->board->getBalloonsLeft();
		}
	}

	void updateEvent(sf::RenderWindow* window, sf::Event& ev)
	{
		while (window->pollEvent(ev))
		{
			switch (ev.type)
			{
			case sf::Event::Closed:
				window->close();
				break;
			case sf::Event::KeyReleased:
				if (ev.key.code == sf::Keyboard::Escape)
				{
					if (this->monkeyToAdd != nullptr)
						this->monkeyToAdd = nullptr;
					else if (!duringRound)
					{
						this->save();
						this->quit();
					}
				}
				if (ev.key.code == sf::Keyboard::S)
					if(!duringRound)
						this->save();
				break;
			case sf::Event::MouseButtonReleased:
				if (ev.mouseButton.button == sf::Mouse::Left)
					eventButton(window);
				break;
			}
		}
	}

public:
	//Events:
	void eventButton(sf::RenderWindow*)
	{
		if (this->buttons["monkeyStandard"]->isPressable())
			monkeyToAdd = new MonkeyStandard();
		if (this->buttons["monkeyNinja"]->isPressable())
			monkeyToAdd = new MonkeyNinja();
		if (this->buttons["monkeySniper"]->isPressable())
			monkeyToAdd = new MonkeySniper();
		if (this->buttons["startRound"]->isPressable())
			startRound();
	}

	//Render:
	void render(sf::RenderTarget* renderTarget) override
	{
		this->board->render(renderTarget);
		for (const auto& text : this->texts)
			renderTarget->draw(*text.second);		
		for (const auto& sprite : this->sprites)
			renderTarget->draw(*sprite.second);
		for (const auto& button : this->buttons)
			button.second->render(renderTarget);
	}

	//Methods:
	void startRound()
	{
		this->round++;
		this->balloonsLeft = this->round * 15;
		this->board->setBalloonsToRespawn(this->balloonsLeft);
		this->duringRound = true;
	}

	void addToBoard(const sf::Vector2f& mousePosition)
	{
		this->monkeyToAdd->setPosition(mousePosition);
		this->saldo -= this->monkeyToAdd->getPrice();
		this->board->addMonkey(monkeyToAdd);
		this->monkeyToAdd = nullptr;
	}

	void changeCursor(sf::RenderWindow* window, const CursorMode& newCursor)
	{
		if (this->cursor != newCursor)
		{
			this->cursor = newCursor;
			sf::Cursor c;
			switch (newCursor)
			{
			case CursorMode::arrow:
				c.loadFromSystem(sf::Cursor::Arrow);
				break;
			case CursorMode::allowed:
				c.loadFromPixels(this->monkeyToAdd->getImage()->getPixelsPtr(), sf::Vector2u(64, 64), sf::Vector2u(32, 32));
				break;
			case CursorMode::notAllowed:
				c.loadFromSystem(sf::Cursor::NotAllowed);
				break;
			}
			window->setMouseCursor(c);
		}
	}

	void save()
	{
		if (!this->duringRound && (this->round != 0 || !this->board->getMonkeysOnBoard()->empty()))
			SaveManager::save(Save(this->round, this->saldo, this->health, *this->board->getMonkeysOnBoard()));
	}

	void end(const std::string& text)	
	{ 
		this->texts["info"]->setString(text);
		std::this_thread::sleep_for(std::chrono::seconds(3));
		this->quit();
	}
};