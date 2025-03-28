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
// $Maintainer: Hannes Roest $
// $Authors: Hannes Roest $
// --------------------------------------------------------------------------

#pragma once

#include <OpenMS/OPENSWATHALGO/DATAACCESS/ISpectrumAccess.h>

namespace OpenSwath
{
  /**
   * @brief Data structure to hold one SWATH map with information about upper /
   * lower isolation window and whether the map is MS1 or MS2.
   */
  struct SwathMap
  {
    OpenSwath::SpectrumAccessPtr sptr;
    double lower;
    double upper;
    double center;
    double imLower;
    double imUpper;
    bool ms1;

    SwathMap() :
      lower(0.0),
      upper(0.0),
      center(0.0),
      imLower(-1),
      imUpper(-1),
      ms1(false)
    {}

    SwathMap(double mz_start, double mz_end, double mz_center, bool is_ms1)
      : lower(mz_start),
        upper(mz_end),
        center(mz_center),
        ms1(is_ms1)
    {}


    SwathMap(double mz_start, double mz_end, double mz_center, double imLower, double imUpper, bool is_ms1)
      : lower(mz_start),
        upper(mz_end),
        center(mz_center),
	imLower(imLower),
	imUpper(imUpper),
        ms1(is_ms1)
    {}


  };

} //end Namespace OpenSwath

