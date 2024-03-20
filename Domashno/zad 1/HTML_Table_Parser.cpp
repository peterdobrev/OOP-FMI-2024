#include <iostream>
#include <fstream>
#include <cstring>

/// <summary>
///				����� ����� ����� �� �������� -> ��������� �� ����� �������
/// </summary>
/// 
/// 
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996)
#endif

constexpr uint8_t MAX_CELL_TEXT_SIZE = 50;
constexpr uint8_t MAX_CELLS = 30;
constexpr uint8_t MAX_ROWS = 300;
constexpr uint16_t MAX_CONTENT_SIZE = 1024;

size_t getMax(size_t a, size_t b)
{
	return a > b ? a : b;
}

size_t getMin(size_t a, size_t b)
{
	return a > b ? b : a;
}


size_t getFileSize(std::ifstream& file)
{
	size_t currentPos = file.tellg();

	file.seekg(0, std::ios::end);
	size_t fileSize = file.tellg();

	file.seekg(currentPos);
	return fileSize;
}

bool isPrefix(const char* text, const char* prefix)
{
	while (true)
	{
		if (*prefix == '\0')     // found match
		{
			return true;
		}
		else if (*text == '\0')  // prefix is longer than text
		{
			return false;
		}
		else if (*text != *prefix) // no match
		{
			return false;
		}
		else
		{
			prefix++;
			text++;
			continue;
		}
	}
}


bool readUntilChar(std::ifstream& file, char* buffer, size_t bufferSize, char find)
{
	if (!file.is_open() || !file.good()) return false;

	size_t i = 0;
	char ch;
	while (i < bufferSize - 1 && file.get(ch))
	{
		if (ch == find) break;
		buffer[i++] = ch;
	}
	buffer[i] = '\0'; // Null-terminate the string

	if (file.fail() && !file.eof())
	{
		file.clear(); // Clear fail state
		std::cerr << "Read error before reaching delimiter or EOF." << std::endl;
		return false;
	}

	//std::cout << "Read content: " << buffer << std::endl;
	return true;
}



class Cell
{
	char text[MAX_CELL_TEXT_SIZE + 1] = "";
	bool isHeader = false;

private:
	void setText(const char* _text, size_t length)
	{
		if (length > MAX_CELL_TEXT_SIZE)
		{
			strcpy(text, "");
		}
		else
		{
			strcpy(text, _text);
		}
	}

	void setHeader(bool _isHeader)
	{
		isHeader = _isHeader;
	}

public:
	Cell() = default;

	Cell(const char* _text) : Cell(_text, false)
	{

	}

	Cell(const char* _text, bool _isHeader)
	{
		size_t length = strlen(_text);
		setText(_text, length);
		setHeader(_isHeader);
	}

	const char* getText() const
	{
		return text;
	}

	bool getIsHeader() const
	{
		return isHeader;
	}
};

class Row
{
	Cell cells[MAX_CELLS] = {};
	uint8_t cellsCount = 0;

private:
	void setCells(const Cell* _cells, size_t length)
	{
		if (length > MAX_CELLS)
		{
			length = MAX_CELLS;
		}

		cellsCount = length;

		for (size_t i = 0; i < cellsCount; i++)
		{
			cells[i] = _cells[i];
		}
	}

public:
	Row() = default;

	Row(const Cell* cells, size_t cellsCount)
	{
		setCells(cells, cellsCount);
	}

	const Cell* getCells() const
	{
		return cells;
	}

	uint8_t getCellsCount() const
	{
		return cellsCount;
	}

	bool addCell(const Cell& cell, size_t index)
	{
		if (index > MAX_CELLS - 1)
		{
			std::cerr << "Cell index was out of bounds, couldn't add to row!" << std::endl;
			return false;
		}
		else
		{
			cells[index] = cell;
			cellsCount++;
		}
	}
};

class Table
{
	Row rows[MAX_ROWS] = {};
	uint8_t rowsCount = 0;
	uint8_t columnsCount = 0;

private:
	void setRows(const Row* _rows, size_t length)
	{
		if (length > MAX_ROWS)
		{
			length = MAX_ROWS;
		}

		rowsCount = length;

		for (size_t i = 0; i < rowsCount; i++)
		{
			rows[i] = _rows[i];
		}
	}

	void setCollumnsCount()
	{
		uint8_t maxCells = 0;

		for (size_t i = 0; i < rowsCount; i++)
		{
			maxCells = getMax(maxCells, rows[i].getCellsCount());
		}

		columnsCount = maxCells;
	}

public:
	Table() = default;

	Table(const Row* rows, size_t rowsCount)
	{
		setRows(rows, rowsCount);
	}

	const Row* getRows() const
	{
		return rows;
	}

	uint8_t getRowsCount() const
	{
		return rowsCount;
	}

