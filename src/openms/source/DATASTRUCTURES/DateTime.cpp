// --------------------------------------------------------------------------
//                   OpenMS -- Open-Source Mass Spectrometry
// --------------------------------------------------------------------------
// Copyright The OpenMS Team -- Eberhard Karls University Tuebingen,
// ETH Zurich, and Freie Universitaet Berlin 2002-2022.
//
// This software is released under a three-clause BSD license:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of any author or any participating institution
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
// For a full list of authors, refer to the file AUTHORS.
// --------------------------------------------------------------------------
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: Timo Sachsenberg $
// $Authors: Nico Pfeifer $
// --------------------------------------------------------------------------

#include <OpenMS/DATASTRUCTURES/DateTime.h>

#include <OpenMS/DATASTRUCTURES/String.h>
#include <OpenMS/CONCEPT/Exception.h>

#include <QtCore/QDateTime> // very expensive to include!

using namespace std;

namespace OpenMS
{
  DateTime::DateTime() :
    dt_(new QDateTime)
  {
  }

  DateTime::DateTime(const DateTime& date)
    : dt_(new QDateTime(*date.dt_))
  {
  }

  DateTime::DateTime(DateTime&& rhs) noexcept
    : dt_(rhs.dt_.release())
  {
  }


  DateTime& DateTime::operator=(const DateTime& source)
  {
    if (&source == this)
    {
      return *this;
    }

    if (dt_ == nullptr)
    { // *this is in a 'moved-from' state; we need to create a dt_ object first
      dt_ = make_unique<QDateTime>(*source.dt_);
    }
    else
    {
      *dt_ = *source.dt_;
    }

    return *this;
  }

  DateTime& DateTime::operator=(DateTime&& source) & noexcept
  {
    if (&source == this)
    {
      return *this;
    }

    std::swap(dt_, source.dt_);

    return *this;
  }

  DateTime::~DateTime() = default;

  bool DateTime::operator==(const DateTime& rhs) const
  {
    return (*dt_ == *rhs.dt_);
  }

  bool DateTime::operator!=(const DateTime& rhs) const
  {
    return !(*this == rhs);
  }

  bool DateTime::operator<(const DateTime& rhs) const
  {
    return (*dt_ < *rhs.dt_);
  }

  bool DateTime::isValid() const
  {
    return dt_->isValid();
  }

  String DateTime::toString(std::string format) const
  {
    return dt_->toString(QString::fromStdString(format)).toStdString();
  }

  void DateTime::set(const String& date)
  {
    clear();

    if (date.has('.') && !date.has('T'))
    {
      *dt_ = (QDateTime::fromString(date.c_str(), "dd.MM.yyyy hh:mm:ss"));
    }
    else if (date.has('/'))
    {
      *dt_ = (QDateTime::fromString(date.c_str(), "MM/dd/yyyy hh:mm:ss"));
    }
    else if (date.has('-'))
    {
      if (date.has('T'))
      {
        if (date.has('+'))
        {
          // remove timezone part, since Qt cannot handle this, check if we also have a millisecond part
          if (date.has('.'))
          {
            *dt_ = (QDateTime::fromString(date.prefix('+').c_str(), "yyyy-MM-ddThh:mm:ss.zzz"));
          }
          else
          {
            *dt_ = (QDateTime::fromString(date.prefix('+').c_str(), "yyyy-MM-ddThh:mm:ss"));
          }
        }
        else
        {
          *dt_ = (QDateTime::fromString(date.c_str(), "yyyy-MM-ddThh:mm:ss"));
        }
      }
      else if (date.has('Z'))
      {
        *dt_ = (QDateTime::fromString(date.c_str(), "yyyy-MM-ddZ"));
      }
      else if (date.has('+'))
      {
        *dt_ = (QDateTime::fromString(date.c_str(), "yyyy-MM-dd+hh:mm"));
      }
      else
      {
        *dt_ = (QDateTime::fromString(date.c_str(), "yyyy-MM-dd hh:mm:ss"));
      }
    }

    if (!dt_->isValid())
    {
      *dt_ = QDateTime::fromString(date.c_str());  // ddd MMM d YYYY format as found in (old?) protXML files
    }

    if (!dt_->isValid())
    {
      throw Exception::ParseError(__FILE__, __LINE__, OPENMS_PRETTY_FUNCTION, date, "Invalid date time string");
    }
  }

