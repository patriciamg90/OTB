/*
 * Copyright (C) 2005-2017 Centre National d'Etudes Spatiales (CNES)
 *
 * This file is part of Orfeo Toolbox
 *
 *     https://www.orfeo-toolbox.org/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "otbWrapperApplication.h"
#include "otbWrapperApplicationFactory.h"

#include "otbMultiChannelExtractROI.h"
#include "otbStandardFilterWatcher.h"
#include "otbWrapperNumericalParameter.h"
#include "otbWrapperListViewParameter.h"
#include "otbWrapperTypes.h"

#include "otbWrapperElevationParametersHandler.h"

#include "otbGenericRSTransform.h"

namespace otb
{
namespace Wrapper
{

class ExtractROI : public Application
{
public:
  /** Standard class typedefs. */
  typedef ExtractROI                          Self;
  typedef Application                         Superclass;
  typedef itk::SmartPointer<Self>             Pointer;
  typedef itk::SmartPointer<const Self>       ConstPointer;

  typedef otb::GenericRSTransform<>           RSTransformType;
  typedef RSTransformType::InputPointType     Point3DType;

  /** Standard macro */
  itkNewMacro(Self);

  itkTypeMacro(ExtractROI, otb::Application);

  /** Filters typedef */
  typedef otb::MultiChannelExtractROI<FloatVectorImageType::InternalPixelType,
                                      FloatVectorImageType::InternalPixelType> ExtractROIFilterType;

