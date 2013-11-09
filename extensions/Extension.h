/*
 * Extension.h
 *
 *  Created on: 2013/11/04
 *      Author: mugwort_rc
 */

#ifndef EXTENSION_H_
#define EXTENSION_H_

#include <map>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

namespace markdown{

class Markdown;  //!< forward declaration

/*!
 * Base class for extensions to subclass.
 */
class Extension
{
public:
    typedef boost::shared_ptr<Extension> Ptr;
    typedef std::map<std::wstring, std::vector<std::wstring>> Config;

public:
    /*!
     * Create an instance of an Extention.
     *
     * Keyword arguments:
     *
     * * configs: A dict of configuration setting used by an Extension.
     */
    Extension(const Config& config=Config());
    virtual ~Extension(void);

    /*!
     * Return a setting for the given key or an empty string.
     */
    std::wstring getConfig(const std::wstring& key, const std::wstring& default_val = std::wstring()) const;
    /*!
     * Return all configs settings as a dict.
     */
    Config getConfigs(void) const
    { return this->config; }
    /*!
     * Set a config setting for `key` with the given `value`.
     */
    void setConfig(const std::wstring& key, const std::vector<std::wstring>& value);

    virtual void extendMarkdown(Markdown* md/*, md_globals*/) = 0;

    virtual void reset(void)
    {}

private:
    Config config;

};

} // end of namespace markdown

#endif // EXTENSION_H_
