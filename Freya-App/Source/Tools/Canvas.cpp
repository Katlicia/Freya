#include "Canvas.h"

using namespace FRE;

Canvas::Canvas()
	: m_Size(800, 600)
{
	if (!m_RenderTexture.resize({ m_Size.x, m_Size.y }))
	{
		throw std::runtime_error("Failed to create RenderTexture");
	}
	m_RenderTexture.clear(sf::Color::White);
	//m_RenderTexture.clear(sf::Color(255, 255, 255, 0));
	m_Sprite.emplace(m_RenderTexture.getTexture());

	CreateTransparencyPattern();
}

Canvas::Canvas(unsigned int width, unsigned height) : m_Size({ width, height })
{
	if (!m_RenderTexture.resize({ m_Size.x, m_Size.y }))
	{
		throw std::runtime_error("Failed to create RenderTexture");
	}
	m_RenderTexture.clear(sf::Color::White);
	//m_RenderTexture.clear(sf::Color(255, 255, 255, 0));
	m_Sprite.emplace(m_RenderTexture.getTexture());

	CreateTransparencyPattern();
}

void Canvas::Update(const sf::RenderWindow& window, const sf::View& view)
{
	Display();
	// Create sprite if it doesn't exist, update if does exist
	if (!m_Sprite.has_value())
		m_Sprite.emplace(m_RenderTexture.getTexture());
	else
		m_Sprite->setTexture(m_RenderTexture.getTexture(), true);

	// Allign canvas to the center of the view
	sf::Vector2f viewCenter = window.getView().getCenter();
	sf::Vector2f canvasSizeF = static_cast<sf::Vector2f>(m_RenderTexture.getSize());
	sf::Vector2f offset;
	offset.x = viewCenter.x - (canvasSizeF.x / 2.f);
	offset.y = viewCenter.y - (canvasSizeF.y / 2.f);
	m_Sprite->setPosition(offset);

	// Update same position for transparency pattern
	if (m_HasTransparencyPattern) {
		m_TransparencyPatternSprite->setPosition(offset);
		// If the canvas size is changed, update the size of the pattern
		m_TransparencyPatternSprite->setTextureRect(sf::IntRect({ 0, 0 }, { (int)m_Size.x, (int)m_Size.y }));
	}
}

void Canvas::DrawLine(const sf::Vector2f& start, const sf::Vector2f& end, const sf::Color& color)
{
	sf::Vertex line[] = {
		sf::Vertex(start, color),
		sf::Vertex(end, color)
	};

	sf::RenderStates states;
	states.blendMode = sf::BlendAlpha;
	m_RenderTexture.draw(line, 2, sf::PrimitiveType::Lines, states);
}

void Canvas::Clear(const sf::Color& color)
{
	m_RenderTexture.clear(color);
}

void Canvas::Display()
{
	m_RenderTexture.display();
}

sf::Vector2u Canvas::GetSize()
{
	return m_Size;
}

bool Canvas::ExportToPNG(const std::string& filename) {
	sf::Texture texture = GetRenderTexture().getTexture();
	sf::Image image = texture.copyToImage();
	return image.saveToFile(filename);
}

sf::Sprite& Canvas::GetSprite()
{
	if (!m_Sprite.has_value())
	{
		throw std::runtime_error("Sprite is not initialized!");
	}
	return *m_Sprite;
}

sf::RenderTexture& Canvas::GetRenderTexture()
{
	return m_RenderTexture;
}

void Canvas::CreateTransparencyPattern()
{
	// Create a texture similar to a chessboard pattern
	const int squareSize = 10; // Square size
	const int textureSize = squareSize * 2; // For 2x2 square

	sf::Image patternImage({ textureSize, textureSize }, sf::Color(200, 200, 200));

	// Create chessboard pattern
	for (int x = 0; x < textureSize; x++) {
		for (int y = 0; y < textureSize; y++) {
			if ((x / squareSize + y / squareSize) % 2 == 0) {
				sf::Vector2u coords = { (unsigned int)x, (unsigned int)y };
				patternImage.setPixel(coords, sf::Color(240, 240, 240));
			}
		}
	}

	if (!m_TransparencyPatternTexture.loadFromImage(patternImage)) {
		throw std::runtime_error("Failed to create transparency pattern texture");
	}

	// Use texture repeatedly
	m_TransparencyPatternTexture.setRepeated(true);

	// Set Sprite
	if (!m_TransparencyPatternSprite.has_value())
		m_TransparencyPatternSprite.emplace(m_TransparencyPatternTexture);
	else
		m_TransparencyPatternSprite->setTexture(m_TransparencyPatternTexture, true);

	m_HasTransparencyPattern = true;
}

void Canvas::DrawTransparencyPattern(sf::RenderTarget& target)
{
	if (m_HasTransparencyPattern) {
		// Set the pattern position to the canvas position
		m_TransparencyPatternSprite->setPosition(m_Sprite->getPosition());
		target.draw(*m_TransparencyPatternSprite);
	}
}