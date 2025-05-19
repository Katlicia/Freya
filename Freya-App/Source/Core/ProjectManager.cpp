#include "ProjectManager.h"

#ifdef _WIN32
#include <shlobj.h>
#elif defined(__APPLE__)
#include <pwd.h>
#include <unistd.h>
#else
#include <pwd.h>
#include <unistd.h>
#endif


using namespace FRE;

ProjectManager::ProjectManager(Canvas& canvas)
    : m_Canvas(canvas)
{
}

bool ProjectManager::SaveProject(const std::string& filePath)
{

    std::ofstream file(filePath, std::ios::binary);
    const char magic[] = "FREYA01";
    file.write(magic, sizeof(magic));
    
    if (!file.is_open()) return false;

    sf::Vector2u size = m_Canvas.GetSize();
    bool transparent = m_Canvas.IsTransparent();

    sf::Image image = m_Canvas.GetRenderTexture().getTexture().copyToImage();
    const std::uint8_t* pixels = image.getPixelsPtr();
    std::uint32_t pixelCount = size.x * size.y * 4;

    file.write(reinterpret_cast<const char*>(&size.x), sizeof(size.x));
    file.write(reinterpret_cast<const char*>(&size.y), sizeof(size.y));
    file.write(reinterpret_cast<const char*>(&transparent), sizeof(transparent));
    file.write(reinterpret_cast<const char*>(&pixelCount), sizeof(pixelCount));
    file.write(reinterpret_cast<const char*>(pixels), pixelCount);

    return true;
}

bool ProjectManager::LoadProject(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary);

    char magic[8];
    file.read(magic, sizeof(magic));

    if (std::string(magic) != "FREYA01") {
        return false;
    }

    if (!file.is_open()) return false;

    sf::Vector2u size;
    bool transparent;
    std::uint32_t pixelCount;

    file.read(reinterpret_cast<char*>(&size.x), sizeof(size.x));
    file.read(reinterpret_cast<char*>(&size.y), sizeof(size.y));
    file.read(reinterpret_cast<char*>(&transparent), sizeof(transparent));
    file.read(reinterpret_cast<char*>(&pixelCount), sizeof(pixelCount));

    std::vector<std::uint8_t> pixelData(pixelCount);
    file.read(reinterpret_cast<char*>(pixelData.data()), pixelCount);

    m_Canvas.SetTransparent(transparent);
    m_Canvas.SetSize(size.x, size.y);

    sf::Image image(size, pixelData.data());

    sf::Texture texture;
    texture.loadFromImage(image);
    sf::Sprite sprite(texture);

    m_Canvas.Clear(transparent ? sf::Color(0, 0, 0, 0) : sf::Color::White);

    sf::RenderStates states;
    states.blendMode = sf::BlendNone;

    m_Canvas.GetRenderTexture().draw(sprite, states);
    m_Canvas.GetRenderTexture().display();

    return true;
}

std::filesystem::path ProjectManager::GetDefaultProjectDirectory()
{
    std::filesystem::path docsPath;

#ifdef _WIN32
    PWSTR path = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &path)))
    {
        docsPath = path;
        CoTaskMemFree(path);
    }
#elif defined(__APPLE__) || defined(__linux__)
    const char* homeDir = getenv("HOME");
    if (!homeDir) {
        struct passwd* pw = getpwuid(getuid());
        homeDir = pw ? pw->pw_dir : ".";
    }
    docsPath = std::filesystem::path(homeDir) / "Documents";
#endif

    std::filesystem::path freyaPath = docsPath / "Freya Projects";
    std::filesystem::create_directories(freyaPath);

    return freyaPath;
}

std::string ProjectManager::GenerateUniqueProjectName(const std::filesystem::path& dir, const std::string& baseName)
{
    std::string name = baseName;
    std::string fullPath = (dir / (name + ".freya")).string();
    int counter = 1;

    while (std::filesystem::exists(fullPath))
    {
        name = baseName + "(" + std::to_string(counter) + ")";
        fullPath = (dir / (name + ".freya")).string();
        counter++;
    }

    return name;
}