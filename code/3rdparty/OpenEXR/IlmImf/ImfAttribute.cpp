///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002, Industrial Light & Magic, a division of Lucas
// Digital Ltd. LLC
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// *       Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// *       Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission. 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////



//-----------------------------------------------------------------------------
//
//	class Attribute
//
//-----------------------------------------------------------------------------

#include <ImfAttribute.h>
#include <Iex.h>
#include <string.h>
#include <map>


namespace Imf {


Attribute::Attribute () {}


Attribute::~Attribute () {}


namespace {

struct NameCompare: std::binary_function <const char *, const char *, bool>
{
    bool
    operator () (const char *x, const char *y) const
    {
	return strcmp (x, y) < 0;
    }
};


typedef Attribute* (*Constructor)();
typedef std::map <const char *, Constructor, NameCompare> TypeMap;


TypeMap &
typeMap ()
{
    static TypeMap *typeMap = 0;

    if (typeMap == 0)
	typeMap = new TypeMap;

    return *typeMap;
}


} // namespace


bool		
Attribute::knownType (const char typeName[])
{
    return typeMap().find (typeName) != typeMap().end();
}


void	
Attribute::registerAttributeType (const char typeName[],
			          Attribute *(*newAttribute)())
{
    if (typeMap().find (typeName) != typeMap().end())
	THROW (Iex::ArgExc, "Cannot register image file attribute "
			    "type \"" << typeName << "\". "
			    "The type has already been registered.");

    typeMap().insert (TypeMap::value_type (typeName, newAttribute));
}


void
Attribute::unRegisterAttributeType (const char typeName[])
{
    typeMap().erase (typeName);
}


Attribute *
Attribute::newAttribute (const char typeName[])
{
    TypeMap::const_iterator i = typeMap().find (typeName);

    if (i == typeMap().end())
	THROW (Iex::ArgExc, "Cannot create image file attribute of "
			    "unknown type \"" << typeName << "\".");

    return (i->second)();
}


} // namespace Imf