/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __otbHistogramOfOrientedGradientCovariantImageFunction_txx
#define __otbHistogramOfOrientedGradientCovariantImageFunction_txx

#include "otbHistogramOfOrientedGradientCovariantImageFunction.h"
#include "itkNumericTraits.h"
#include "itkMacro.h"

namespace otb
{

/**
 * Constructor
 */
template <class TInputImage, class TOutputPrecision, class TCoordRep>
HistogramOfOrientedGradientCovariantImageFunction<TInputImage, TOutputPrecision, TCoordRep>
::HistogramOfOrientedGradientCovariantImageFunction() : m_NeighborhoodRadius(1),
							m_NumberOfOrientationBins(10)
{}

template <class TInputImage, class TOutputPrecision, class TCoordRep>
void
HistogramOfOrientedGradientCovariantImageFunction<TInputImage, TOutputPrecision, TCoordRep>
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << " Neighborhood radius value   : "  << m_NeighborhoodRadius << std::endl;
}

template <class TInputImage, class TOutputPrecision, class TCoordRep>
typename HistogramOfOrientedGradientCovariantImageFunction<TInputImage, TOutputPrecision,TCoordRep>::OutputType
HistogramOfOrientedGradientCovariantImageFunction<TInputImage, TOutputPrecision,TCoordRep>
::EvaluateAtIndex(const IndexType& index) const
{
  // Build output vector
  OutputType hog;
  
  // Check for input image
  if( !this->GetInputImage() )
    {
    return hog;
    }
  
  // Check for out of buffer
  if ( !this->IsInsideBuffer( index ) )
    {
    return hog;
    }
  
  // Compute the center bin radius
  unsigned int centerBinRadius = vcl_floor(vcl_log(m_NeighborhoodRadius)/vcl_log(2));

  // Define a gaussian kernel around the center location
  double squaredRadius = m_NeighborhoodRadius * m_NeighborhoodRadius;
  double squaredCenterBinRadius = centerBinRadius * centerBinRadius;
  double squaredSigma = 0.25 * squaredRadius;

  // Build a global orientation histogram
  std::vector<TOutputPrecision> globalOrientationHistogram(m_NumberOfOrientationBins,0.);

  // Compute the orientation bin width
  double orientationBinWidth = 2 * M_PI / m_NumberOfOrientationBins;

  // Build the global orientation histogram
  for(int i = -(int)m_NeighborhoodRadius;i< (int)m_NeighborhoodRadius;++i)
    {
    for(int j = -(int)m_NeighborhoodRadius;j< (int)m_NeighborhoodRadius;++j)
      {
      // Check if the current pixel lies within a disc of radius m_NeighborhoodRadius
      double currentSquaredRadius = i*i+j*j;
      if(currentSquaredRadius < squaredRadius)
	{
	// If so, compute the gaussian weighting (this could be
	// computed once for all for the sake of optimisation)
	double gWeight = (1/squaredSigma) * vcl_exp(- currentSquaredRadius/squaredSigma);

	// Compute pixel location
	IndexType currentIndex = index;
	currentIndex[0]+=i;
	currentIndex[1]+=j;

	// Get the current gradient covariant value
	InputPixelType gradient = this->GetInputImage()->GetPixel(currentIndex);

	// Then, compute the gradient orientation
	double angle = vcl_atan2(gradient[1],gradient[0]);

	// Also compute its magnitude
	TOutputPrecision magnitude = vcl_sqrt(gradient[0]*gradient[0]+gradient[1]*gradient[1]);

	// Determine the bin index (shift of M_PI since atan2 values
	// lies in [-pi,pi]
	unsigned int binIndex = vcl_floor((M_PI + angle)/orientationBinWidth);
	
	// Cumulate values
	globalOrientationHistogram[binIndex]+= magnitude * gWeight;
	}
      }
    }

  // Compute principal orientation. We also use this loop to compute
  // the normalisation factor with the L1 norm
  // TODO: Replace this with a stl algorithm
  double normalisationFactor = 1e-10;
  double maxOrientationHistogramValue = globalOrientationHistogram.at(0);
  unsigned int maxOrientationHistogramBin = 0;

  for(unsigned int i = 1; i < m_NumberOfOrientationBins;++i)
    {
    // Retrieve current value
    double currentValue = globalOrientationHistogram.at(i);

    // Update normalisation factor
    normalisationFactor+=currentValue;

    // Look for new maximum
    if(maxOrientationHistogramValue<currentValue)
      {
      maxOrientationHistogramValue = currentValue;
      maxOrientationHistogramBin = i;
      }
    }

  // Derive principal orientation
  double principalOrientation = maxOrientationHistogramBin * orientationBinWidth;

  // Derive normalisation factor
  normalisationFactor = 1/normalisationFactor;
  
  // Only for debug purpose
  std::cout<<"Principal orientation = "<<principalOrientation*180./M_PI<<std::endl;
  
  // Initialize the five spatial bins
  std::vector<TOutputPrecision> centerHistogram(m_NumberOfOrientationBins,0.);
  std::vector<TOutputPrecision> upperLeftHistogram(m_NumberOfOrientationBins,0.);
  std::vector<TOutputPrecision> upperRightHistogram(m_NumberOfOrientationBins,0.);
  std::vector<TOutputPrecision> lowerLeftHistogram(m_NumberOfOrientationBins,0.);
  std::vector<TOutputPrecision> lowerRightHistogram(m_NumberOfOrientationBins,0.);

  // Walk the image once more to fill the spatial bins
  for(int i = -(int)m_NeighborhoodRadius;i< (int)m_NeighborhoodRadius;++i)
    {
    for(int j = -(int)m_NeighborhoodRadius;j< (int)m_NeighborhoodRadius;++j)
      {
      // Check if the current pixel lies within a disc of radius m_NeighborhoodRadius
      double currentSquaredRadius = i*i+j*j;
      if(currentSquaredRadius < squaredRadius)
	{
	// If so, compute the gaussian weighting (this could be
	// computed once for all for the sake of optimisation)
	double gWeight = (1/squaredSigma) * vcl_exp(- currentSquaredRadius/squaredSigma);

	// Compute pixel location
	IndexType currentIndex = index;
	currentIndex[0]+=i;
	currentIndex[1]+=j;

	// Get the current gradient covariant value
	InputPixelType gradient = this->GetInputImage()->GetPixel(currentIndex);

	// Then, compute the compensated gradient orientation
	double angle = vcl_atan2(gradient[1],gradient[0]) - principalOrientation;

	// Angle is supposed to lie with [-pi,pi], so we ensure that
	// compenstation did not introduce out-of-range values
	if(angle > M_PI)
	  {
	  angle -= 2*M_PI;
	  }
	else if(angle < -M_PI)
	  {
	  angle += M_PI;
	  }

	// Also compute its magnitude
	TOutputPrecision magnitude = vcl_sqrt(gradient[0]*gradient[0]+gradient[1]*gradient[1]);

	// Determine the bin index (shift of M_PI since atan2 values
	// lies in [-pi,pi]
	unsigned int binIndex = vcl_floor((M_PI + angle)/orientationBinWidth);

	// Compute the angular position
	double angularPosition = vcl_atan2(j,i) - principalOrientation;

	// Check if we lie in center bin
	if(currentSquaredRadius < squaredCenterBinRadius)
	  {
	  centerHistogram[binIndex]+= magnitude * gWeight * normalisationFactor;
	  }
	else if(angularPosition > 0)
	  {
	  if(angularPosition < M_PI/2)
	    {
	    upperRightHistogram[binIndex]+= magnitude * gWeight * normalisationFactor;
	    }
	  else
	    {
	    upperLeftHistogram[binIndex]+= magnitude * gWeight * normalisationFactor;
	    }
	  }
	else
	  {
	  if(angularPosition > -M_PI/2)
	    {
	    lowerRightHistogram[binIndex]+= magnitude * gWeight * normalisationFactor;
	    }
	  else
	    {
	    lowerLeftHistogram[binIndex]+= magnitude * gWeight * normalisationFactor;
	    }
	  }
	
	// Cumulate values
	globalOrientationHistogram[binIndex]+= magnitude * gWeight * normalisationFactor;
	}
      }
    }

  // Build the final output
  hog.push_back(centerHistogram);
  hog.push_back(upperLeftHistogram);
  hog.push_back(upperRightHistogram);
  hog.push_back(lowerRightHistogram);
  hog.push_back(lowerLeftHistogram);

  // Return result
  return hog;
}

} // namespace otb

#endif
