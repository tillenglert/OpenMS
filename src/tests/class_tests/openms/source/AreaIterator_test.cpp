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
// $Authors: Marc Sturm $
// --------------------------------------------------------------------------

#include <OpenMS/CONCEPT/ClassTest.h>
#include <OpenMS/test_config.h>

///////////////////////////
#include <OpenMS/KERNEL/StandardTypes.h>
#include <OpenMS/KERNEL/AreaIterator.h>
#include <OpenMS/KERNEL/MSExperiment.h>
///////////////////////////

using namespace OpenMS;
using namespace std;

START_TEST(AreaIterator, "$Id$")

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

typedef PeakMap Map;
using AI = Internal::AreaIterator<Map::PeakType, Map::PeakType&, Map::PeakType*, Map::Iterator, Map::SpectrumType::Iterator>;
using AIP = AI::Param;
//typedef Internal::AreaIterator<Map::PeakType, const Map::PeakType&, const Map::PeakType*, Map::ConstIterator, Map::SpectrumType::ConstIterator> CAI;

AI* ptr1 = nullptr, *ptr2 = nullptr;
AI* nullPointer = nullptr;

Map exp;
exp.resize(5);
exp[0].resize(2);
exp[0].setRT(2.0);
exp[0].setDriftTime(1);
exp[0].setMSLevel(1);
exp[0][0].setMZ(502);
exp[0][1].setMZ(510);

exp[1].resize(2);
exp[1].setRT(4.0);
exp[1].setDriftTime(1.4);
exp[1].setMSLevel(1);
exp[1][0].setMZ(504);
exp[1][1].setMZ(506);

exp[2].setRT(6.0);
exp[2].setDriftTime(1.6);
exp[2].setMSLevel(1);

exp[3].resize(2);
exp[3].setRT(8.0);
exp[3].setDriftTime(1.8);
exp[3].setMSLevel(1);
exp[3][0].setMZ(504.1);
exp[3][1].setMZ(506.1);

exp[4].resize(2);
exp[4].setRT(10.0);
exp[4].setDriftTime(1.99);
exp[4].setMSLevel(1);
exp[4][0].setMZ(502.1);
exp[4][1].setMZ(510.1);

START_SECTION((AreaIterator()))
	ptr1 = new AI();
  TEST_NOT_EQUAL(ptr1,nullPointer)
END_SECTION

START_SECTION((AreaIterator(SpectrumIteratorType first, SpectrumIteratorType begin, SpectrumIteratorType end, CoordinateType low_mz, CoordinateType high_mz)))
	ptr2 = new AI(AIP(exp.begin(),exp.RTBegin(0), exp.RTEnd(0)).lowMZ(0).highMZ(0));
  TEST_NOT_EQUAL(ptr2,nullPointer)
END_SECTION

START_SECTION((~AreaIterator()))
	delete ptr1;
	delete ptr2;
END_SECTION

START_SECTION((bool operator==(const AreaIterator &rhs) const))
	AI a1, a2;
	TEST_EQUAL(a1==a1, true)
	TEST_EQUAL(a2==a2, true)
	TEST_EQUAL(a1==a2, true)
	
	AI a3(AIP(exp.begin(),exp.RTBegin(0), exp.RTEnd(10)).lowMZ(500).highMZ(600));
	TEST_EQUAL(a3==a3, true)
	TEST_EQUAL(a1==a3, false)
	TEST_EQUAL(a2==a3, false)
END_SECTION

START_SECTION((bool operator!=(const AreaIterator &rhs) const))
	AI a1, a2;
	TEST_EQUAL(a1!=a1, false)
	TEST_EQUAL(a2!=a2, false)
	TEST_EQUAL(a1!=a2, false)
	
	AI a3(AIP(exp.begin(), exp.RTBegin(0), exp.RTEnd(10)).lowMZ(500).highMZ(600));
	TEST_EQUAL(a3!=a3, false)
	TEST_EQUAL(a1!=a3, true)
	TEST_EQUAL(a2!=a3, true)
END_SECTION

START_SECTION((AreaIterator(const AreaIterator &rhs)))
	AI a1;
	AI a2(AIP(exp.begin(),exp.RTBegin(0), exp.RTEnd(10)).lowMZ(500).highMZ(600));

	AI a3(a2);
	TEST_EQUAL(a3==a1, false)
	TEST_EQUAL(a3==a2, true)
	
  // copy-constructor on end-Iterator is undefined, so the following
  // operation is invalid
  // AI a4(a1);
  // TEST_EQUAL(a4==a1, true)
  // TEST_EQUAL(a4==a2, false)
