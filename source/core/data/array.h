////////////////////////////////////////////////////////////////////////////////
//    ScopeFun Oscilloscope ( http://www.scopefun.com )
//    Copyright (C) 2016 - 2021 David Košenina
//
//    This file is part of ScopeFun Oscilloscope.
//
//    ScopeFun Oscilloscope is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    ScopeFun Oscilloscope is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this ScopeFun Oscilloscope.  If not, see <http://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __ARRAY__
#define __ARRAY__

#if defined(__GNUC__)
#pragma GCC system_header
#endif

#include <algorithm>

////////////////////////////////////////////////////////////////////////////////
//
// static array
//
////////////////////////////////////////////////////////////////////////////////
template<class T, int size> class Array
{
    static_assert(size > 0, "Array size must be positive");
private:
    struct Cell
    {
        T value;

        Cell() = default;
        Cell(const T& value) : value(value) {}

        Cell& operator=(const T& value)
        {
            this->value = value;
            return *this;
        }

        Cell& operator=(const Cell& other)
        {
            value = other.value;
            return *this;
        }
    };

    Cell data[(size > 0) ? size : 1];
    int count;
public:
    Array()
    {
        count = 0;
    }
public:
    int setCount(int count)
    {
        if(count >= 0 && count <= size)
        {
            this->count = count;
            return 0;
        }
        return 1;
    }

    int getCount() const
    {
        return count;
    }

    int getSize() const
    {
        return size;
    }
public:
    T& index(int idx)
    {
        if(idx < 0 || idx >= count)
        {
            CORE_ABORT("idx out of range", 0);
        }
        return data[idx].value;
    }

    const T& index(int idx) const
    {
        if(idx < 0 || idx >= count)
        {
            CORE_ABORT("idx out of range", 0);
        }
        return data[idx].value;
    }

    int find(const T& el) const
    {
        for(int idx = 0; idx < count; idx++)
        {
            if(data[idx].value == el)
            {
                return idx;
            }
        }
        return -1;
    }

    void insert(int index, const T& el)
    {
        int oldCount = count;
        if (oldCount < getSize()) {
            setCount(oldCount + 1);
            for(int idx = oldCount; idx > index; idx--)
            {
                data[idx].value = data[idx - 1].value;
            }
            data[index].value = el;
        }
        else {
            CORE_ABORT("count out of range", 0);
        }
    }

    void remove(int index)
    {
        for(int idx = index; idx + 1 < count; idx++)
        {
            data[idx].value = data[idx + 1].value;
        }
        setCount(count - 1);
    }

    void clear()
    {
        count = 0;
    }

    int findRemove(const T& el)
    {
        int idx = find(el);
        if(idx >= 0)
        {
            remove(idx);
            return 0;
        }
        return 1;
    }

    T& first()
    {
        return index(0);
    }

    const T& first() const
    {
        return index(0);
    }

    T& last()
    {
        return index(getCount() - 1);
    }

    const T& last() const
    {
        return index(getCount() - 1);
    }

    T& add()
    {
        T element;
        pushBack(element);
        return last();
    }

    void pushBack(const T& el)
    {
        insert(count, el);
    }

    void pushFront(const T& el)
    {
        insert(0, el);
    }

    void popBack()
    {
        remove(count - 1);
    }

    void popFront()
    {
        remove(0);
    }
public:
    operator int () const
    {
        return getCount();
    }

    const T& operator[](int idx) const
    {
        return index(idx);
    }

    T& operator[](int idx)
    {
        return index(idx);
    }
};

#endif
////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
