// ----- << External Library >> ----- //
#include "TextSection.hpp"

// ----- << Implementation >> ----- //
TextSection::TextSection() = default;

TextSection::TextSection(TextSection&& source) noexcept
	: instructions(std::move(source.instructions))
{

}

TextSection& TextSection::operator=(TextSection&& source) noexcept
{
	if (this != &source)
	{
		instructions = std::move(source.instructions);
	}

	return *this;
}

TextSection::~TextSection() = default;

void TextSection::addInstruction(std::unique_ptr<Instruction> instruction)
{
	instructions.push_back(std::move(instruction));
}

const std::unique_ptr<Instruction>& TextSection::getInstruction(size_t index) const
{
	return instructions.at(index);
}

const std::vector<std::unique_ptr<Instruction>>& TextSection::getInstructionAll() const
{
	return instructions;
}