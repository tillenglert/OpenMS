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

#include <OpenMS/METADATA/ID/ObservationMatch.h>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>

namespace OpenMS
{
  namespace IdentificationDataInternal
  {
    /** @brief: Group of related (co-identified) input matches

      E.g. for cross-linking data or multiplexed spectra.
    */
    struct ObservationMatchGroup: public ScoredProcessingResult
    {
      std::set<ObservationMatchRef> observation_match_refs;

      bool allSameMolecule() const
      {
        // @TODO: return true or false for the empty set?
        if (observation_match_refs.size() <= 1) return true;
        const IdentifiedMolecule var =
          (*observation_match_refs.begin())->identified_molecule_var;
        for (auto it = ++observation_match_refs.begin();
             it != observation_match_refs.end(); ++it)
        {
          if (!((*it)->identified_molecule_var == var)) return false;
        }
        return true;
      }

      bool allSameQuery() const
      {
        // @TODO: return true or false for the empty set?
        if (observation_match_refs.size() <= 1) return true;
        ObservationRef ref = (*observation_match_refs.begin())->observation_ref;
        for (auto it = ++observation_match_refs.begin();
             it != observation_match_refs.end(); ++it)
        {
          if ((*it)->observation_ref != ref) return false;
        }
        return true;
      }

      bool operator==(const ObservationMatchGroup rhs) const
      {
        return ((rhs.observation_match_refs == observation_match_refs) &&
                (rhs.steps_and_scores == steps_and_scores));
      }

      bool operator!=(const ObservationMatchGroup& rhs) const
      {
        return !operator==(rhs);
      }
    };

    typedef boost::multi_index_container<
      ObservationMatchGroup,
      boost::multi_index::indexed_by<
        boost::multi_index::ordered_unique<
          boost::multi_index::member<ObservationMatchGroup, std::set<ObservationMatchRef>,
                                     &ObservationMatchGroup::observation_match_refs>>>
      > ObservationMatchGroups;
    typedef IteratorWrapper<ObservationMatchGroups::iterator> MatchGroupRef;
  }
}