END_SECTION

START_SECTION((AreaIterator& operator=(const AreaIterator &rhs)))
	AI a1, a2;
	AI a3(AIP(exp.begin(),exp.RTBegin(0), exp.RTEnd(10)).lowMZ(500).highMZ(600));

	a2 = a3;
	TEST_EQUAL(a2==a3, true)
	TEST_EQUAL(a2==a1, false)
	
	a2 = a1;
	TEST_EQUAL(a2==a1, true)
	TEST_EQUAL(a2==a3, false)
END_SECTION

START_SECTION((reference operator *() const))
  AI it = AI(AIP(exp.begin(), exp.RTBegin(0), exp.RTEnd(7)).lowMZ(505).highMZ(520));
  TEST_REAL_SIMILAR((*it).getMZ(),510.0);
END_SECTION

START_SECTION((pointer operator->() const))
	AI it = AI(AIP(exp.begin(), exp.RTBegin(0), exp.RTEnd(7)).lowMZ(505).highMZ(520));
	TEST_REAL_SIMILAR(it->getMZ(),510.0);
END_SECTION

START_SECTION((AreaIterator& operator++()))
	AI it = AI(AIP(exp.begin(), exp.RTBegin(0), exp.RTEnd(7)).lowMZ(505).highMZ(520));
	Map::PeakType* peak = &(*(it++));
	TEST_REAL_SIMILAR(peak->getMZ(),510.0);
	peak = &(*(it++));
	TEST_REAL_SIMILAR(peak->getMZ(),506.0);
	TEST_EQUAL(it==exp.areaEnd(),true);
END_SECTION

START_SECTION((AreaIterator operator++(int)))
	AI it = AI(AIP(exp.begin(), exp.RTBegin(0), exp.RTEnd(7)).lowMZ(505).highMZ(520));
	TEST_REAL_SIMILAR(it->getMZ(),510.0);
	++it;
	TEST_REAL_SIMILAR(it->getMZ(),506.0);
	++it;
	TEST_EQUAL(it==exp.areaEnd(),true);
END_SECTION

START_SECTION((CoordinateType getRT() const))
	AI it = AI(AIP(exp.begin(), exp.RTBegin(3), exp.RTEnd(9)).lowMZ(503).highMZ(509));
	TEST_REAL_SIMILAR(it->getMZ(),504.0);
	TEST_REAL_SIMILAR(it.getRT(),4.0);
	++it;
	TEST_REAL_SIMILAR(it->getMZ(),506.0);
	TEST_REAL_SIMILAR(it.getRT(),4.0);
	++it;
	TEST_REAL_SIMILAR(it->getMZ(),504.1);
	TEST_REAL_SIMILAR(it.getRT(),8.0);
	++it;
	TEST_REAL_SIMILAR(it->getMZ(),506.1);
	TEST_REAL_SIMILAR(it.getRT(),8.0);
	++it;
	TEST_EQUAL(it==exp.areaEnd(),true);
END_SECTION

START_SECTION((CoordinateType getDriftTime() const))
  AI it = AI(AIP(exp.begin(), exp.RTBegin(3), exp.RTEnd(9)).lowMZ(503).highMZ(509).lowIM(1).highIM(1.5));
  TEST_REAL_SIMILAR(it->getMZ(), 504.0);
  TEST_REAL_SIMILAR(it.getRT(), 4.0);
  TEST_REAL_SIMILAR(it.getDriftTime(), 1.4);
  ++it;
  TEST_REAL_SIMILAR(it->getMZ(), 506.0);
  TEST_REAL_SIMILAR(it.getRT(), 4.0);
  TEST_REAL_SIMILAR(it.getDriftTime(), 1.4);
  ++it;
  ++it;
  TEST_EQUAL(it == exp.areaEnd(), true);
END_SECTION

