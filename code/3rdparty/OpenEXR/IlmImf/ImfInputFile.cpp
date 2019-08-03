///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2004, Industrial Light & Magic, a division of Lucas
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
#pragma warning(disable: 4290)
#pragma warning(disable: 4541)
#pragma warning(disable: 4244)
#pragma warning(disable: 4018)
#include "..\compata.h"

//-----------------------------------------------------------------------------
//
//	class InputFile
//
//-----------------------------------------------------------------------------

#include <ImfInputFile.h>
#include <ImfScanLineInputFile.h>
#include <ImfTiledInputFile.h>
#include <ImfChannelList.h>
#include <ImfMisc.h>
#include <ImfStdIO.h>
#include <Iex.h>
#include <ImfVersion.h>
#include <ImathFun.h>
#include <half.h>
#include <fstream>
#include <algorithm>

namespace Imf {


using Imath::Box2i;
using Imath::divp;
using Imath::modp;


//
// Struct InputFile::Data stores things that will be
// needed between calls to readPixels
//

struct InputFile::Data
{
    Header              header;
    int                 version;
    IStream *		is;
    bool		deleteStream;

    TiledInputFile *	tFile;
    ScanLineInputFile *	sFile;

    LineOrder		lineOrder;      // the file's lineorder
    int			minY;           // data window's min y coord
    int			maxY;           // data window's max x coord
    
    FrameBuffer		tFileBuffer;
    FrameBuffer *	cachedBuffer;
    
    int			cachedTileY;
    int			offset;

     Data (bool del);
    ~Data ();

