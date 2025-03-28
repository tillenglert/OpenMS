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
// $Maintainer: Hendrik Weisser $
// $Authors: Hendrik Weisser $
// --------------------------------------------------------------------------

#pragma once

#include <OpenMS/METADATA/ID/MetaData.h>

namespace OpenMS
{
  namespace IdentificationDataInternal
  {
    /** @brief Information about a score type.
    */
    struct ScoreType: public MetaInfoInterface
    {
      CVTerm cv_term; // @TODO: derive from CVTerm instead?

      bool higher_better;

      ScoreType():
        higher_better(true)
      {
      }

      explicit ScoreType(const CVTerm& cv_term, bool higher_better):
        cv_term(cv_term), higher_better(higher_better)
      {
      }

      explicit ScoreType(const String& name, bool higher_better):
        cv_term(), higher_better(higher_better)
      {
        cv_term.setName(name);
      }

      ScoreType(const ScoreType& other) = default;

      // don't include "higher_better" in the comparison:
      bool operator<(const ScoreType& other) const
      {
        // @TODO: implement/use "CVTerm::operator<"?
        return (std::tie(cv_term.getAccession(), cv_term.getName()) <
                std::tie(other.cv_term.getAccession(),
                         other.cv_term.getName()));
      }

      // don't include "higher_better" in the comparison:
      bool operator==(const ScoreType& other) const
      {
        return cv_term == other.cv_term;
      }

      bool isBetterScore(double first, double second) const
      {
        if (higher_better) return first > second;
        return first < second;
      }
    };

    typedef std::set<ScoreType> ScoreTypes;
    typedef IteratorWrapper<ScoreTypes::iterator> ScoreTypeRef;
  }
}
