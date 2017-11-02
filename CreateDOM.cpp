#include "CreateDOM.h"
#include "FastXml.h"
#include "StringHelper.h"
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <stdlib.h>
#include <unordered_map>
#include <map>
#include <vector>
#include <algorithm>
#include <assert.h>

#pragma warning(disable:4100)

namespace CREATE_DOM
{

#define MAX_STACK_INDEX 32

enum KeyType
{
	KT_STRING,			// It's a string
	KT_FLOAT,			// It's a floating point number
	KT_ENUM,			// It's an enum
	KT_INTEGER,			// It's an integer value
	KT_BOOL,			// It's a bool true/false
	KT_BITFIELD,		// It's a collection of bitflags
	KT_LAST
};

static const char *getKeyTypeName(KeyType type)
{
	const char *ret = "**UNKNOWN**!";

	switch (type)
	{
		case KT_STRING:
			ret = "STRING";
			break;
		case KT_FLOAT:
			ret = "FLOAT";
			break;
		case KT_ENUM:
			ret = "ENUM";
			break;
		case KT_INTEGER:
			ret = "INT";
			break;
		case KT_BOOL:
			ret = "BOOL";
			break;
		case KT_BITFIELD:
			ret = "BITFIELD";
			break;
	}

	return ret;
}

static bool isDigit(char c)
{
	bool ret = false;

	if (c >= '0' && c <= '9')
	{
		ret = true;
	}
	else if (c == '-' || c == '+' || c == '.' )
	{
		ret = true;
	}

	return ret;
}

static KeyType getKeyType(const char *data)
{
	KeyType ret = KT_STRING; // default value

	if (strchr(data, '|'))
	{
		ret = KT_BITFIELD;
	}
	else if (*data == '@')
	{
		ret = KT_STRING;
	}
	else if (*data == 'e')
	{
		ret = KT_ENUM;
	}
	else if (strcmp(data, "true") == 0 || strcmp(data, "false") == 0)
	{
		ret = KT_BOOL;
	}
	else if (isDigit(*data))
	{
		if (strchr(data, '.'))
		{
			ret = KT_FLOAT;
		}
		else
		{
			ret = KT_INTEGER;
		}
	}

	return ret;
}

class KeyValuePair
{
public:
	KeyValuePair(void)
	{

	}
	KeyValuePair(const char *keyName,const char *keyData)
	{
		mKeyName = std::string(keyName);
		mType = getKeyType(keyData);
	}

	void saveDOM(FILE *fph)
	{
		fprintf(fph, ",%s,%s,,,,,ShortDescription,LongDescription\r\n", mKeyName.c_str(), getKeyTypeName(mType));
	}

	std::string	mKeyName;
	KeyType		mType{ KT_LAST };
};

typedef std::vector< KeyValuePair > KeyValuePairVector;
typedef std::vector< std::string > StringVector;

class ElementType;
typedef std::vector< ElementType *> ElementTypeVector;

class ElementType
{
public:
	ElementType(void)
	{

	}
	ElementType(const char *str)
	{
		mElementName = std::string(str);
	}

	void addKeyValuePair(const char *key, const char *value)
	{
		std::string skey(key);
		for (auto &i : mKeyValues)
		{
			if (i.mKeyName == skey)
			{
				return; // already represented
			}
		}
		KeyValuePair kvp(key, value);
		mKeyValues.push_back(kvp);
	}

	void saveDOM(FILE *fph,ElementTypeVector &elements)
	{
		if (mSaved) return; // if this one has already been saved...
		// We must recursively save out any dependent children!
		for (auto &i : mChildren)
		{
			for (auto &j : elements)
			{
				if (j->mElementName == i)
				{
					j->saveDOM(fph,elements);
				}
			}
		}

		mSaved = true;
		fprintf(fph, "\r\n");
		fprintf(fph, "%s,,Class,%s,,,,ShortDescription,DetailedDescription\r\n", mElementName.c_str(), mParentName.c_str());
		fprintf(fph, "\r\n");
		for (auto &i : mChildren)
		{
			fprintf(fph, ",%s,%s,,,,,ShortDescription,DetailedDescription\r\n", i.c_str(), i.c_str());
		}
		for (auto &i : mKeyValues)
		{
			i.saveDOM(fph);
		}
		fprintf(fph, "\r\n");
	}

	void addChild(const std::string &cname)
	{
		for (auto &i : mChildren)
		{
			if (i == cname)
			{
				return;
			}
		}
		mChildren.push_back(cname);
	}

	bool				mSaved{ false };
	std::string			mElementName;		// Name of this element
	KeyValuePairVector	mKeyValues;			// Key value pairs
	std::string			mParentName;		// Parent element
	StringVector		mChildren;			// Any children elements
};

class CreateDOMImpl :public CreateDOM, public FAST_XML::FastXml::Callback
{
public:
	CreateDOMImpl(void)
	{

	}