START_SECTION([EXTRA] Overall test)
	// whole area
  auto test_all = [&](auto it) {
    TEST_REAL_SIMILAR(it->getMZ(), 502.0)
    TEST_REAL_SIMILAR(it.getRT(), 2.0)
    ++it;
    TEST_REAL_SIMILAR(it->getMZ(), 510.0)
    TEST_REAL_SIMILAR(it.getRT(), 2.0)
    ++it;
    TEST_REAL_SIMILAR(it->getMZ(), 504.0)
    TEST_REAL_SIMILAR(it.getRT(), 4.0)
    ++it;
    TEST_REAL_SIMILAR(it->getMZ(), 506.0)
    TEST_REAL_SIMILAR(it.getRT(), 4.0)
    ++it;
    TEST_REAL_SIMILAR(it->getMZ(), 504.1)
    TEST_REAL_SIMILAR(it.getRT(), 8.0)
    ++it;
    TEST_REAL_SIMILAR(it->getMZ(), 506.1)
    TEST_REAL_SIMILAR(it.getRT(), 8.0)
    ++it;
    TEST_REAL_SIMILAR(it->getMZ(), 502.1)
    TEST_REAL_SIMILAR(it.getRT(), 10.0)
    ++it;
    TEST_REAL_SIMILAR(it->getMZ(), 510.1)
    TEST_REAL_SIMILAR(it.getRT(), 10.0)
    ++it;
    TEST_EQUAL(it == exp.areaEnd(), true);
  };
  // restrict dimensions (from -inf,+inf), but include the whole range
  test_all(AI(AIP(exp.begin(), exp.RTBegin(0), exp.RTEnd(15)).lowMZ(500).highMZ(520)));
  test_all(AI(AIP(exp.begin(), exp.RTBegin(0), exp.RTEnd(15))));
  test_all(AI(AIP(exp.begin(), exp.RTBegin(0), exp.RTEnd(15)).lowIM(0).highIM(2)));
  test_all(AI(AIP(exp.begin(), exp.RTBegin(0), exp.RTEnd(15)).lowIM(0).highIM(2).lowMZ(500).highMZ(520)));
  test_all(AI(AIP(exp.begin(), exp.RTBegin(0), exp.RTEnd(15)).lowIM(0).highIM(2).lowMZ(500).highMZ(520).msLevel(1)));
	
  AI it = AI(AIP(exp.begin(), exp.RTBegin(0), exp.RTEnd(15)).lowMZ(500).highMZ(520));
  // center peaks
  it = AI(AIP(exp.begin(), exp.RTBegin(3), exp.RTEnd(9)).lowMZ(503).highMZ(509));
	TEST_REAL_SIMILAR(it->getMZ(),504.0);
	TEST_REAL_SIMILAR(it.getRT(),4.0);	
	++it;
	TEST_REAL_SIMILAR(it->getMZ(),506.0);
	TEST_REAL_SIMILAR(it.getRT(),4.0);	
	++it;
	TEST_REAL_SIMILAR(it->getMZ(),504.1);
	TEST_REAL_SIMILAR(it.getRT(),8.0);	
	++it;
	TEST_REAL_SIMILAR(it->getMZ(),506.1);
	TEST_REAL_SIMILAR(it.getRT(),8.0);	
	++it;
	TEST_EQUAL(it==exp.areaEnd(),true);
	
	//upper left area
  it = AI(AIP(exp.begin(), exp.RTBegin(0), exp.RTEnd(7)).lowMZ(505).highMZ(520));
	TEST_REAL_SIMILAR(it->getMZ(),510.0);
	TEST_REAL_SIMILAR(it.getRT(),2.0);	
	++it;
	TEST_REAL_SIMILAR(it->getMZ(),506.0);
	TEST_REAL_SIMILAR(it.getRT(),4.0);	
	++it;
	TEST_EQUAL(it==exp.areaEnd(),true);
	
	//upper right area
  it = AI(AIP(exp.begin(), exp.RTBegin(5), exp.RTEnd(11)).lowMZ(505).highMZ(520));
	TEST_REAL_SIMILAR(it->getMZ(),506.1);
	TEST_REAL_SIMILAR(it.getRT(),8.0);	
	++it;
	TEST_REAL_SIMILAR(it->getMZ(),510.1);
	TEST_REAL_SIMILAR(it.getRT(),10.0);	
	++it;
	TEST_EQUAL(it==exp.areaEnd(),true);
	
	//lower right
	it = AI(AIP(exp.begin(), exp.RTBegin(5), exp.RTEnd(11)).lowMZ(500).highMZ(505));
	TEST_REAL_SIMILAR(it->getMZ(),504.1);
	TEST_REAL_SIMILAR(it.getRT(),8.0);	
	++it;
	TEST_REAL_SIMILAR(it->getMZ(),502.1);
	TEST_REAL_SIMILAR(it.getRT(),10.0);	
	++it;
	TEST_EQUAL(it==exp.areaEnd(),true);

	// lower left
  it = AI(AIP(exp.begin(), exp.RTBegin(0), exp.RTEnd(7)).lowMZ(500).highMZ(505));
	TEST_REAL_SIMILAR(it->getMZ(),502.0)
	TEST_REAL_SIMILAR(it.getRT(),2.0)	
	++it;
	TEST_REAL_SIMILAR(it->getMZ(),504.0)
	TEST_REAL_SIMILAR(it.getRT(),4.0)	
	++it;
	TEST_EQUAL(it==exp.areaEnd(),true)

	// Test with empty RT range
  it = AI(AIP(exp.begin(), exp.RTBegin(5), exp.RTEnd(5.5)).lowMZ(500).highMZ(520));
	TEST_EQUAL(it==exp.areaEnd(),true)

	// Test with empty MZ range
  it = AI(AIP(exp.begin(), exp.RTBegin(0), exp.RTEnd(15)).lowMZ(505).highMZ(505.5));
	TEST_EQUAL(it==exp.areaEnd(),true)

	// Test with empty RT + MZ range
  it = AI(AIP(exp.begin(), exp.RTBegin(5), exp.RTEnd(5.5)).lowMZ(505).highMZ(505.5));
	TEST_EQUAL(it==exp.areaEnd(),true)

	// Test with empty IM range
  it = AI(AIP(exp.begin(), exp.RTBegin(0), exp.RTEnd(15)).lowIM(0).highIM(0.9));
  TEST_EQUAL(it == exp.areaEnd(), true)

	// Test with empty MS level
  it = AI(AIP(exp.begin(), exp.RTBegin(0), exp.RTEnd(15)).msLevel(3));
  TEST_EQUAL(it == exp.areaEnd(), true)

  // Test with empty (no MS level 1) experiment
  PeakMap exp2(exp);
  exp2[0].setMSLevel(2);
  exp2[1].setMSLevel(2);
  exp2[2].setMSLevel(2);
  exp2[3].setMSLevel(2);
  exp2[4].setMSLevel(2);
  it = AI(AIP(exp2.begin(), exp2.RTBegin(0), exp2.RTEnd(15)).lowMZ(500).highMZ(520).msLevel(1));
  TEST_TRUE(it==exp2.areaEnd())
  // however: automatic MS level should work (determined from exp2.RTBegin(0))
  it = AI(AIP(exp2.begin(), exp2.RTBegin(0), exp2.RTEnd(15)).lowMZ(500).highMZ(520));
  TEST_FALSE(it == exp2.areaEnd())
  END_SECTION

