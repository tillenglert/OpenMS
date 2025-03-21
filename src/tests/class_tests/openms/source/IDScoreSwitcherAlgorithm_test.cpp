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
// $Maintainer: Timo Sachsenberg$
// $Authors: Immanuel Luhn$
// --------------------------------------------------------------------------

#include <OpenMS/CONCEPT/ClassTest.h>
#include <OpenMS/test_config.h>

///////////////////////////
#include <OpenMS/ANALYSIS/ID/IDScoreSwitcherAlgorithm.h>
#include <OpenMS/METADATA/PeptideIdentification.h>
#include <OpenMS/METADATA/ProteinIdentification.h>
#include <OpenMS/FORMAT/IdXMLFile.h>

#include <vector>
///////////////////////////

using namespace OpenMS;
using namespace std;

START_TEST(IDRipper, "$Id$")

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////


///load input data
std::vector< ProteinIdentification > protein_identifications;
std::vector< PeptideIdentification > identifications;
String document_id;
IdXMLFile().load(OPENMS_GET_TEST_DATA_PATH("IDScoreSwitcherAlgorithm_test_input.idXML"), protein_identifications, identifications, document_id);
PeptideIdentification identification = identifications[0];
ProteinIdentification protein_identification = protein_identifications[0];

IDScoreSwitcherAlgorithm* ptr = nullptr;
IDScoreSwitcherAlgorithm* null_ptr = nullptr;
START_SECTION(IDScoreSwitcherAlgorithm())
{
  ptr = new IDScoreSwitcherAlgorithm();
  TEST_NOT_EQUAL(ptr, null_ptr)
}
END_SECTION

START_SECTION(~IDScoreSwitcherAlgorithm())
{
  delete ptr;
}
END_SECTION

START_SECTION(switchToGeneralScoreType)
{
  IDScoreSwitcherAlgorithm switcher{};
  Size c(0);
  switcher.switchToGeneralScoreType(identifications, IDScoreSwitcherAlgorithm::ScoreType::PEP, c);
  TEST_EQUAL(identifications[0].getScoreType(), "Posterior Error Probability");
}
END_SECTION


/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
END_TEST