private:
  void DoInit() ITK_OVERRIDE
  {
    SetName("ExtractROI");
    SetDescription("Extract a ROI defined by the user.");

    // Documentation
    SetDocName("Extract ROI");
    SetDocLongDescription("This application extracts a Region Of Interest with user defined size, or reference image.");
    SetDocLimitations("None");
    SetDocAuthors("OTB-Team");
    SetDocSeeAlso(" ");

    AddDocTag(Tags::Manip);

    AddParameter(ParameterType_InputImage,  "in",   "Input Image");
    SetParameterDescription("in", "Input image.");
    AddParameter(ParameterType_OutputImage, "out",  "Output Image");
    SetParameterDescription("out", "Output image.");

    AddRAMParameter();

    AddParameter(ParameterType_Choice,"mode","Extraction mode");
    AddChoice("mode.standard","Standard");
    SetParameterDescription("mode.standard","In standard mode, extract is done according the coordinates entered by the user");
    AddChoice("mode.fit","Fit");
    SetParameterDescription("mode.fit","In fit mode, extract is made to best fit a reference image.");
    AddChoice( "mode.extent" , "Extent" );
    SetParameterDescription( "mode.extent" , "In progress" );

    AddParameter( ParameterType_InputImage , "mode.fit.ref" , "Reference image" );
    SetParameterDescription( "mode.fit.ref" , "Reference image to define the ROI" );

    AddParameter( ParameterType_Float , "mode.extent.ulx" , "Up left X" );
    SetParameterDescription( "mode.extent.ulx" , "ROI x coordinate of upper left corner point." );
    AddParameter( ParameterType_Float , "mode.extent.uly" , "Up left Y" );
    SetParameterDescription( "mode.extent.uly" , "ROI y coordinate of upper left corner point." );
    AddParameter( ParameterType_Float , "mode.extent.lrx" , "Down right X" );
    SetParameterDescription( "mode.extent.lrx" , "ROI x coordinate of lower right corner point." );
    AddParameter( ParameterType_Float , "mode.extent.lry" , "Down right Y" );
    SetParameterDescription( "mode.extent.lry" , "ROI y coordinate of lower right corner point." );

    AddParameter( ParameterType_Choice , "mode.extent.unit" , "Unit" );
    AddChoice( "mode.extent.unit.pixel" , "Pixel" );
    AddChoice( "mode.extent.unit.phy" , "Physical" );
    AddChoice( "mode.extent.unit.lonlat" , "Lon/Lat" );

    // Elevation
    ElevationParametersHandler::AddElevationParameters(this,"mode.fit.elev");

    AddParameter(ParameterType_Int,  "startx", "Start X");
    SetParameterDescription("startx", "ROI start x position.");
    AddParameter(ParameterType_Int,  "starty", "Start Y");
    SetParameterDescription("starty", "ROI start y position.");
    AddParameter(ParameterType_Int,  "sizex",  "Size X");
    SetParameterDescription("sizex","size along x in pixels.");
    AddParameter(ParameterType_Int,  "sizey",  "Size Y");
    SetParameterDescription("sizey","size along y in pixels.");

    // Default values
    SetDefaultParameterInt("startx", 0);
    SetDefaultParameterInt("starty", 0);
    SetDefaultParameterInt("sizex",  0);
    SetDefaultParameterInt("sizey",  0);

    SetDefaultParameterInt("mode.extent.ulx", 0);
    SetDefaultParameterInt("mode.extent.uly", 0);
    SetDefaultParameterInt("mode.extent.lrx",  0);
    SetDefaultParameterInt("mode.extent.lry",  0);

    // Channelist Parameters
    AddParameter(ParameterType_ListView,  "cl", "Output Image channels");
    SetParameterDescription("cl","Channels to write in the output image.");

    // Doc example parameter settings
    SetDocExampleParameterValue("in", "VegetationIndex.hd");
    SetDocExampleParameterValue("startx", "40");
    SetDocExampleParameterValue("starty", "250");
    SetDocExampleParameterValue("sizex", "150");
    SetDocExampleParameterValue("sizey", "150");
    SetDocExampleParameterValue("out", "ExtractROI.tif");

    SetOfficialDocLink();
  }

  void DoUpdateParameters() ITK_OVERRIDE
  {
    // Update the sizes only if the user has not defined a size
    if ( HasValue("in") )
      {
      ExtractROIFilterType::InputImageType* inImage = GetParameterImage("in");
      ExtractROIFilterType::InputImageType::RegionType  largestRegion = inImage->GetLargestPossibleRegion();

      if (!HasUserValue("sizex")  && !HasUserValue("sizey") )
        {
        SetParameterInt("sizex",largestRegion.GetSize()[0], false);
        SetParameterInt("sizey",largestRegion.GetSize()[1], false);
        }

      unsigned int nbComponents = inImage->GetNumberOfComponentsPerPixel();
      ListViewParameter *clParam = dynamic_cast<ListViewParameter*>(GetParameterByKey("cl"));
      // Update the values of the channels to be selected if nbComponents is changed
      if (clParam != ITK_NULLPTR && clParam->GetNbChoices() != nbComponents)
        {

        ClearChoices("cl");
        for (unsigned int idx = 0; idx < nbComponents; ++idx)
          {
          std::ostringstream key, item;
          key<<"cl.channel"<<idx+1;
          item<<"Channel"<<idx+1;
          AddChoice(key.str(), item.str());
          }
        }

      // Put the limit of the index and the size relative the image
      SetMinimumParameterIntValue("sizex", 0);
      SetMaximumParameterIntValue("sizex", largestRegion.GetSize(0));

      SetMinimumParameterIntValue("sizey", 0);
      SetMaximumParameterIntValue("sizey", largestRegion.GetSize(1));

      SetMinimumParameterIntValue("startx", 0);
      SetMaximumParameterIntValue("startx", largestRegion.GetSize(0));

      SetMinimumParameterIntValue("starty", 0);
      SetMaximumParameterIntValue("starty", largestRegion.GetSize(1));

      // Crop the roi region to be included in the largest possible
      // region
      if (GetParameterString("mode")=="extent")
        {
          computeStartandSize();
        }
      if(!this->CropRegionOfInterest())
        {
        // Put the index of the ROI to origin and try to crop again
        SetParameterInt("startx",0, false);
        SetParameterInt("starty",0, false);
        this->CropRegionOfInterest();
        }
      }

    if(GetParameterString("mode")=="fit")
      {
      this->SetParameterRole("startx",Role_Output);
      this->SetParameterRole("starty",Role_Output);
      this->SetParameterRole("sizex",Role_Output);
      this->SetParameterRole("sizey",Role_Output);

      this->DisableParameter("startx");
      this->DisableParameter("starty");
      this->DisableParameter("sizex");
      this->DisableParameter("sizey");

      }
    else if(GetParameterString("mode")=="standard" || GetParameterString("mode")=="extent")
      {
      this->SetParameterRole("startx",Role_Input);
      this->SetParameterRole("starty",Role_Input);
      this->SetParameterRole("sizex",Role_Input);
      this->SetParameterRole("sizey",Role_Input);

      this->EnableParameter("startx");
      this->EnableParameter("starty");
      this->EnableParameter("sizex");
      this->EnableParameter("sizey");
      }
  }

  bool CropRegionOfInterest()
  {
    FloatVectorImageType::RegionType region;
    region.SetSize(0,  GetParameterInt("sizex"));
    region.SetSize(1,  GetParameterInt("sizey"));
    region.SetIndex(0, GetParameterInt("startx"));
    region.SetIndex(1, GetParameterInt("starty"));

    if ( HasValue("in") )
      {
        if (region.Crop(GetParameterImage("in")->GetLargestPossibleRegion()))
          {
            SetParameterInt("sizex",region.GetSize(0), HasUserValue("sizex"));
            SetParameterInt("sizey",region.GetSize(1), HasUserValue("sizey"));
            SetParameterInt("startx",region.GetIndex(0), HasUserValue("startx"));
            SetParameterInt("starty",region.GetIndex(1), HasUserValue("starty"));
            return true;
          }
      }
    return false;
  }

  void
  computeStartandSize()
  {
    assert( GetParameterString( "mode" ) != "mode.extent" );
    int pixelValue;
    if (GetParameterString( "mode.extent.unit" ) == "pixel" )
      {
        pixelValue = floor( GetParameterFloat( "mode.extent.ulx" ) );
        SetParameterInt( "startx", pixelValue , false );
        pixelValue = floor( GetParameterFloat( "mode.extent.lrx" ) - pixelValue ) + 1;
        // assert( pixelValue >= 0 );
        SetParameterInt( "sizex", pixelValue , false );
        pixelValue = floor( GetParameterFloat( "mode.extent.uly" ) );
        SetParameterInt( "starty", pixelValue , false );
        pixelValue = floor( GetParameterFloat( "mode.extent.lry" ) - pixelValue ) + 1;
        // assert( pixelValue >= 0 );
        SetParameterInt( "sizey", pixelValue , false );
      }
    else if(GetParameterString( "mode.extent.unit" ) == "phy")
      {
        itk::Point<double, 2> origin = GetParameterImage("in")->GetOrigin();
        FloatVectorImageType::SpacingType spacing = GetParameterImage("in")->GetSpacing();
        pixelValue = floor( ( GetParameterFloat( "mode.extent.ulx" ) - origin[ 0 ] ) \
                      / static_cast<float>( spacing[ 0 ] ) ) ;
         // assert( pixelValue >= 0 );
        SetParameterInt( "startx", pixelValue , false );
        pixelValue = floor( ( GetParameterFloat( "mode.extent.lrx" ) - \
                      GetParameterFloat( "mode.extent.ulx" ) ) / static_cast<float>( spacing[ 0 ] ) ) - pixelValue ;
         // assert( pixelValue >= 0 );
        SetParameterInt( "sizex", pixelValue , false );
        pixelValue = floor( ( GetParameterFloat( "mode.extent.uly" ) - origin[ 1 ] ) \
                      / static_cast<float>( spacing[ 1 ] ) ) ;
         // assert( pixelValue >= 0 );
        SetParameterInt( "starty", pixelValue , false );
        pixelValue = floor( ( GetParameterFloat( "mode.extent.lry" ) - \
                      GetParameterFloat( "mode.extent.uly" ) ) / static_cast<float>( spacing[ 1 ] ) ) - pixelValue ;
         // assert( pixelValue >= 0 );
        SetParameterInt( "sizey", pixelValue , false );
      }
    else
    {
      std::cout<<"work in progress"<<std::endl;
    }
    
  }

  void DoExecute() ITK_OVERRIDE
  {
    ExtractROIFilterType::InputImageType* inImage = GetParameterImage("in");
    inImage->UpdateOutputInformation();


    if(GetParameterString("mode")=="fit")
      {
      // Setup the DEM Handler
      otb::Wrapper::ElevationParametersHandler::SetupDEMHandlerFromElevationParameters(this,"mode.fit.elev");

      FloatVectorImageType::Pointer referencePtr = this->GetParameterImage("mode.fit.ref");
      referencePtr->UpdateOutputInformation();

      RSTransformType::Pointer rsTransform = RSTransformType::New();
      rsTransform->SetInputKeywordList(referencePtr->GetImageKeywordlist());
      rsTransform->SetInputProjectionRef(referencePtr->GetProjectionRef());
      rsTransform->SetOutputKeywordList(inImage->GetImageKeywordlist());
      rsTransform->SetOutputProjectionRef(inImage->GetProjectionRef());
      rsTransform->InstantiateTransform();

      FloatVectorImageType::IndexType uli_ref,uri_ref,lli_ref,lri_ref;

      uli_ref = referencePtr->GetLargestPossibleRegion().GetIndex();
      uri_ref = uli_ref;
      uri_ref[0]+=referencePtr->GetLargestPossibleRegion().GetSize()[0];
      lli_ref = uli_ref;
      lli_ref[1]+=referencePtr->GetLargestPossibleRegion().GetSize()[1];
      lri_ref = lli_ref;
      lri_ref[0]+=referencePtr->GetLargestPossibleRegion().GetSize()[0];

      FloatVectorImageType::PointType ulp_ref,urp_ref,llp_ref,lrp_ref;

      referencePtr->TransformIndexToPhysicalPoint(uli_ref,ulp_ref);
      referencePtr->TransformIndexToPhysicalPoint(uri_ref,urp_ref);
      referencePtr->TransformIndexToPhysicalPoint(lli_ref,llp_ref);
      referencePtr->TransformIndexToPhysicalPoint(lri_ref,lrp_ref);

      FloatVectorImageType::PointType ulp_out, urp_out, llp_out,lrp_out;

      ulp_out = rsTransform->TransformPoint(ulp_ref);
      urp_out = rsTransform->TransformPoint(urp_ref);
      llp_out = rsTransform->TransformPoint(llp_ref);
      lrp_out = rsTransform->TransformPoint(lrp_ref);

      FloatVectorImageType::IndexType uli_out, uri_out, lli_out, lri_out;

      inImage->TransformPhysicalPointToIndex(ulp_out,uli_out);
      inImage->TransformPhysicalPointToIndex(urp_out,uri_out);
      inImage->TransformPhysicalPointToIndex(llp_out,lli_out);
      inImage->TransformPhysicalPointToIndex(lrp_out,lri_out);

      FloatVectorImageType::IndexType uli, lri;

      uli[0] = std::min(std::min(uli_out[0],uri_out[0]),std::min(lli_out[0],lri_out[0]));
      uli[1] = std::min(std::min(uli_out[1],uri_out[1]),std::min(lli_out[1],lri_out[1]));

      lri[0] = std::max(std::max(uli_out[0],uri_out[0]),std::max(lli_out[0],lri_out[0]));
      lri[1] = std::max(std::max(uli_out[1],uri_out[1]),std::max(lli_out[1],lri_out[1]));

      SetParameterInt("startx",uli[0]);
      SetParameterInt("starty",uli[1]);
      SetParameterInt("sizex",lri[0]-uli[0]);
      SetParameterInt("sizey",lri[1]-uli[1]);

      this->CropRegionOfInterest();
      }


    m_ExtractROIFilter = ExtractROIFilterType::New();
    m_ExtractROIFilter->SetInput(inImage);
    m_ExtractROIFilter->SetStartX(GetParameterInt("startx"));
    m_ExtractROIFilter->SetStartY(GetParameterInt("starty"));
    m_ExtractROIFilter->SetSizeX(GetParameterInt("sizex"));
    m_ExtractROIFilter->SetSizeY(GetParameterInt("sizey"));

    for (unsigned int idx = 0; idx < GetSelectedItems("cl").size(); ++idx)
      {
      m_ExtractROIFilter->SetChannel(GetSelectedItems("cl")[idx] + 1 );
      }

    SetParameterOutputImage("out", m_ExtractROIFilter->GetOutput());
  }

  ExtractROIFilterType::Pointer   m_ExtractROIFilter;

};

}
}

OTB_APPLICATION_EXPORT(otb::Wrapper::ExtractROI)
