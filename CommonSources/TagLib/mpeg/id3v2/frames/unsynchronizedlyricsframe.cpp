/***************************************************************************
    copyright            : (C) 2002 - 2008 by Scott Wheeler
    email                : wheeler@kde.org
    copyright            : (C) 2006 by Urs Fleisch
    email                : ufleisch@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *   This library is free software; you can redistribute it and/or modify  *
 *   it  under the terms of the GNU Lesser General Public License version  *
 *   2.1 as published by the Free Software Foundation.                     *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful, but   *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
 *   USA                                                                   *
 *                                                                         *
 *   Alternatively, this file is available under the Mozilla Public        *
 *   License Version 1.1.  You may obtain a copy of the License at         *
 *   http://www.mozilla.org/MPL/                                           *
 ***************************************************************************/

#include "unsynchronizedlyricsframe.h"
#include <tbytevectorlist.h>
#include <tdebug.h>

using namespace TagLib;
using namespace ID3v2;

class UnsynchronizedLyricsFrame::UnsynchronizedLyricsFramePrivate
{
public:
  UnsynchronizedLyricsFramePrivate() : textEncoding(String::Latin1) {}
  String::Type textEncoding;
  ByteVector language;
  String description;
  String text;
};

////////////////////////////////////////////////////////////////////////////////
// public members
////////////////////////////////////////////////////////////////////////////////

UnsynchronizedLyricsFrame::UnsynchronizedLyricsFrame(String::Type encoding) :
  Frame("USLT")
{
  d = new UnsynchronizedLyricsFramePrivate;
  d->textEncoding = encoding;
}

UnsynchronizedLyricsFrame::UnsynchronizedLyricsFrame(const ByteVector &data) :
  Frame(data)
{
  d = new UnsynchronizedLyricsFramePrivate;
  setData(data);
}

UnsynchronizedLyricsFrame::~UnsynchronizedLyricsFrame()
{
  delete d;
}

String UnsynchronizedLyricsFrame::toString() const
{
  return d->text;
}

ByteVector UnsynchronizedLyricsFrame::language() const
{
  return d->language;
}

String UnsynchronizedLyricsFrame::description() const
{
  return d->description;
}

String UnsynchronizedLyricsFrame::text() const
{
  return d->text;
}

void UnsynchronizedLyricsFrame::setLanguage(const ByteVector &languageEncoding)
{
  d->language = languageEncoding.mid(0, 3);
}

void UnsynchronizedLyricsFrame::setDescription(const String &s)
{
  d->description = s;
}

void UnsynchronizedLyricsFrame::setText(const String &s)
{
  d->text = s;
}


String::Type UnsynchronizedLyricsFrame::textEncoding() const
{
  return d->textEncoding;
}

void UnsynchronizedLyricsFrame::setTextEncoding(String::Type encoding)
{
  d->textEncoding = encoding;
}

////////////////////////////////////////////////////////////////////////////////
// protected members
////////////////////////////////////////////////////////////////////////////////

void UnsynchronizedLyricsFrame::parseFields(const ByteVector &data)
{
  if(data.size() < 5) {
    debug("An unsynchronized lyrics frame must contain at least 5 bytes.");
    return;
  }

  d->textEncoding = String::Type(data[0]);
  d->language = data.mid(1, 3);

  int byteAlign
    = d->textEncoding == String::Latin1 || d->textEncoding == String::UTF8 ? 1 : 2;

  ByteVectorList l =
    ByteVectorList::split(data.mid(4), textDelimiter(d->textEncoding), byteAlign, 2);

  if(l.size() == 2) {
    d->description = String(l.front(), d->textEncoding);
    d->text = String(l.back(), d->textEncoding);
  }
}

ByteVector UnsynchronizedLyricsFrame::renderFields() const
{
  ByteVector v;

  //=== Alex. There is a problem with the Writing of UNICODE strings. That's why we manually set the encoding
  String::Type encoding = String::UTF8;
  v.append(char(encoding));
  v.append(d->language.size() == 3 ? d->language : "XXX");
  v.append(d->description.data(encoding));
  v.append(textDelimiter(encoding));
  v.append(d->text.data(encoding));
  //=== Alex END. There is a problem


  return v;
}

////////////////////////////////////////////////////////////////////////////////
// private members
////////////////////////////////////////////////////////////////////////////////

UnsynchronizedLyricsFrame::UnsynchronizedLyricsFrame(const ByteVector &data, Header *h)
  : Frame(h)
{
  d = new UnsynchronizedLyricsFramePrivate();
  parseFields(fieldData(data));
}
