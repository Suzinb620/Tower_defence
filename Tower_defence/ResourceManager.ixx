export module ResourceManager;
import <SFML/System.hpp>;
import <SFML/Graphics.hpp>;
import <filesystem>;
import <iostream>;
import <map>;

export class ResourceManager
{
private:
	//Variables:
	inline static std::map<std::string, sf::Texture*> textures;
	inline static std::map<std::string, sf::Image*> pictures;
	inline static sf::Font font;

	//Constructors/destructors:
	ResourceManager() = delete;
	~ResourceManager() = delete;
	
public:
	//Initialization:
	inline static void load()
	{
		initFont(std::filesystem::current_path() / "Font");
		initResources(std::filesystem::current_path() / "Textures");
	}

	inline static void unload()
	{
		for (auto& texture : textures)
			delete texture.second;
		for (auto& image : pictures)
			delete image.second;
	}

private:
	inline static void initFont(const std::filesystem::path& path)
	{
		if (!font.loadFromFile(path.string() + "/Font.ttf"))
			throw std::exception("Nie mozna otworzyc pliku z czcionka\n");
	}

	inline static void initResources(const std::filesystem::path& path)
	{
		for (const auto& it : std::filesystem::directory_iterator(path))
		{
			initTexture(it.path().filename().string(), it.path());

			if (std::string_view(it.path().filename().string()).starts_with("Monkey"))
				initPicture(it.path().filename().string(), it.path());
		}
	}

	inline static void initTexture(const std::string& name, const std::filesystem::path& path)
	{
		textures.insert({ name, new sf::Texture() });

		if (!textures[name]->loadFromFile(path.string()))
			throw std::exception("Nie mozna otworzyc pliku tekstury\n");
	}

	inline static void initPicture(const std::string& name, const std::filesystem::path& path)
	{
		pictures.insert({ name, new sf::Image() });

		if(!pictures[name]->loadFromFile(path.string()))
			throw std::exception("Nie mozna utworzyc obrazu\n");
	}

public:
	//Accessors:
	inline static sf::Texture* getTexture(const std::string& id) { return textures[id]; }
	inline static sf::Image* getImage(const std::string& id) { return pictures[id]; }
	inline static sf::Font* getFont() { return &font; }
};