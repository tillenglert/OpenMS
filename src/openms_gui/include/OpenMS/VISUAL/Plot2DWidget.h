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

#pragma once

// OpenMS_GUI config
#include <OpenMS/VISUAL/OpenMS_GUIConfig.h>

// OpenMS
#include <OpenMS/VISUAL/PlotWidget.h>
#include <OpenMS/VISUAL/Plot2DCanvas.h>

class QGroupBox;
class QLabel;
class QCheckBox;

namespace OpenMS
{
  class Plot1DWidget;

  /**
      @brief Widget for 2D-visualization of peak map and feature map data

      The widget is composed of two scroll bars, two AxisWidget and a Plot2DCanvas as central widget.

      @image html Plot2DWidget.png

      The example image shows %Plot2DWidget displaying a peak layer and a feature layer.

      @ingroup PlotWidgets
  */
  class OPENMS_GUI_DLLAPI Plot2DWidget :
    public PlotWidget
  {
    Q_OBJECT
public:
    /// Main managed data type (experiment)
    typedef LayerDataBase::ExperimentSharedPtrType ExperimentSharedPtrType;

    /// Default constructor
    Plot2DWidget(const Param & preferences, QWidget * parent = nullptr);
    /// Destructor
    ~Plot2DWidget() override;

    /// This method is overwritten to make the class specific members accessible
    inline Plot2DCanvas * canvas()
    {
      return static_cast<Plot2DCanvas *>(canvas_);
    }

    /// const reference to the horizontal projection
    const Plot1DWidget * getHorizontalProjection() const;
    /// const reference to the vertical projection
    const Plot1DWidget * getVerticalProjection() const;

    /// Returns if one of the projections is visible (or both are visible)
    bool projectionsVisible() const;


public slots:
    // Docu in base class
    void recalculateAxes_() override;
    /// Shows/hides the projections
    void toggleProjections();
    /// Updates and shows the projections
    void updateProjections();
    // Docu in base class
    void showGoToDialog() override;

signals:
    /**
        @brief Signal emitted whenever the visible area changes.

        @param area The new visible area.
    */
    void visibleAreaChanged(DRange<2> area);
    /// Requests to display the spectrum with index @p index in 1D
    void showSpectrumAsNew1D(int index);
    void showChromatogramsAsNew1D(std::vector<int, std::allocator<int> > indices);
    /// Requests to display all spectra as 1D
    void showCurrentPeaksAs3D();

protected:
    /// Vertical projection widget
    Plot1DWidget * projection_vert_;
    /// Horizontal projection widget
    Plot1DWidget * projection_horz_;
    /// Group box that shows information about the projections
    QGroupBox * projection_box_;
    /// Number of peaks of the projection
    QLabel * projection_peaks_;
    /// Intensity sum of the projection
    QLabel * projection_sum_;
    /// Intensity maximum of the projection
    QLabel * projection_max_;
    /// Checkbox that indicates that projections should be automatically updated (with a slight delay)
    QCheckBox * projections_auto_;
    /// Timer that triggers auto-update of projections
    QTimer * projections_timer_;

private slots:
    /// shows the horizontal projection with the given data and draw mode
    void horizontalProjection(ExperimentSharedPtrType exp);
    /// shows the vertical projection with the given data and draw mode
    void verticalProjection(ExperimentSharedPtrType exp);
    /// shows projections information
    void projectionInfo(int peaks, double intensity, double max);
    /// slot that monitors the visible area changes and triggers the update of projections
    void autoUpdateProjections();
  };
}

