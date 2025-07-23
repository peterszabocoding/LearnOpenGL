#include "pch.h"
#include "TextureAtlas.h"

void TextureAtlas::Clear()
{
	m_TextureSizes.clear();
	m_Boxes.clear();
}

void TextureAtlas::AllocateTextureAtlas(glm::uvec2 const& atlasSize)
{
	// we have to separately sort the indices so that the i-th region
		// of the output corresponds to the i-th texture size of the input
	std::vector<uint16_t> sorted(m_TextureSizes.size());
	for (uint16_t i = 0; i < sorted.size(); ++i)
		sorted[i] = i;

	// sort in descending order
	std::sort(sorted.begin(), sorted.end(), [&](const unsigned int i, const unsigned int j) {
		return m_TextureSizes[i] > m_TextureSizes[j];
		});

	glm::uvec2 pen{ 0, 0 };
	std::vector<glm::uvec2> ladder;
	std::vector<AtlasBox> result(m_TextureSizes.size());

	for (const unsigned int i : sorted)
	{
		int const size = m_TextureSizes[i];

		// allocate a texture region
		result[i] = { {pen.x, pen.y}, {pen.x + size, pen.y + size} };

		m_Boxes[size].push_back({ {pen.x, pen.y}, {pen.x + size, pen.y + size} });

		// shift the pen to the right
		pen.x += size;

		// update the ladder
		if (!ladder.empty() && ladder.back().y == pen.y + size)
			ladder.back().x = pen.x;
		else
			ladder.emplace_back(pen.x, pen.y + size);

		if (pen.x == atlasSize.x)
		{
			// the pen hit the right edge of the atlas
			ladder.pop_back();

			pen.x = !ladder.empty() ? ladder.back().x : 0;
			pen.y += size;
		}
	}
	
}
