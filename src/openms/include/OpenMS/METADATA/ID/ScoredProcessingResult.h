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

#include <OpenMS/METADATA/ID/AppliedProcessingStep.h>

namespace OpenMS
{
  namespace IdentificationDataInternal
  {
    /// Base class for ID data with scores and processing steps (and meta info)
    struct ScoredProcessingResult: public MetaInfoInterface
    {
      AppliedProcessingSteps steps_and_scores;

      /// Return the applied processing steps (incl. scores) as a set ordered by processing step reference (option)
      AppliedProcessingSteps::nth_index<1>::type& getStepsAndScoresByStep()
      {
        return steps_and_scores.get<1>();
      }

      /// Return the applied processing steps (incl. scores) as a set ordered by processing step reference (option) - const variant
      const AppliedProcessingSteps::nth_index<1>::type&
      getStepsAndScoresByStep() const
      {
        return steps_and_scores.get<1>();
      }

      /*!
        @brief Add an applied processing step

        If the step already exists, scores are merged (existing ones updated).
      */
      void addProcessingStep(const AppliedProcessingStep& step)
      {
        auto step_pos =
          steps_and_scores.get<1>().find(step.processing_step_opt);
        if (step_pos == steps_and_scores.get<1>().end()) // new step
        {
          steps_and_scores.push_back(step);
        }
        else // existing step - add or update scores
        {
          steps_and_scores.get<1>().modify(
            step_pos, [&](AppliedProcessingStep& old_step)
            {
              for (const auto& pair : step.scores)
              {
                old_step.scores[pair.first] = pair.second;
              }
            });
        }
      }

      /// Add a processing step (and associated scores, if any)
      void addProcessingStep(ProcessingStepRef step_ref,
                             const std::map<ScoreTypeRef, double>& scores =
                             std::map<ScoreTypeRef, double>())
      {
        AppliedProcessingStep applied(step_ref, scores);
        addProcessingStep(applied);
      }

      /// Add a score (possibly connected to a processing step)
      void addScore(ScoreTypeRef score_type, double score,
                    const std::optional<ProcessingStepRef>&
                    processing_step_opt = std::nullopt)
      {
        AppliedProcessingStep applied(processing_step_opt);
        applied.scores[score_type] = score;
        addProcessingStep(applied);
      }

      /// Merge in data from another object
      ScoredProcessingResult& merge(const ScoredProcessingResult& other)
      {
        // merge applied processing steps and scores:
        for (const auto& step : other.steps_and_scores)
        {
          addProcessingStep(step);
        }
        // merge meta info - existing entries may be overwritten:
        addMetaValues(other);

        return *this;
      }

      /*!
        @brief Look up a score by score type

        All processing steps are considered, in "most recent first" order.

        @return A pair: score (or NaN), success indicator
      */
      std::pair<double, bool> getScore(ScoreTypeRef score_ref) const
      {
        std::tuple<double, std::optional<ProcessingStepRef>, bool> result =
          getScoreAndStep(score_ref);
        return std::make_pair(std::get<0>(result), std::get<2>(result));
      }

      /*!
        @brief Look up a score by score type and processing step

        @return A pair: score (or NaN), success indicator
      */
      std::pair<double, bool> getScore(ScoreTypeRef score_ref,
                                       std::optional<ProcessingStepRef>
                                       processing_step_opt) const
      {
        auto step_pos = steps_and_scores.get<1>().find(processing_step_opt);
        if (step_pos != steps_and_scores.get<1>().end())
        {
          auto score_pos = step_pos->scores.find(score_ref);
          if (score_pos != step_pos->scores.end())
          {
            return std::make_pair(score_pos->second, true);
          }
        }
        // not found:
        return std::make_pair(std::numeric_limits<double>::quiet_NaN(), false);
      }

      /*!
        @brief Look up a score and associated processing step by score type

        All processing steps are considered, in "most recent first" order.

        @return A tuple: score (or NaN), processing step reference (option), success indicator
      */
      std::tuple<double, std::optional<ProcessingStepRef>, bool>
      getScoreAndStep(ScoreTypeRef score_ref) const
      {
        // give priority to scores from later processing steps:
        for (const auto& step : boost::adaptors::reverse(steps_and_scores))
        {
          auto pos = step.scores.find(score_ref);
          if (pos != step.scores.end())
          {
            return std::make_tuple(pos->second, step.processing_step_opt, true);
          }
        }
        // not found:
        return std::make_tuple(std::numeric_limits<double>::quiet_NaN(),
                               std::nullopt, false);
      }

      /*!
        @brief Get the (primary) score from the most recent processing step

        This will fail if no scores have been assigned.

        @return A tuple: score (or NaN), score type reference (option), success indicator
        */
      std::tuple<double, std::optional<ScoreTypeRef>, bool>
      getMostRecentScore() const
      {
        // check steps starting with most recent:
        for (const auto& step : boost::adaptors::reverse(steps_and_scores))
        {
          auto top_score = step.getScoresInOrder(true);
          if (!top_score.empty())
          {
            return std::make_tuple(top_score[0].second, top_score[0].first,
                                   true);
          }
        }
        return std::make_tuple(std::numeric_limits<double>::quiet_NaN(),
                               std::nullopt, false); // no score available
      }

      /// Return the number of scores associated with this result
      Size getNumberOfScores() const
      {
        Size counter = 0;
        for (const auto& step : steps_and_scores)
        {
          counter += step.scores.size();
        }
        return counter;
      }

    protected:
      /// Constructor
      explicit ScoredProcessingResult(
        const AppliedProcessingSteps& steps_and_scores =
        AppliedProcessingSteps()):
        steps_and_scores(steps_and_scores)
      {
      }

      /// Copy c'tor
      ScoredProcessingResult(const ScoredProcessingResult&) = default;
    };

  }
}