START_SECTION((PeakIndex getPeakIndex() const))
  PeakIndex i;
AI it = AI(AIP(exp.begin(), exp.begin(), exp.end()).lowMZ(0).highMZ(1000));
	i = it.getPeakIndex();
	TEST_EQUAL(i.peak,0)
	TEST_EQUAL(i.spectrum,0)	
	++it;
	i = it.getPeakIndex();
	TEST_EQUAL(i.peak,1)
	TEST_EQUAL(i.spectrum,0)	
	++it;
	i = it.getPeakIndex();
	TEST_EQUAL(i.peak,0)
	TEST_EQUAL(i.spectrum,1)	
	++it;
	i = it.getPeakIndex();
	TEST_EQUAL(i.peak,1)
	TEST_EQUAL(i.spectrum,1)	
	++it;
	i = it.getPeakIndex();
	TEST_EQUAL(i.peak,0)
	TEST_EQUAL(i.spectrum,3)	
	++it;
	i = it.getPeakIndex();
	TEST_EQUAL(i.peak,1)
	TEST_EQUAL(i.spectrum,3)	
	++it;
	i = it.getPeakIndex();
	TEST_EQUAL(i.peak,0)
	TEST_EQUAL(i.spectrum,4)	
	++it;
	i = it.getPeakIndex();
	TEST_EQUAL(i.peak,1)
	TEST_EQUAL(i.spectrum,4)	
	++it;
	i = it.getPeakIndex();
  TEST_EQUAL(i.isValid(),false) 	
END_SECTION
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
END_TEST
