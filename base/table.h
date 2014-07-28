#ifndef _THEFOX_BASE_TABLE_H_
#define _THEFOX_BASE_TABLE_H_

#include <assert.h>
#include <base/common.h>

namespace thefox {

template<typename T>
class Table
{
public:
	typedef std::vector<T> Row;
	typedef std::vector<Row> TableType;
	
	Table(void)
		: _rowIndex(-1)
	{}
	Table(const Table &from)
		: _table(from._table)
		, _rowIndex(from._rowIndex)
	{}
	~Table()
	{}

	const Table &operator=(const Table &from)
	{
		_table = from._table;
		_rowIndex = from._rowIndex;
		return *this;
	}

	int fieldCount() const
	{
		if (!_table.empty()) {
			return _table[0].size();
		}
		return 0;
	}

	int rowCount() const
	{ return _table.size(); }

	bool fetchRow()
	{
		if (_table.empty()) {
			return false;
		}
		if (++_rowIndex >= _table.size()) {
			return false;
		} else {
			return true;
		}
	}

	const T &operator[](const int index) const
	{ 
		assert(_rowIndex < _table.size());
		assert(index < _table[_rowIndex].size());

		return _table[_rowIndex][index]; 
	}

	operator bool() const
	{ return !_table.empty(); }

	Row *addRow()
	{ 
		Row row;
		_table.push_back(row);
		return &_table[_table.size() - 1];
	}

	void clear()
	{ _table.clear(); }

	bool empty() const
	{ return _table.empty(); }

private:
	TableType _table;
	int _rowIndex;
};

} // namespace thefox

#endif // _THEFOX_BASE_TABLE_H_