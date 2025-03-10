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
// $Maintainer: Chris Bielow $
// $Authors: Chris Bielow $
// --------------------------------------------------------------------------

#include <OpenMS/APPLICATIONS/TOPPBase.h>

#include <OpenMS/SYSTEM/StopWatch.h>

#include <OpenMS/ANALYSIS/DECHARGING/FeatureDeconvolution.h>
#include <OpenMS/KERNEL/FeatureMap.h>
#include <OpenMS/FORMAT/FileHandler.h>
// #include <OpenMS/FORMAT/FeatureXMLFile.h>
// #include <OpenMS/FORMAT/ConsensusXMLFile.h>
#include <OpenMS/KERNEL/ConsensusMap.h>

using namespace OpenMS;
using namespace std;

//-------------------------------------------------------------
//Doxygen docu
//-------------------------------------------------------------

/**
   @page TOPP_Decharger Decharger

   @brief Decharges a feature map by clustering charge variants of a peptide to zero-charge entities.
<CENTER>
    <table>
        <tr>
            <td ALIGN = "center" BGCOLOR="#EBEBEB"> pot. predecessor tools </td>
            <td VALIGN="middle" ROWSPAN=2> \f$ \longrightarrow \f$ Decharger \f$ \longrightarrow \f$</td>
            <td ALIGN = "center" BGCOLOR="#EBEBEB"> pot. successor tools </td>
        </tr>
        <tr>
      <td VALIGN="middle" ALIGN = "center" ROWSPAN=1> @ref TOPP_FeatureFinderCentroided </td>
            <td VALIGN="middle" ALIGN = "center" ROWSPAN=1> @ref TOPP_ProteinQuantifier</td>
        </tr>
    </table>
</CENTER>

   The Decharger uses an ILP approach to group charge variants of the same peptide, which
   usually occur in ESI ionization mode. The resulting zero-charge peptides, which are defined by RT and mass,
   are written to consensusXML. Intensities of charge variants are summed up. The position of the zero charge
   variant is the average of all clustered peptides in each dimension (m/z and RT).
   It is also possible to include adducted species to the charge ladders (see 'potential_adducts' parameter).
   Via this mechanism it is also possible to use this tool to find pairs/triples/quadruples/... in labeled data (by specifing the mass
   tag weight as an adduct). If mass tags induce an RT shift (e.g. deuterium labeled data) you can also specify this also in the adduct list.
   This will allow to tighten the RT search window, thus reducing false positive results.

  This tool is described in the following publication:

  Bielow C, Ruzek S, Huber CG, Reinert K. Optimal decharging and clustering of charge ladders generated in ESI-MS. J Proteome Res 2010; 9: 2688.<br>
  DOI: 10.1021/pr100177k

     <B>The command line parameters of this tool are:</B>
   @verbinclude TOPP_Decharger.cli
    <B>INI file documentation of this tool:</B>
    @htmlinclude TOPP_Decharger.html
*/

// We do not want this class to show up in the docu:
/// @cond TOPPCLASSES

class TOPPDecharger :
  virtual public TOPPBase
{
public:
  TOPPDecharger() :
    TOPPBase("Decharger", "Decharges and merges different feature charge variants of the same peptide.", true,
             { // citation(s), specific for this tool
              { "Bielow C, Ruzek S, Huber CG, Reinert K", "Optimal decharging and clustering of charge ladders generated in ESI-MS", "J Proteome Res 2010; 9: 2688", "10.1021/pr100177k" }
             }
            )
  {
  }

protected:
  void registerOptionsAndFlags_() override
  {
    registerInputFile_("in", "<file>", "", "input file ");
    setValidFormats_("in", ListUtils::create<String>("featureXML"));
    registerOutputFile_("out_cm", "<file>", "", "output consensus map");
    registerOutputFile_("out_fm", "<file>", "", "output feature map", false);
    registerOutputFile_("outpairs", "<file>", "", "output file", false);
    setValidFormats_("out_fm", ListUtils::create<String>("featureXML"));
    setValidFormats_("out_cm", ListUtils::create<String>("consensusXML"));
    setValidFormats_("outpairs", ListUtils::create<String>("consensusXML"));
    addEmptyLine_();
    registerSubsection_("algorithm", "Feature decharging algorithm section");
  }

  Param getSubsectionDefaults_(const String & /*section*/) const override
  {
    // there is only one subsection: 'algorithm' (s.a) .. and in it belongs the FeatureDecharger param
    FeatureDeconvolution fdc;
    Param tmp;
    tmp.insert("FeatureDeconvolution:", fdc.getParameters());
    return tmp;
  }

  ExitCodes main_(int, const char **) override
  {
    //-------------------------------------------------------------
    // parameter handling
    //-------------------------------------------------------------
    String infile = getStringOption_("in");
    String outfile_fm = getStringOption_("out_fm");
    String outfile_cm = getStringOption_("out_cm");
    String outfile_p = getStringOption_("outpairs");

    FeatureDeconvolution fdc;
    Param const & dc_param = getParam_().copy("algorithm:FeatureDeconvolution:", true);

    writeDebug_("Parameters passed to Decharger", dc_param, 3);

    fdc.setParameters(dc_param);

    //-------------------------------------------------------------
    // loading input
    //-------------------------------------------------------------

    writeDebug_("Loading input file", 1);

    typedef FeatureMap FeatureMapType;
    FeatureMapType map_in, map_out;
    FileHandler().loadFeatures(infile, map_in);

    //-------------------------------------------------------------
    // calculations
    //-------------------------------------------------------------
    ConsensusMap cm, cm2;
    StopWatch a;
    a.start();
    fdc.compute(map_in, map_out, cm, cm2);
    a.stop();
    //std::cerr << "took: " << a.getClockTime() << " seconds\n\n\n";

    //-------------------------------------------------------------
    // writing output
    //-------------------------------------------------------------

    writeDebug_("Saving output files", 1);

    cm.getColumnHeaders()[0].filename = infile;
    cm2.getColumnHeaders()[0].filename = infile;

    //annotate output with data processing info
    addDataProcessing_(map_out, getProcessingInfo_(DataProcessing::CHARGE_DECONVOLUTION));
    addDataProcessing_(cm, getProcessingInfo_(DataProcessing::CHARGE_DECONVOLUTION));
    addDataProcessing_(cm2, getProcessingInfo_(DataProcessing::CHARGE_DECONVOLUTION));


    FileHandler().storeConsensusFeatures(outfile_cm, cm);

    if (!outfile_p.empty())
    {
      FileHandler().storeConsensusFeatures(outfile_p, cm2);
    }
    if (!outfile_fm.empty())
    {
      FileHandler().storeFeatures(outfile_fm, map_out);
    }
    return EXECUTION_OK;
  }

};


int main(int argc, const char ** argv)
{
  TOPPDecharger tool;
  return tool.main(argc, argv);
}

/// @endcond
