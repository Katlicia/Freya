#include "Canvas.h"
#include <iostream>

using namespace FRE;

Canvas::Canvas()
	: m_Size(800, 600)
{
	if (!m_RenderTexture.resize({ m_Size.x, m_Size.y }))
	{
		throw std::runtime_error("Failed to create RenderTexture");
	}
	if (m_Transparent)
	{
		m_RenderTexture.clear(sf::Color(255, 255, 255, 0));
	}
	else
	{
		m_RenderTexture.clear(sf::Color::White);
	}
	m_Sprite.emplace(m_RenderTexture.getTexture());

	CreateTransparencyPattern();
}

Canvas::Canvas(unsigned int width, unsigned height, bool transparant = false) : m_Size({ width, height }), m_Transparent(transparant)
{
	if (!m_RenderTexture.resize({ m_Size.x, m_Size.y }))
	{
		throw std::runtime_error("Failed to create RenderTexture");
	}
	if (m_Transparent)
	{
		m_RenderTexture.clear(sf::Color(255, 255, 255, 0));
	}
	else
	{
		m_RenderTexture.clear(sf::Color::White);
	}
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

void Canvas::SetSize(unsigned int width, unsigned int height)
{
	// Keep old content
	sf::Texture oldTex = m_RenderTexture.getTexture();
	sf::Image  oldImg = oldTex.copyToImage();

	// Resize RenderTexture
	m_Size.x = width;
	m_Size.y = height;
	if (!m_RenderTexture.resize({ m_Size.x, m_Size.y }))
		throw std::runtime_error("Failed to resize RenderTexture");

	if (m_Transparent)
	{
		m_RenderTexture.clear(sf::Color(255, 255, 255, 0));
	}
	else
	{
		m_RenderTexture.clear(sf::Color::White);
	}

	// Draw old content with sprite
	sf::Texture tmpTex;
	tmpTex.loadFromImage(oldImg);
	sf::Sprite  tmpSpr(tmpTex);
	// Scalable
	// tmpSpr.setScale(width / float(oldImg.getSize().x), height / float(oldImg.getSize().y));
	m_RenderTexture.draw(tmpSpr);

	m_RenderTexture.display();
	m_Sprite->setTexture(m_RenderTexture.getTexture(), true);

	// Update transparency pattern
	if (m_HasTransparencyPattern) {
		m_TransparencyPatternSprite->setTextureRect(
			sf::IntRect({ 0,0 }, { int(width), int(height) }));
	}
}

bool Canvas::ExportToPNG(const std::string& filename) {
	sf::Texture texture = GetRenderTexture().getTexture();
	sf::Image image = texture.copyToImage();
	return image.saveToFile(filename);
}

bool Canvas::ExportToJPG(const std::string& filename) {
	sf::Texture texture = GetRenderTexture().getTexture();
	sf::Image image = texture.copyToImage();
	return image.saveToFile(filename);
}

bool Canvas::ExportToBMP(const std::string& filename) {
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
	const int squareSize = 8; // Square size
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

void Canvas::SetTransparent(bool transparent) {
	m_Transparent = transparent;

	sf::Texture oldTex = m_RenderTexture.getTexture();
	sf::Image oldImg = oldTex.copyToImage();

	if (m_Transparent) {
		m_RenderTexture.clear(sf::Color(0, 0, 0, 0));
	}
	else {
		m_RenderTexture.clear(sf::Color::White);
	}

	sf::Texture tmpTex;
	tmpTex.loadFromImage(oldImg);
	sf::Sprite tmpSpr(tmpTex);
	m_RenderTexture.draw(tmpSpr);
	m_RenderTexture.display();
}

bool Canvas::IsTransparent() const {
	return m_Transparent;
}

void Canvas::SaveState()
{
	CanvasState state;
	state.image = m_RenderTexture.getTexture().copyToImage();
	state.size = m_Size;

	m_UndoStack.push(std::move(state));
}

bool Canvas::Undo()
{

	if (m_UndoStack.empty())
		return false;

	CanvasState state = std::move(m_UndoStack.top());
	m_UndoStack.pop();

	if (state.size != m_Size)
	{
		m_Size = state.size;
		if (!m_RenderTexture.resize(m_Size))
		{
			std::cerr << "Failed to resize RenderTexture during undo operation" << std::endl;
			return false;
		}
	}

	sf::Texture texture;
	if (!texture.loadFromImage(state.image))
	{
		std::cerr << "Failed to load texture from image during undo operation" << std::endl;
		return false;
	}

	if (m_Transparent)
	{
		m_RenderTexture.clear(sf::Color(255, 255, 255, 0));
	}
	else
	{
		m_RenderTexture.clear(sf::Color::White);
	}

	sf::Sprite restoreSprite(texture);
	m_RenderTexture.draw(restoreSprite);
	m_RenderTexture.display();

	if (m_Sprite.has_value())
	{
		m_Sprite->setTexture(m_RenderTexture.getTexture(), true);
	}

	return true;
}

bool Canvas::CanUndo() const
{
	return !m_UndoStack.empty();
}

void Canvas::BeginDrawOperation()
{
	if (!m_IsDrawingOperation)
	{
		SaveState();
		m_IsDrawingOperation = true;
	}
}

void Canvas::EndDrawOperation()
{
	m_IsDrawingOperation = false;
}
void Canvas::Rotate90()
{
	SaveState();
	sf::Image src = m_RenderTexture.getTexture().copyToImage();
	sf::Vector2u size = src.getSize();
	sf::Image dest(sf::Vector2u(size.y, size.x));

	for (unsigned int x = 0; x < size.x; ++x)
		for (unsigned int y = 0; y < size.y; ++y)
			dest.setPixel({ size.y - y - 1, x }, src.getPixel({ x, y }));

	SetSize(size.y, size.x);
	sf::Color clearColor = m_Transparent ? sf::Color(0, 0, 0, 0) : sf::Color::White;
	m_RenderTexture.clear(clearColor);

	sf::Texture tmpTex;
	tmpTex.loadFromImage(dest);
	sf::RenderStates states;
	states.blendMode = sf::BlendNone;
	m_RenderTexture.draw(sf::Sprite(tmpTex), states);
	m_RenderTexture.display();
}

void Canvas::RotateCounterClockwise90()
{
	SaveState();
	sf::Image src = m_RenderTexture.getTexture().copyToImage();
	sf::Vector2u size = src.getSize();
	sf::Image dest(sf::Vector2u(size.y, size.x));

	for (unsigned int x = 0; x < size.x; ++x)
		for (unsigned int y = 0; y < size.y; ++y)
			dest.setPixel({ y, size.x - x - 1 }, src.getPixel({ x, y }));

	SetSize(size.y, size.x);
	sf::Color clearColor = m_Transparent ? sf::Color(0, 0, 0, 0) : sf::Color::White;
	m_RenderTexture.clear(clearColor);

	sf::Texture tmpTex;
	tmpTex.loadFromImage(dest);
	sf::RenderStates states;
	states.blendMode = sf::BlendNone;
	m_RenderTexture.draw(sf::Sprite(tmpTex), states);
	m_RenderTexture.display();
}

void Canvas::Rotate180()
{
	SaveState();
	sf::Image src = m_RenderTexture.getTexture().copyToImage();
	sf::Vector2u size = src.getSize();
	sf::Image dest(size);

	for (unsigned int x = 0; x < size.x; ++x)
		for (unsigned int y = 0; y < size.y; ++y)
			dest.setPixel({ size.x - x - 1, size.y - y - 1 }, src.getPixel({ x, y }));

	sf::Color clearColor = m_Transparent ? sf::Color(0, 0, 0, 0) : sf::Color::White;
	m_RenderTexture.clear(clearColor);

	sf::Texture tmpTex;
	tmpTex.loadFromImage(dest);
	sf::RenderStates states;
	states.blendMode = sf::BlendNone;
	m_RenderTexture.draw(sf::Sprite(tmpTex), states);
	m_RenderTexture.display();
}

void Canvas::FlipHorizontal()
{
	SaveState();
	sf::Image src = m_RenderTexture.getTexture().copyToImage();
	sf::Vector2u size = src.getSize();
	sf::Image dest(size);

	for (unsigned int x = 0; x < size.x; ++x)
		for (unsigned int y = 0; y < size.y; ++y)
			dest.setPixel({ size.x - x - 1, y }, src.getPixel({ x, y }));

	sf::Color clearColor = m_Transparent ? sf::Color(0, 0, 0, 0) : sf::Color::White;
	m_RenderTexture.clear(clearColor);

	sf::Texture tmpTex;
	tmpTex.loadFromImage(dest);
	sf::RenderStates states;
	states.blendMode = sf::BlendNone;
	m_RenderTexture.draw(sf::Sprite(tmpTex), states);
	m_RenderTexture.display();
}

void Canvas::FlipVertical()
{
	SaveState();
	sf::Image src = m_RenderTexture.getTexture().copyToImage();
	sf::Vector2u size = src.getSize();
	sf::Image dest(size);

	for (unsigned int x = 0; x < size.x; ++x)
		for (unsigned int y = 0; y < size.y; ++y)
			dest.setPixel({ x, size.y - y - 1 }, src.getPixel({ x, y }));

	sf::Color clearColor = m_Transparent ? sf::Color(0, 0, 0, 0) : sf::Color::White;
	m_RenderTexture.clear(clearColor);

	sf::Texture tmpTex;
	tmpTex.loadFromImage(dest);
	sf::RenderStates states;
	states.blendMode = sf::BlendNone;
	m_RenderTexture.draw(sf::Sprite(tmpTex), states);
	m_RenderTexture.display();
}
