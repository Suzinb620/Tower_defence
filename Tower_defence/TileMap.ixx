export module TileMap;
import <SFML/System.hpp>;
import <SFML/Graphics.hpp>;
import <filesystem>;
import <functional>;
import <algorithm>;
import <iostream>;
import <thread>;
import <vector>;
import <atomic>;
import <mutex>;

export class TileMap : public sf::Drawable
{
private:
    //Variables:
    sf::VertexArray vertices;
    sf::Vector2i tileSize;
    sf::Texture tileSet;
    int width;
    int height;
    std::vector<sf::Vector2f> path;
    sf::Vector2f start;
    sf::Vector2f end;
    std::mutex mutex;

public:
    //Constructors/destructors:
    TileMap(const sf::Texture& tileSet, const sf::Vector2i& tileSize, const std::vector<int>& tiles, const int& width, const int& height, const int& whichTailMakesPath) : tileSet(tileSet), width(width), height(height), tileSize(tileSize), start(sf::Vector2f(0, 0)), end(sf::Vector2f(0, 0))
    {
        initTileMap(tiles, whichTailMakesPath);
        initPath(whichTailMakesPath);
    }

    //Initialization:
    void initTileMap(const std::vector<int>& tiles, const int& whichTailMakesPath)
    {
        vertices.setPrimitiveType(sf::Quads);
        vertices.resize(this->width * this->height * 4);

        parallelFor(this->width, [&](std::size_t start, std::size_t end)
            {
                for (std::size_t i = start; i < end; i++)
                    parallelFor(this->height, [&](std::size_t start, std::size_t end)
                        {
                            for (std::size_t j = start; j < end; j++)
                                initTile(tiles, whichTailMakesPath, i, j);
                        });
            });
    }

    void initTile(const std::vector<int>& tiles, const int& whichTailMakesPath, const std::size_t& i, const std::size_t& j)
    {
        const std::lock_guard<std::mutex> lock(this->mutex);

        int currentTileNumber = tiles[i + j * this->width];
        setStartAndEnd(currentTileNumber, i, j, whichTailMakesPath);

        //find its position in the tileset texture
        int tu = currentTileNumber % (this->tileSet.getSize().x / this->tileSize.x);
        int tv = currentTileNumber / (this->tileSet.getSize().x / this->tileSize.x);

        sf::Vertex* quad = &vertices[(i + j * this->width) * 4];
        //define its 4 corners
        quad[0].position = sf::Vector2f(i * this->tileSize.x, j * this->tileSize.y);
        quad[1].position = sf::Vector2f((i + 1) * this->tileSize.x, j * this->tileSize.y);
        quad[2].position = sf::Vector2f((i + 1) * this->tileSize.x, (j + 1) * this->tileSize.y);
        quad[3].position = sf::Vector2f(i * this->tileSize.x, (j + 1) * this->tileSize.y);
        //define its 4 texture coordinates
        quad[0].texCoords = sf::Vector2f(tu * this->tileSize.x, tv * this->tileSize.y);
        quad[1].texCoords = sf::Vector2f((tu + 1) * this->tileSize.x, tv * this->tileSize.y);
        quad[2].texCoords = sf::Vector2f((tu + 1) * this->tileSize.x, (tv + 1) * this->tileSize.y);
        quad[3].texCoords = sf::Vector2f(tu * this->tileSize.x, (tv + 1) * this->tileSize.y);
    }

    void initPath(const int& whichTailMakesPath)
    {
        path.push_back(this->start);
        sf::Vector2f temp = this->start;
        while (temp != this->end)
        {
            if (checkPosition(temp += sf::Vector2f(this->tileSize.x, 0), whichTailMakesPath) && !compareToPenultimateInPath(temp))
                this->path.push_back(temp);
            else if (checkPosition(temp += sf::Vector2f(-this->tileSize.x, this->tileSize.y), whichTailMakesPath) && !compareToPenultimateInPath(temp))
                this->path.push_back(temp);
            else if (checkPosition(temp += sf::Vector2f(0, -2 * this->tileSize.y), whichTailMakesPath) && !compareToPenultimateInPath(temp))
                this->path.push_back(temp);
            else if (checkPosition(temp += sf::Vector2f(-this->tileSize.x, this->tileSize.y), whichTailMakesPath) && !compareToPenultimateInPath(temp))
                this->path.push_back(temp);
            else throw std::exception("blad podczas szukania sciezki\n");
        }
    }

private:
    //Methods:
    bool compareToPenultimateInPath(const sf::Vector2f& toCompare)
    {
        if (this->path.size() > 1)
            return this->path.rbegin()[1] == toCompare ? true : false;
        else return false;
    }

    void setStartAndEnd(int& tileNumber, const std::size_t& i, const std::size_t& j, const int& whichTailMakesPath)
    {
        if (tileNumber == -1)
        {
            set(this->start, i, j);
            tileNumber = whichTailMakesPath;
        }

        if (tileNumber == -2)
        {
            set(this->end, i, j);
            tileNumber = whichTailMakesPath;
        }
    }

    void set(sf::Vector2f& toSet, const std::size_t& i, const std::size_t& j)
    {
        if (toSet == sf::Vector2f(0, 0))
            toSet = sf::Vector2f(i * this->tileSize.x, j * this->tileSize.y);
        else throw std::exception("blednie skonstruowany level\n");
    }

    void parallelFor(size_t numOfElements, std::function<void(int start, int end)> functor)
    {
        std::size_t numOfThreads = std::thread::hardware_concurrency();
        std::vector<std::jthread> threads(numOfThreads);
        std::size_t batchSize = numOfElements / numOfThreads;
        std::size_t batchRemainder = numOfElements % numOfThreads;

        for (std::size_t i = 0; i < numOfThreads; i++)
        {
            std::size_t start = i * batchSize;
            threads[i] = std::jthread(functor, start, start + batchSize);
        }

        if (batchRemainder != 0)
        {
            std::size_t start = numOfThreads * batchSize;
            functor(start, start + batchRemainder);
        }
    }

    void draw(sf::RenderTarget& renderTarget, sf::RenderStates renderStates) const override
    {
        renderStates.texture = &this->tileSet;
        renderTarget.draw(this->vertices, renderStates);
    }

public:
    bool checkPosition(sf::Vector2f position, int goodTile)
    {
        if (vertices.getBounds().contains(position))
        {
            //set the good tile
            int x = goodTile * this->tileSize.x;  

            //find quad in vertices that contain position
            sf::Vertex* quad = &vertices[(((int)position.x / this->tileSize.x) + (((int)position.y / this->tileSize.y) * this->width)) * 4];

            //check that coords fit good tile
            if (quad[0].texCoords == sf::Vector2f(x, 0) && quad[1].texCoords == sf::Vector2f(x + this->tileSize.x, 0) && quad[2].texCoords == sf::Vector2f(x + this->tileSize.x, this->tileSize.y) && quad[3].texCoords == sf::Vector2f(x, this->tileSize.y))
                return true;
            else return false;
        }
        else return false;
    }

    //Accessors:
    const std::vector<sf::Vector2f>* const getPath() { return &this->path; }
};