	virtual ~CreateDOMImpl(void)
	{
		for (auto &i : mElementTypes)
		{
			delete i;
		}
	}

	// return true to continue processing the XML document, false to skip.
	virtual bool processElement(
		const char *elementName,   // name of the element
		uint32_t argc,         // number of attributes pairs
		const char **argv,         // list of attributes.
		const char  *elementData,  // element data, null if none
		uint32_t lineno) final  // line number in the source XML file
	{
		bool isElementType = false;
		if (elementData == nullptr || strlen(elementData) == 0)
		{
			isElementType = true;
		}
		mStackElements[mStackIndex] = std::string(elementName);
		mStackIsElement[mStackIndex] = isElementType;

		if (isElementType) // if this is an element/type then..
		{
			mPreviousElementType = mCurrentElementType;
			std::string ename(elementName);
			ElementType *etype = findElementType(ename);
			if ( etype == nullptr )
			{
				mCurrentElementType = new ElementType(ename.c_str());
				mElementTypes.push_back(mCurrentElementType);
				if (mPreviousElementType)
				{
					mCurrentElementType->mParentName = mPreviousElementType->mElementName;
				}
			}
			else
			{
				mCurrentElementType = etype;
			}
			if (mPreviousElementType)
			{
				mPreviousElementType->addChild(ename);
			}
		}
		else
		{
			mCurrentElementType->addKeyValuePair(elementName, elementData);
		}

		if (mStackIndex < MAX_STACK_INDEX)
		{
			mStackIndex++;
		}

		return true;
	}

	// Parses this XML and accumulates all of the unique element and attribute names
	virtual void inspectXml(const char *xmlName) final
	{
		FAST_XML::FastXml *f = FAST_XML::FastXml::create();
		printf("Inspecting XML File: %s\r\n", xmlName);
		f->processXml(xmlName, this);
		f->release();
		printf("\n");
	}

	virtual void release(void) final
	{
		delete this;
	}

	virtual bool processComment(const char *comment)final  // encountered a comment in the XML
	{
		return true;
	}

	// 'element' is the name of the element that is being closed.
	// depth is the recursion depth of this element.
	// Return true to continue processing the XML file.
	// Return false to stop processing the XML file; leaves the read pointer of the stream right after this close tag.
	// The bool 'isError' indicates whether processing was stopped due to an error, or intentionally canceled early.
	virtual bool processClose(const char *element, uint32_t depth, bool &isError, uint32_t lineno) final	  // process the 'close' indicator for a previously encountered element
	{
		if (mStackIndex)
		{
			mStackIndex--;
		}
		if (mStackIsElement[mStackIndex])
		{
			mCurrentElementType = nullptr;
			mPreviousElementType = nullptr;
			if (mStackIndex)
			{
				uint32_t scan = mStackIndex - 1;
				for (;;)
				{
					if (mStackIsElement[scan])
					{
						std::string ename = mStackElements[scan];
						mCurrentElementType = findElementType(ename);
						if (mCurrentElementType )
						{
							break;
						}
					}
					if (scan == 0) break;
					scan--;
				}
				if (mCurrentElementType && scan )
				{
					scan--;
					for (;;)
					{
						if (mStackIsElement[scan])
						{
							std::string ename = mStackElements[scan];
							mPreviousElementType = findElementType(ename);
							if ( mPreviousElementType )
							{
								break;
							}
						}
						if (scan == 0) break;
						scan--;
					}
				}
			}
		}

		return true;
	}

	// Save the DOM as a CSV
	virtual void saveDOM(const char *fname) final
	{
		printf("Saving DOM to '%s'\r\n", fname);
		FILE *fph = fopen(fname, "wb");
		if (fph == nullptr)
		{
			return;
		}
		fprintf(fph, "ObjectName,DataItemName,Type,Inherits,DefaultValue,MinValue,MaxValue,ShortDescription,DetailedDescription\r\n");
		for (auto &i : mElementTypes)
		{
			i->saveDOM(fph,mElementTypes);
		}
		fclose(fph);
	}

	ElementType *findElementType(const std::string &ename)
	{
		ElementType *ret = nullptr;

		for (const auto &i : mElementTypes)
		{
			if (i->mElementName == ename)
			{
				ret = i;
				break;
			}
		}

		return ret;
	}


	uint32_t		mStackIndex{ 0 };
	std::string		mStackElements[MAX_STACK_INDEX];
	bool			mStackIsElement[MAX_STACK_INDEX];
	ElementType		*mCurrentElementType{ nullptr };
	ElementType		*mPreviousElementType{ nullptr };
	ElementTypeVector	mElementTypes;
};

CreateDOM *CreateDOM::create(void)
{
	CreateDOMImpl *in = new CreateDOMImpl;
	return static_cast<CreateDOM *>(in);
}



}