    void		deleteCachedBuffer();
};


InputFile::Data::Data (bool del):
    is (0),
    deleteStream (del),
    tFile (0),
    sFile (0),
    cachedBuffer (0),
    cachedTileY (-1)
{
    // empty
}


InputFile::Data::~Data ()
{
    if (deleteStream)
	delete is;

    delete tFile;
    delete sFile;
    deleteCachedBuffer();
}


void	
InputFile::Data::deleteCachedBuffer()
{
    //
    // Delete the cached frame buffer, and all memory
    // allocated for the slices in the cached frameBuffer.
    //

    if (cachedBuffer)
    {
	for (FrameBuffer::Iterator k = cachedBuffer->begin();
	     k != cachedBuffer->end();
	     ++k)
	{
	    Slice &s = k.slice();

	    switch (s.type)
	    {
	      case UINT:

		delete [] (((unsigned int *)s.base) + offset);
		break;

	      case HALF:

		delete [] ((half *)s.base + offset);
		break;

	      case FLOAT:

		delete [] (((float *)s.base) + offset);
		break;
	    }                
	}

	//
	// delete the cached frame buffer
	//

	delete cachedBuffer;        
	cachedBuffer = 0;
    }
}


namespace {

void
bufferedReadPixels (InputFile::Data* ifd, int scanLine1, int scanLine2)
{
    //
    // bufferedReadPixels reads each row of tiles that intersect the
    // scan-line range (scanLine1 to scanLine2). The previous row of
    // tiles is cached in order to prevent redundent tile reads when
    // accessing scanlines sequentially.
    //

    int minY = min (scanLine1, scanLine2);
    int maxY = max (scanLine1, scanLine2);

    if (minY < ifd->minY || maxY >  ifd->maxY)
    {
        throw Iex::ArgExc ("Tried to read scan line outside "
			   "the image file's data window.");
    }

    //
    // The minimum and maximum y tile coordinates that intersect this
    // scanline range
    //

    int minDy = (minY - ifd->minY) / ifd->tFile->tileYSize();
    int maxDy = (maxY - ifd->minY) / ifd->tFile->tileYSize();

    //
    // Figure out which one is first in the file so we can read without seeking
    //

    int yStart, yEnd, yStep;

    if (ifd->lineOrder == DECREASING_Y)
    {
        yStart = maxDy;
        yEnd = minDy - 1;
        yStep = -1;
    }
    else
    {
        yStart = minDy;
        yEnd = maxDy + 1;
        yStep = 1;
    }

    //
    // the number of pixels in a row of tiles
    //

    int tileRowSize = ifd->tFile->levelWidth(0) * ifd->tFile->tileYSize();

    //
    // Read the tiles into our temporary framebuffer and copy them into
    // the user's buffer
    //

    for (int j = yStart; j != yEnd; j += yStep)
    {
        Box2i tileRange = ifd->tFile->dataWindowForTile (0, j, 0);

        int minYThisRow = max (minY, tileRange.min.y);
        int maxYThisRow = min (maxY, tileRange.max.y);

        if (j != ifd->cachedTileY)
        {
            //
            // We don't have any valid buffered info, so we need to read in
            // from the file.
            //

            //
            // If cachedBuffer is not 0 then that means we already have
            // some data buffered. We must delete the memory allocated for
            // the previous cachedBuffer before continuing.
            //

            if (ifd->cachedBuffer)
		ifd->deleteCachedBuffer();

            //
            // Then, allocate a framebuffer big enough to store all tiles in
            // this row of tiles and save it as cachedBuffer.
            //

            ifd->cachedBuffer = new FrameBuffer();
            ifd->cachedTileY = j;

            ifd->offset = tileRange.min.y * ifd->tFile->levelWidth(0) +
                          tileRange.min.x;

            for (FrameBuffer::ConstIterator k = ifd->tFileBuffer.begin();
                 k != ifd->tFileBuffer.end();
		 ++k)
            {
                Slice s = k.slice();

                switch (s.type)
                {
		  case UINT:

		    ifd->cachedBuffer->insert
			(k.name(),
			 Slice (UINT,
				(char *)(new unsigned int[tileRowSize] -	
				     ifd->offset),
				 sizeof (unsigned int),
				 sizeof (unsigned int) *
				     ifd->tFile->levelWidth(0),
				 1, 1,
				 s.fillValue));
		    break;

		  case HALF:

		    ifd->cachedBuffer->insert
			(k.name(),
			 Slice (HALF,
				(char *)(new half[tileRowSize] -
				     ifd->offset),
				 sizeof (half),
				 sizeof (half) *
				     ifd->tFile->levelWidth(0),
				 1, 1,
				 s.fillValue));
		    break;

		  case FLOAT:

		    ifd->cachedBuffer->insert
			(k.name(),
			 Slice (FLOAT,
				(char *)(new float[tileRowSize] -
				     ifd->offset),
				 sizeof(float),
				 sizeof(float) *
				     ifd->tFile->levelWidth(0),
				 1, 1,
				 s.fillValue));
		    break;

		  default:

		    throw Iex::ArgExc ("Unknown pixel data type.");
                }
            }

            ifd->tFile->setFrameBuffer (*ifd->cachedBuffer);

            //
            // Read in the whole row of tiles into cachedBuffer.
            //

            for (int i = 0; i < ifd->tFile->numXTiles (0); ++i)
                ifd->tFile->readTile (i, j, 0);
        }

        //
        // Copy the data from our cached framebuffer into the user's
        // framebuffer.
        //

        Box2i levelRange = ifd->tFile->dataWindowForLevel(0);

        for (FrameBuffer::ConstIterator k = ifd->cachedBuffer->begin();
             k != ifd->cachedBuffer->end();
             ++k)
        {
            Slice fromSlice = k.slice();		// slice to write from
            Slice toSlice = ifd->tFileBuffer[k.name()];	// slice to write to

            char *fromPtr, *toPtr;
            int size = pixelTypeSize (toSlice.type);

	    int xStart = levelRange.min.x;
	    int yStart = minYThisRow;

	    while (modp (xStart, toSlice.xSampling) != 0)
		++xStart;

	    while (modp (yStart, toSlice.ySampling) != 0)
		++yStart;

            for (int y = yStart;
		 y <= maxYThisRow;
		 y += toSlice.ySampling)
            {
		//
                // Set the pointers to the start of the y scanline in
                // this row of tiles
		//

                fromPtr = fromSlice.base +
                          y * fromSlice.yStride +
                          xStart * fromSlice.xStride;

                toPtr = toSlice.base +
                        divp (y, toSlice.ySampling) * toSlice.yStride +
                        divp (xStart, toSlice.xSampling) * toSlice.xStride;

		//
                // Copy all pixels for the scanline in this row of tiles
		//

                for (int x = xStart;
		     x <= levelRange.max.x;
		     x += toSlice.xSampling)
                {
		    for (size_t i = 0; i < size; ++i)
			toPtr[i] = fromPtr[i];

		    fromPtr += fromSlice.xStride * toSlice.xSampling;
		    toPtr += toSlice.xStride;
                }
            }
        }
    }
}

} // namespace



InputFile::InputFile (const char fileName[]):
    _data (new Data (true))
{
//    try
//    {
	_data->is = new StdIFStream (fileName);
	initialize();
//   }
//    catch (Iex::BaseExc &e)
//   {
//	delete _data;

//        REPLACE_EXC (e, "Cannot read image file "
//			"\"" << fileName << "\". " << e);
//        throw;
//    }
}


InputFile::InputFile (IStream &is):
    _data (new Data (false))
{
 //   try
 //   {
	_data->is = &is;
	initialize();
 //   }
 //   catch (Iex::BaseExc &e)
 //   {
//	delete _data;

 //       REPLACE_EXC (e, "Cannot read image file "
//			"\"" << is.fileName() << "\". " << e);
 //       throw;
 //   }
}


void
InputFile::initialize ()
{
    _data->header.readFrom (*_data->is, _data->version);
    _data->header.sanityCheck (isTiled (_data->version));

    if (isTiled (_data->version))
    {
	_data->lineOrder = _data->header.lineOrder();

	//
	// Save the dataWindow information
	//

	const Box2i &dataWindow = _data->header.dataWindow();
	_data->minY = dataWindow.min.y;
	_data->maxY = dataWindow.max.y;
    
	_data->tFile = new TiledInputFile (_data->header,
					   _data->is,
					   _data->version);
    }
    else
    {
	_data->sFile = new ScanLineInputFile (_data->header,
					      _data->is);
    }
}


InputFile::~InputFile ()
{
    delete _data;
}


const char *
InputFile::fileName () const
{
    return _data->is->fileName();
}


const Header &
InputFile::header () const
{
    return _data->header;
}


int
InputFile::version () const
{
    return _data->version;
}


void
InputFile::setFrameBuffer (const FrameBuffer &frameBuffer)
{
    if (isTiled (_data->version))
    {
	//
        // Invalidate the cached buffer if the new frame buffer
	// has a different set of channels than the old frame
	// buffer, or if the type of a channel has changed.
	//

	const FrameBuffer &oldFrameBuffer = _data->tFileBuffer;

	FrameBuffer::ConstIterator i = oldFrameBuffer.begin();
	FrameBuffer::ConstIterator j = frameBuffer.begin();

	while (i != oldFrameBuffer.end())
	{
	    if (strcmp (i.name(), j.name()) || i.slice().type != j.slice().type)
		break;

	    ++i;
	    ++j;
	}

	if (i != oldFrameBuffer.end() || j != frameBuffer.end())
	    _data->cachedTileY = -1;

	_data->tFileBuffer = frameBuffer;
    }
    else
    {
        _data->sFile->setFrameBuffer (frameBuffer);
    }
}


const FrameBuffer &
InputFile::frameBuffer () const
{
    if (isTiled (_data->version))
	return _data->tFileBuffer;
    else
	return _data->sFile->frameBuffer();
}


bool
InputFile::isComplete () const
{
    if (isTiled (_data->version))
	return _data->tFile->isComplete();
    else
	return _data->sFile->isComplete();
}


void
InputFile::readPixels (int scanLine1, int scanLine2)
{
    if (isTiled (_data->version))
        bufferedReadPixels (_data, scanLine1, scanLine2);
    else
        _data->sFile->readPixels (scanLine1, scanLine2);
}


void
InputFile::readPixels (int scanLine)
{
    readPixels (scanLine, scanLine);
}


void
InputFile::rawPixelData (int firstScanLine,
			 const char *&pixelData,
			 int &pixelDataSize)
{
//    try
//    {
	if (isTiled (_data->version))
	{
	    throw Iex::ArgExc ("Tried to read a raw scanline "
			       "from a tiled image.");
	}
        
        _data->sFile->rawPixelData (firstScanLine, pixelData, pixelDataSize);
//    }
//    catch (Iex::BaseExc &e)
//    {
//	REPLACE_EXC (e, "Error reading pixel data from image "
//		        "file \"" << fileName() << "\". " << e);
//	throw;
//    }
}


void
InputFile::rawTileData (int &dx, int &dy,
			int &lx, int &ly,
			const char *&pixelData,
			int &pixelDataSize)
{
//    try
//    {
	if (!isTiled (_data->version))
	{
	    throw Iex::ArgExc ("Tried to read a raw tile "
			       "from a scanline-based image.");
	}
        
        _data->tFile->rawTileData (dx, dy, lx, ly, pixelData, pixelDataSize);
//    }
//    catch (Iex::BaseExc &e)
//    {
//	REPLACE_EXC (e, "Error reading tile data from image "
//		        "file \"" << fileName() << "\". " << e);
//	throw;
//    }
}


TiledInputFile*
InputFile::tFile()
{
    if (!isTiled (_data->version))
    {
	throw Iex::ArgExc ("Cannot get a TiledInputFile pointer "
			   "from an InputFile that is not tiled.");
    }

    return _data->tFile;
}


} // namespace Imf