  void DateTime::set(UInt month, UInt day, UInt year, UInt hour, UInt minute, UInt second)
  {
    dt_->setDate(QDate(year, month, day));
    dt_->setTime(QTime(hour, minute, second));

    if (!dt_->isValid())
    {
      String date_time = String(year) + "-" + String(month) + "-" + String(day)
                         + " " + String(hour) + ":" + String(minute) + ":" + String(second);
      throw Exception::ParseError(__FILE__, __LINE__, OPENMS_PRETTY_FUNCTION, date_time, "Invalid date time");
    }
  }

  DateTime DateTime::now()
  {
    DateTime d;
    *d.dt_ = QDateTime::currentDateTime();
    return d;
  }

  String DateTime::get() const
  {
    if (dt_->isValid())
    {
      return dt_->toString("yyyy-MM-dd hh:mm:ss");
    }
    return "0000-00-00 00:00:00";
  }

  void DateTime::get(UInt& month, UInt& day, UInt& year,
                     UInt& hour, UInt& minute, UInt& second) const
  {
    const QDate& temp_date = dt_->date();
    const QTime& temp_time = dt_->time();

    year = temp_date.year();
    month = temp_date.month();
    day = temp_date.day();
    hour = temp_time.hour();
    minute = temp_time.minute();
    second = temp_time.second();
  }

  void DateTime::clear()
  {
    *dt_ = QDateTime();
  }

  void DateTime::setDate(const String& date)
  {
    QDate temp_date;

    if (date.has('-'))
    {
      temp_date = QDate::fromString(date.c_str(), "yyyy-MM-dd");
    }
    else if (date.has('.'))
    {
      temp_date = QDate::fromString(date.c_str(), "dd-MM-yyyy");
    }
    else if (date.has('/'))
    {
      temp_date = QDate::fromString(date.c_str(), "MM/dd/yyyy");
    }
    else
    {
      throw Exception::ParseError(__FILE__, __LINE__, OPENMS_PRETTY_FUNCTION, date, "Could not set date");
    }
    if (!temp_date.isValid())
    {
      throw Exception::ParseError(__FILE__, __LINE__, OPENMS_PRETTY_FUNCTION, date, "Could not set date");
    }

    dt_->setDate(temp_date);
  }

  void DateTime::setTime(const String& time)
  {
    QTime temp_time;

    temp_time = QTime::fromString(time.c_str(), "hh:mm:ss");
    if (!temp_time.isValid())
    {
      throw Exception::ParseError(__FILE__, __LINE__, OPENMS_PRETTY_FUNCTION, time, "Could not set time");
    }

    dt_->setTime(temp_time);
  }

  void DateTime::setDate(UInt month, UInt day, UInt year)
  {
    QDate temp_date;

    if (!temp_date.setDate(year, month, day))
    {
      throw Exception::ParseError(__FILE__, __LINE__, OPENMS_PRETTY_FUNCTION, String(year) + "-" + String(month) + "-" + String(day), "Could not set date");
    }

    dt_->setDate(temp_date);
  }

  void DateTime::setTime(UInt hour, UInt minute, UInt second)
  {
    QTime temp_time;

    if (!temp_time.setHMS(hour, minute, second))
    {
      throw Exception::ParseError(__FILE__, __LINE__, OPENMS_PRETTY_FUNCTION, String(hour) + ":" + String(minute) + ":" + String(second), "Could not set time");
    }
    dt_->setTime(temp_time);
  }

  void DateTime::getDate(UInt& month, UInt& day, UInt& year) const
  {
    const QDate& temp_date = dt_->date();

    month = temp_date.month();
    day = temp_date.day();
    year = temp_date.year();
  }

  String DateTime::getDate() const
  {
    if (dt_->isValid())
    {
      return dt_->date().toString("yyyy-MM-dd");
    }
    return "0000-00-00";
  }

  void DateTime::getTime(UInt& hour, UInt& minute, UInt& second) const
  {
    const QTime& temp_time =dt_->time();

    hour = temp_time.hour();
    minute = temp_time.minute();
    second = temp_time.second();
  }

  String DateTime::getTime() const
  {
    if (dt_->isValid())
    {
      return dt_->time().toString("hh:mm:ss");
    }
    return "00:00:00";
  }
  
  DateTime& DateTime::addSecs(int s)
  {
    *dt_ = dt_->addSecs(s);
    return *this;
  }

  bool DateTime::isNull() const
  {
    return dt_->isNull();
  }

  // static
  DateTime DateTime::fromString(const std::string& date, std::string format)
  {
    DateTime d;
    *d.dt_ = QDateTime::fromString(QString::fromStdString(date), QString::fromStdString(format));
    return d;
  }

} // namespace OpenMS