	uint8_t getColumnsCount() const
	{
		return columnsCount;
	}

	bool addRow(const Row& row, size_t index)
	{
		if (index > MAX_ROWS - 1)
		{
			std::cerr << "Couldn't add row, invalid index" << std::endl;
			return false;
		}


		rows[index] = row;
		rowsCount++;
		setCollumnsCount();
	}

	void print() const
	{
		for (size_t rowIdx = 0; rowIdx < rowsCount; ++rowIdx)
		{
			// Fetch the current row.
			const Row& currentRow = rows[rowIdx];

			for (size_t cellIdx = 0; cellIdx < currentRow.getCellsCount(); ++cellIdx)
			{
				// Fetch the current cell from the row.
				const Cell& currentCell = currentRow.getCells()[cellIdx];

				// Check if the cell is a header and print accordingly.
				if (currentCell.getIsHeader())
				{
					std::cout << "[" << currentCell.getText() << "]";
				}
				else
				{
					std::cout << "(" << currentCell.getText() << ")";
				}

				// Print a separator between cells, if not the last cell.
				if (cellIdx < currentRow.getCellsCount() - 1)
				{
					std::cout << " | ";
				}
			}

			// Move to the next line after printing all cells in the row.
			std::cout << std::endl;
		}
	}
};


bool parseCell(std::ifstream& file, size_t fileSize, bool isHeader, Cell& outCell)
{
	char content[MAX_CONTENT_SIZE] = { 0 };

	if (!readUntilChar(file, content, sizeof(content),'<'))
	{
		std::cerr << "Error reading cell content." << std::endl;
		return false;
	}

	outCell = Cell(content, isHeader);
	std::cout << "Cell content: " << content << std::endl;
	return true;
}

bool parseRow(std::ifstream& file, size_t fileSize, Row& outRow)
{
	if (!file.is_open())
	{
		std::cerr << "File is not open." << std::endl;
		return false;
	}

	Cell cell;
	uint8_t cellIndex = 0;
	char buffer[MAX_CONTENT_SIZE];
	bool success = false; // Indicates if at least one cell was successfully parsed

	while (readUntilChar(file, buffer, sizeof(buffer), '<'))
	{
		readUntilChar(file, buffer, sizeof(buffer), '>');

		if (isPrefix(buffer, "/tr"))
		{
			// Successfully parsed the row if at least one cell was parsed
			return success;
		}
		else if(isPrefix(buffer, "/t"))
		{
			// Check for end tag (/td, /th)
			continue;
		} 

		bool isHeader = isPrefix(buffer, "th"); // Check if the next cell is a header
		bool cellParsed = parseCell(file, fileSize, isHeader, cell);
		success = success || cellParsed;

		if (cellParsed)
		{
			outRow.addCell(cell, cellIndex);
			cellIndex++;
		}
		else
		{
			std::cerr << "Error parsing cell in row." << std::endl;
			return false;
		}
	}

	// If we reach here without finding "</tr>", it's an incomplete row
	std::cerr << "Incomplete row. End tag </tr> not found." << std::endl;
	return false; // Row parsing failed
}



bool parseTable(std::ifstream& file, Table& outTable)
{
	if (!file.is_open())
	{
		std::cerr << "File is not open." << std::endl;
		return false;
	}

	char buffer[MAX_CONTENT_SIZE] = { 0 };
	bool success = false; // Indicates if at least one row was successfully parsed

	readUntilChar(file, buffer, sizeof(buffer), '<');
	readUntilChar(file, buffer, sizeof(buffer), '>');

	if(!isPrefix(buffer, "table"))
	{
		std::cerr << "Couldn't find opening tag" << std::endl;
		return false;
	}

	while (readUntilChar(file, buffer, sizeof(buffer),'<'))
	{
		readUntilChar(file, buffer, sizeof(buffer), '>');
		if (isPrefix(buffer, "/table"))
		{
			// Successfully parsed the table if at least one row was parsed
			return success;
		}


		if (isPrefix(buffer, "tr"))
		{
			Row newRow;
			if (parseRow(file, getFileSize(file), newRow))
			{
				outTable.addRow(newRow, outTable.getRowsCount()); // Assuming addRow adds to the end and manages the index internally
				success = true;
			}
			else
			{
				// If a row fails to parse, you can decide to continue or break based on your error handling strategy
			}
		}
	}

	// If we reach here without finding "</table>", it's an incomplete table
	std::cerr << "Incomplete table. End tag </table> not found." << std::endl;
	return false; // Table parsing failed
}

void readFile(const char* fileName)
{
	std::ifstream file;
	file.open(fileName);

	if (!file.is_open())
	{
		return;
	}

	Table table;
	bool success = parseTable(file, table);

	if (success)
		table.print();
}

int main()
{
	readFile("html_table.txt");
}