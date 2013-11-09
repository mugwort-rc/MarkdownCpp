/*
 * Processor.h
 *
 *  Created on: 2013/10/25
 *      Author: mugwort_rc
 */

#ifndef PROCESSOR_H_
#define PROCESSOR_H_

#include <list>

#include "odict.h"

namespace markdown{

class Markdown;  //!< forward declaration

class Processor
{
public:
	Processor(Markdown* markdown_instance=nullptr) :
		markdown(markdown_instance)
	{}
	virtual ~Processor(void)
	{}

    virtual std::list<std::wstring> run(const std::list<std::wstring>& lines) = 0;

protected:
	Markdown* markdown;

};

typedef OrderedDict<Processor> OrderedDictProcessors;

} // end of namespace markdown

#endif /* PROCESSOR_H_ */
