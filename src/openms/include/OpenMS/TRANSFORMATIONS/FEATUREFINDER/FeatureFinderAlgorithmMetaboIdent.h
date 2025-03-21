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
// $Authors: Timo Sachsenberg, Hendrik Weisser $
// --------------------------------------------------------------------------

#pragma once

#include <OpenMS/ANALYSIS/TARGETED/TargetedExperiment.h>
#include <OpenMS/ANALYSIS/MAPMATCHING/TransformationDescription.h>
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureFinderScoring.h>
#include <OpenMS/CONCEPT/ProgressLogger.h>
#include <OpenMS/DATASTRUCTURES/DefaultParamHandler.h>
#include <OpenMS/KERNEL/MSExperiment.h>
#include <OpenMS/TRANSFORMATIONS/FEATUREFINDER/FeatureFinderAlgorithmPickedHelperStructs.h>
#include <OpenMS/CHEMISTRY/ISOTOPEDISTRIBUTION/CoarseIsotopePatternGenerator.h>

#include <map>
#include <vector>

namespace OpenMS
{
class IsotopeDistribution;
  
class OPENMS_DLLAPI FeatureFinderAlgorithmMetaboIdent :
  public DefaultParamHandler
{
public:
  /// @brief represents a compound in the assay library
  struct OPENMS_DLLAPI FeatureFinderMetaboIdentCompound 
  {    
    FeatureFinderMetaboIdentCompound(const String& _name, 
        const String& _formula, 
        double _mass, 
        const std::vector<int>& _charges, 
        const std::vector<double>& _rts, 
        const std::vector<double>& _rt_ranges, 
        const std::vector<double>& _iso_distrib):
      name(_name),
      formula(_formula),
      mass(_mass),
      charges(_charges),
      rts(_rts),
      rt_ranges(_rt_ranges),
      iso_distrib(_iso_distrib)
      {        
      }


    String name;
    String formula;
    double mass;
    std::vector<int> charges;
    std::vector<double> rts;
    std::vector<double> rt_ranges;
    std::vector<double> iso_distrib;
  };

  /// default constructor
  FeatureFinderAlgorithmMetaboIdent();

  /// @brief perform targeted feature extraction of compounds from @param metaboIdentTable and stores them in @param feature.
  /// If @p spectra_file is provided it will be used as a fall-back to setPrimaryMSRunPath
  /// in the feature map in case a proper primaryMSRunPath is not annotated in the MSExperiment.
  void run(const std::vector<FeatureFinderMetaboIdentCompound>& metaboIdentTable, FeatureMap& features, String spectra_file = "");

  /// @brief Retrieve chromatograms (empty if run was not executed)
  PeakMap& getMSData() { return ms_data_; }
  const PeakMap& getMSData() const { return ms_data_; }

  /// @brief Set spectra
  void setMSData(const PeakMap& m); // needed because pyOpenMS can't wrap the non-const reference version

  void setMSData(PeakMap&& m); // moves peak data and saves the copy. Note that getMSData() will give back a processed/modified version.

  /// @brief Retrieve chromatograms (empty if run was not executed)
  const PeakMap& getChromatograms() const { return chrom_data_; }
  PeakMap& getChromatograms() { return chrom_data_; }

  /// @brief Retrieve the assay library (e.g., to store as TraML, empty if run was not executed)
  const TargetedExperiment& getLibrary() const { return library_; }
  
  /// @brief Retrieve deviations between provided coordinates and extracted ones (e.g., to store as TrafoXML or for plotting)
  const TransformationDescription& getTransformations() const { return trafo_; }

  /// @brief Retrieve number of features with shared identifications
  size_t getNShared() const  { return n_shared_; }

  String prettyPrintCompound(const TargetedExperiment::Compound& compound);
protected:

  /// Boundaries for a mass trace in a feature
  struct MassTraceBounds
  {
    Size sub_index;
    double rt_min, rt_max, mz_min, mz_max;
  };

  /// Boundaries for all mass traces per feature
  typedef std::map<UInt64, std::vector<MassTraceBounds> > FeatureBoundsMap;

  typedef FeatureFinderAlgorithmPickedHelperStructs::MassTrace MassTrace;
  typedef FeatureFinderAlgorithmPickedHelperStructs::MassTraces MassTraces;

  typedef std::vector<Feature*> FeatureGroup; ///< group of (overlapping) features

  /// Predicate for filtering features by overall quality
  struct FeatureFilterQuality
  {
    bool operator()(const Feature& feature)
    {
      return feature.metaValueExists("FFMetId_remove");
    }
  } feature_filter_;

  /// Comparison functor for features
  struct FeatureCompare
  {
    bool operator()(const Feature& f1, const Feature& f2)
    {
      const String& ref1 = f1.getMetaValue("PeptideRef");
      const String& ref2 = f2.getMetaValue("PeptideRef");
      if (ref1 == ref2)
      {
        return f1.getRT() < f2.getRT();
      }
      return ref1 < ref2;
    }
  } feature_compare_;


  void extractTransformations_(const FeatureMap& features);

  /// Add a target (from the input file) to the assay library
  void addTargetToLibrary_(const String& name, const String& formula,
                           double mass, const std::vector<Int>& charges,
                           const std::vector<double>& rts,
                           std::vector<double> rt_ranges,
                           const std::vector<double>& iso_distrib);

  /// Add "peptide" identifications with information about targets to features
  Size addTargetAnnotations_(FeatureMap& features);

  void addTargetRT_(TargetedExperiment::Compound& target, double rt);

  /// Calculate mass-to-charge ratio from mass and charge
  double calculateMZ_(double mass, Int charge) const;

  void generateTransitions_(const String& target_id, double mz, Int charge,
                            const IsotopeDistribution& iso_dist);

  /// Check if two sets of mass trace boundaries overlap
  bool hasOverlappingBounds_(const std::vector<MassTraceBounds>& mtb1,
                             const std::vector<MassTraceBounds>& mtb2) const;

  void getFeatureBounds_(const FeatureMap& features,
                         FeatureBoundsMap& feature_bounds);


  bool hasOverlappingFeature_(const Feature& feature, const FeatureGroup& group, 
                              const FeatureBoundsMap& feature_bounds) const;

  void findOverlappingFeatures_(FeatureMap& features,
                                const FeatureBoundsMap& feature_bounds,
                                std::vector<FeatureGroup>& overlap_groups);

  void resolveOverlappingFeatures_(FeatureGroup& group,
                                   const FeatureBoundsMap& feature_bounds);

  void annotateFeatures_(FeatureMap& features);

  void ensureConvexHulls_(Feature& feature) const;

  void selectFeaturesFromCandidates_(FeatureMap& features);

  double rt_window_; ///< RT window width
  double mz_window_; ///< m/z window width
  bool mz_window_ppm_; ///< m/z window width is given in PPM (not Da)?

  double isotope_pmin_; ///< min. isotope probability for peptide assay
  Size n_isotopes_; ///< number of isotopes for peptide assay

  double peak_width_;
  double min_peak_width_;
  double signal_to_noise_;

  String elution_model_;

  // output file (before filtering)
  String candidates_out_;

  Size debug_level_;

  void updateMembers_() override;

  PeakMap ms_data_; ///< input LC-MS data
  PeakMap chrom_data_; ///< accumulated chromatograms (XICs)

  MRMFeatureFinderScoring feat_finder_; ///< OpenSWATH feature finder

  TargetedExperiment library_; ///< accumulated assays for targets
  
  TransformationDescription trafo_;
  
  CoarseIsotopePatternGenerator iso_gen_; ///< isotope pattern generator
  std::map<String, double> isotope_probs_; ///< isotope probabilities of transitions
  std::map<String, double> target_rts_; ///< RTs of targets (assays)
  
  size_t n_shared_ = 0;
};

} // namespace OpenMS
