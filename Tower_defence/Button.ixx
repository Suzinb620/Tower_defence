export module Button;
import <SFML/System.hpp>;
import <SFML/Graphics.hpp>;
import <iostream>;

export class Button
{
private:
	//Variables:
	sf::RectangleShape rect;
	sf::Text text;
	sf::Color idleColor;
	sf::Color hoverColor;
	sf::Color activeColor;
	sf::Sprite sprite;
	bool activated;
	bool mouseOver;

public:
	//Constructors/destructors:
	Button(float x, float y, float width, float height, std::string text, sf::Color idleColor, sf::Color hoverColor, sf::Color activeColor, sf::Font* font) : idleColor(idleColor), hoverColor(hoverColor), activeColor(activeColor), activated(true), mouseOver(false)
	{
		initRect(idleColor, x, y, width, height);
		initText(text, sf::Color::Black, 30, font, x, y + (height / 2) - 15, width);

	};

	Button(float x, float y, float width, float height, std::string text, sf::Color idleColor, sf::Color hoverColor, sf::Color activeColor, sf::Font* font, sf::Texture* texture) : idleColor(idleColor), hoverColor(hoverColor), activeColor(activeColor), activated(true), mouseOver(false)
	{
		initRect(idleColor, x, y, width, height);
		initText(text, sf::Color::Black, 17, font, x, y + (height - 20), width);
		initSprite(texture, x, y, width, height);
	};

private:
	//Initialization:
	void initRect(const sf::Color& idleColor, const float& x, const float& y, const float& width, const float& height)
	{
		this->rect.setFillColor(idleColor);
		this->rect.setPosition(sf::Vector2f(x, y));
		this->rect.setSize(sf::Vector2f(width, height));
	}

	void initText(const std::string& text, const sf::Color& color, const int& size, sf::Font* font, const float& x, const float& y , const float& width)
	{
		this->text.setString(text);
		this->text.setFillColor(color);
		this->text.setCharacterSize(size);
		this->text.setFont(*font);
		this->text.setPosition(x + (width / 2) - (this->text.getGlobalBounds().width / 2), y);
	}

	void initSprite(sf::Texture* texture, const float& x, const float& y, const float& width, const float& height)
	{
		this->sprite.setTexture(*texture, true);
		this->sprite.scale((width / 100) - 0.05, (height / 100) - 0.05);
		this->sprite.setPosition(x + (width / 2) - (this->sprite.getGlobalBounds().width / 2), y + 5);
	}

public:
	//Update:
	void update(const sf::Vector2f& mousePosition, sf::Event& ev) 
	{
		if (this->activated)
			updateState(mousePosition, ev);
		else
			setColor(sf::Color(90, 90, 90));
	}

	void updateState(const sf::Vector2f& mousePosition, sf::Event& ev)
	{
		this->mouseOver = false;
		setColor(idleColor);
		if (this->rect.getGlobalBounds().contains(mousePosition))
		{
			this->mouseOver = true;
			setColor(hoverColor);
			if (ev.type == sf::Event::MouseButtonPressed && ev.mouseButton.button == sf::Mouse::Left)
				setColor(activeColor);
		}
	}

	//Render:
	void render(sf::RenderTarget* renderTarget) 
	{
		renderTarget->draw(this->rect);
		renderTarget->draw(this->text);
		renderTarget->draw(this->sprite);
	}

	//Methods:
	void setColor(const sf::Color& color) { this->rect.setFillColor(color); }
	void activate() { this->activated = true; }
	void deactivate() { this->activated = false; }

	//Accessors:
	bool isMouseOver() const { return this->mouseOver; }
	bool isActivated() const { return this->activated; }
	bool isPressable() const { return isMouseOver() && isActivated(); }
};