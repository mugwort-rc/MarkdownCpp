/*
 * tables.h
 *
 *  Created on: 2013/11/04
 *      Author: mugwort_rc
 */

#ifndef TABLES_H_
#define TABLES_H_

/*!
 * Tables Extension for Python-Markdown
 * ====================================
 *
 * Added parsing of tables to Python-Markdown.
 *
 * A simple example:
 *
 *    First Header  | Second Header
 *    ------------- | -------------
 *    Content Cell  | Content Cell
 *    Content Cell  | Content Cell
 *
 * Copyright 2009 - [Waylan Limberg](http://achinghead.com)
 */

#include "Extension.h"

namespace markdown{

/*!
 * Add tables to Markdown.
 */
class TableExtension : public Extension
{
public:
    TableExtension();

    void extendMarkdown(Markdown* md/*, md_globals*/);

public:
    static Extension::Ptr generate(void);

};

} // end of namespace markdown

#endif // TABLES_H_
