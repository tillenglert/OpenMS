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
// $Maintainer: $
// $Authors: $
// --------------------------------------------------------------------------

#include <OpenMS/CONCEPT/ClassTest.h>

///////////////////////////
#include <OpenMS/METADATA/PeptideEvidence.h>
///////////////////////////

using namespace OpenMS;
using namespace std;

START_TEST(PeptideEvidence, "$Id$")

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

PeptideEvidence* ptr = nullptr;
PeptideEvidence* null_ptr = nullptr;
START_SECTION(PeptideEvidence())
{
	ptr = new PeptideEvidence();
	TEST_NOT_EQUAL(ptr, null_ptr)
}
END_SECTION

START_SECTION(~PeptideEvidence())
{
	delete ptr;
}
END_SECTION

START_SECTION((PeptideEvidence(const PeptideEvidence &source)))
{
  // TODO
}
END_SECTION

START_SECTION((~PeptideEvidence()))
{
  // TODO
}
END_SECTION

START_SECTION((PeptideEvidence& operator=(const PeptideEvidence &source)))
{
  // TODO
}
END_SECTION

START_SECTION((bool operator==(const PeptideEvidence &rhs) const ))
{
  // TODO
}
END_SECTION

START_SECTION((bool operator!=(const PeptideEvidence &rhs) const ))
{
  // TODO
}
END_SECTION

START_SECTION((const String& getProteinAccession() const ))
{
  // TODO
}
END_SECTION

START_SECTION((void setProteinAccession(const String &s)))
{
  // TODO
}
END_SECTION

START_SECTION((void setStart(const Int a)))
{
  // TODO
}
END_SECTION

START_SECTION((Int getStart() const ))
{
  // TODO
}
END_SECTION

START_SECTION((void setEnd(const Int a)))
{
  // TODO
}
END_SECTION

START_SECTION((Int getEnd() const ))
{
  // TODO
}
END_SECTION

START_SECTION((void setAABefore(const char acid)))
{
  // TODO
}
END_SECTION

START_SECTION((char getAABefore() const ))
{
  // TODO
}
END_SECTION

START_SECTION((void setAAAfter(const char acid)))
{
  // TODO
}
END_SECTION

START_SECTION((char getAAAfter() const ))
{
  // TODO
}
END_SECTION


/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
END_TEST



