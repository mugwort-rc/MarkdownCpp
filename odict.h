/*
 * odict.h
 *
 *  Created on: 2013/10/27
 *      Author: mugwort_rc
 */

#ifndef ODICT_H_
#define ODICT_H_

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

namespace markdown{

/*!
 * A dictionary that keeps its keys in the order in which they're inserted.
 *
 * Copied from Django's SortedDict with some modifications.
 *
 */
template <class T>
class OrderedDict
{
public:
    typedef boost::shared_ptr<T> Ptr;
    typedef std::vector<Ptr> Sequence;

public:
    OrderedDict() :
        _dict(), _keyOrder()
    {}
    OrderedDict(const OrderedDict& copy) :
        _dict(copy._dict), _keyOrder(copy._keyOrder)
    {}
    OrderedDict& operator =(const OrderedDict& rhs)
    {
        this->_dict     = rhs._dict;
        this->_keyOrder = rhs._keyOrder;
        return *this;
    }

    /*!
     * Returns the value of the item at the given zero-based index.
     */
    Ptr at(int i) const
    {
        if ( i < 0 ) {
            i = this->_dict.size() + i;
        }
        return this->_dict.at(this->_keyOrder.at(i));
    }
    Ptr operator [](int i) const
    {
        if ( i < 0 ) {
            i = this->_dict.size() + i;
        }
        return this->_dict.at(this->_keyOrder.at(i));
    }
    Ptr operator [](const std::string& key) const
    {
        return this->_dict.at(key);
    }
    bool exists(const std::string& key)
    {
        return this->_dict.find(key) != this->_dict.end();
    }
    /*!
     * Return the index of a given key.
     */
    int index(const std::string& key)
    {
        if ( std::find(this->_keyOrder.begin(), this->_keyOrder.end(), key) == this->_keyOrder.end() ) {
            return -1;
        }
        int result = 0;
        for ( auto it = this->_keyOrder.begin(); it != this->_keyOrder.end(); ++it, ++result ) {
            if ( *it == key ) {
                break;
            }
        }
        return result;
    }
    std::size_t size(void) const
    {
        return this->_dict.size();
    }
    void clear(void)
    {
        this->_dict.clear();
        this->_keyOrder.clear();
    }

    void append(const std::string& key, const Ptr& val)
    {
        if (  this->index(key) == -1 ) {
            this->_keyOrder.push_back(key);
        }
        this->_dict[key] = val;
    }
    /*!
     * Insert by key location.
     */
    void add(const std::string& key, const Ptr& val, const std::string& location)
    {
        int i = this->index_for_location(location);
        if ( i < 0 ) {
            this->append(key, val);
        } else {
            this->insert(i, key, val);
        }
    }
    /*!
     * Inserts the key, value pair before the item with the given index.
     */
    void insert(int index, const std::string& key, const Ptr& val)
    {
        if ( this->exists(key) ) {
            boost::range::remove_erase(this->_keyOrder, key);
        }
        if ( index < 0 ) {
            index = this->_dict.size() + index;
        }
        auto it = this->_keyOrder.begin();
        for ( int i = 0; i < index; ++i, ++it ) {
            if ( it == this->_keyOrder.end() ) {
                break;
            }
        }
        this->_keyOrder.insert(it, key);
        this->append(key, val);
    }
    /*!
     * Change location of an existing item.
     */
    void link(const std::string& key, const std::string& location)
    {
        int n = this->index(key);
        if ( n < 0 ) {
            n = this->_dict.size() + n;
        }
        int counter = 0;
        boost::range::remove_erase_if(this->_keyOrder, [&](const std::string&) -> bool{ bool result = counter == n; ++counter; return result;  });
        try {
            int i = this->index_for_location(location);
            if ( i < 0 ) {
                this->_keyOrder.push_back(key);
            } else {
                auto it = this->_keyOrder.begin();
                for ( int x = 0; x < i; ++x, ++it ) {
                    if ( it == this->_keyOrder.end() ) {
                        break;
                    }
                }
                this->_keyOrder.insert(it, key);
            }
        } catch ( const std::invalid_argument&) {
            //! restore to prevent data loss and reraise
        }

    }

    Sequence toList(void) const
    {
        Sequence result;
        for ( const std::string& key : this->_keyOrder ) {
            result.push_back(this->_dict.at(key));
        }
        return result;
    }

private:
    /*!
     * Return index or None for a given location.
     */
    int index_for_location(const std::string& location)
    {
        int i = 0;
        if ( location == "_begin" ) {
            i = 0;
        } else if ( location == "_end" ) {
            i = -1;
        } else if ( location.substr(0, 1) == "<" || location.substr(0, 1) == ">" ) {
            i = this->index(location.substr(1, location.size()-1));
            if ( location.substr(0, 1) == ">" ) {
                if ( i >= this->_dict.size() ) {
                    //! last item
                    i = -1;
                } else {
                    i = i + 1;
                }
            }
        } else {
            throw std::invalid_argument("Not a valid location: \"" + location + "\". Location key "
                                        "must start with a \">\" or \"<\".");
        }
        return i;
    }

private:
    std::map<std::string, Ptr> _dict;
    std::vector<std::string> _keyOrder;

};

} // end of namespace markdown

#endif /* ODICT_H_ */
